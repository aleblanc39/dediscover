#include "TimeVaryingFunction.h"

#include <algorithms/ConcreteParameters.h>
#include <base/CBIMDefinitions.h>
#include <base/XDEUtil.h>

#include "TVFunctionException.h"
#include <boost/make_shared.hpp>
#include <cmath>

using namespace std;

TimeVaryingFunction::TimeVaryingFunction() {
    disabled = false;
    minTimePoint = XDEUtil::XDE_INF;
    maxTimePoint = -XDEUtil::XDE_INF;
    outOfRangeValue = XDEUtil::XDE_NAN;
    useOutOfRangeFromControl = true;
    _containsVariableControls = true;
    initialized = false;

    addToControlParameters(std::make_shared<DoubleParameter>(
        getOutOfRangeValueOption(), XDEUtil::XDE_NAN, 0, 0, false, false,
        "Out of range value to use"));
}

TimeVaryingFunction::~TimeVaryingFunction() {}

void TimeVaryingFunction::reset() {
    controlPoints.clear();
    singleParameters.clear();
    basisCoeffs.clear();
    initialized = false;
}

double TimeVaryingFunction::compute(double t) {
    // muParser is a bit too agressive in computing the function.
    // Unless explicitely initialized, will just return 0

    if (!initialized) {
        //	cerr << "Unitialized tvf...\n";
        return 0;
        // initialize();
        // initialized = true;
    }
    double ret;
    if (std::isnan(t) || disabled || t > maxTimePoint || t < minTimePoint)
        // return outOfRangeValue;
        ret = outOfRangeValue;
    else
        ret = computeFunction(t);
    // cerr << "Returning from TVF: " << ret << endl;
    return ret;
}

void TimeVaryingFunction::addControlPoint(ControlPoint &cp) {
    controlPoints.push_back(cp);
}

void TimeVaryingFunction::addControlPoints(const TDoubleVector &t,
                                           const TDoubleVector &v) {
    XDE_ASSERT(t.size() == v.size());
    unsigned nbValues = t.size();
    for (unsigned i = 0; i < nbValues; i++) {
        if (!std::isnan(t[i]) && !std::isnan(v[i]))
            controlPoints.push_back(ControlPoint(t[i], v[i]));
    }
}

void TimeVaryingFunction::addControlPoint(TimeVaryingEntry a,
                                          TimeVaryingEntry b) {
    controlPoints.push_back(ControlPoint(a, b));
}

/**
 * These are the arguments such as Degree of Freedom, as opposed to
 * control points, which are pairs.
 */

void TimeVaryingFunction::addSingleArgument(const TimeVaryingEntry &tve) {
    singleParameters.push_back(tve);
}

void TimeVaryingFunction::addBasisCoeff(const TimeVaryingEntry &tve) {
    basisCoeffs.push_back(tve);
}

/**
 * Will map the variables in the control points to their locations
 * using the information from the memory synchronizer.
 * Will also map the out of range value.
 *
 * TODO: This is probably the wrong name. Should be part of the initialize
 * process.
 */

void TimeVaryingFunction::mapVariables() {
    mappedControlPoints.clear();
    cerr << "TimeVaryingFunction::mapVariables() # mapped control points: "
         << controlPoints.size() << endl;
    for (auto cp : controlPoints) {
        string s1 = boost::apply_visitor(ControlPointModifier(), cp.first);
        string s2 = boost::apply_visitor(ControlPointModifier(), cp.second);

        // Assert specifies that it's ok to have null memorySynchronizer as long
        // as both s1 and s2 are doubles.
        XDE_ASSERT(
            !(memorySynchronizer.get() == NULL && (s1 != "" || s2 != "")));
        // 	    throw
        // XDEProgrammingException("TimeVaryingFunction.cc::mapVariables -
        // trying to map variables without a memory synchronizer.");

        // MappedControlPoints are either doubles or pointers to double.

        mappedControlPoints.push_back(MappedControlPoint(
            (s1 == "") ? cp.first : (memorySynchronizer->getLocation(s1)),
            (s2 == "") ? cp.second : (memorySynchronizer->getLocation(s2))));
    }

    // Mapping the out of range value. Several possibilities:
    // This function is mapped, and the value is taken from the control
    // parameter. This function was read from the parser, and either:
    //  1) argument for default value was not used, and use the control
    //  parameter. 2) argument was used, and can be either a symbol or number.
    //  The variant will type will
    //     handle both these cases.

    if (useOutOfRangeFromControl) {
        outOfRangeValue = getDoubleParameterValue(getOutOfRangeValueOption());
            // ((DoubleParameter *)getCtrlParameter(getOutOfRangeValueOption())
            //      .get())
            //     ->getValue();
    } else {
        string s3 =
            boost::apply_visitor(ControlPointModifier(), outOfRangeEntry);
        mappedOutOfRangeEntry = (s3 == "")
                                    ? outOfRangeEntry
                                    : (memorySynchronizer->getLocation(s3));
        outOfRangeValue =
            boost::apply_visitor(ControlPointVisitor(), mappedOutOfRangeEntry);
    }
}

/**
 * Need to store the list of symbols used in the function since some
 * information about them will not be available while the function is
 * being created.
 */
void TimeVaryingFunction::addSymbolUsed(std::string s) {
    symbolsUsed.insert(s);
}

std::vector<std::string> TimeVaryingFunction::getSymbolsUsed() {
    vector<string> ret(symbolsUsed.begin(), symbolsUsed.end());
    return ret;
}

/**
 * Extracts the points into a vector and sorts them. If allowRepeats
 * is false then the duplicates are removed. In this case only the
 * latest one is saved.
 */

/**
 * Structure to sort the points. Will be first sorted by time, by
 * inverse index. When repeats are not allowed only keep the first.
 */
struct IndexedPoint {
    IndexedPoint(double t, double v) : time(t), value(v) {}
    double time;
    double value;
};

static bool compareIndexedPoints(IndexedPoint a, IndexedPoint b) {
    return (double_less_than(a.time, b.time));
}

/**
 * Get the timepoints being used. Use the DISALLOW_REPEATS policy to
 * remove duplicates. Maybe too much work here, but was fast to
 * implement since extratSortedPoints was already written.
 */

TDoubleVector TimeVaryingFunction::getUsedTimepoints() {
    TDoubleVector retTimes;
    TDoubleVector retValues;
    extractSortedPoints(retTimes, retValues, AVERAGE_REPEATS);
    TDoubleVector::iterator tend =
        std::remove_if(retTimes.begin(), retTimes.end(), [](double d) { return std::isnan(d); });
    retTimes.resize(retTimes.size() - (retTimes.end() - tend));
    return retTimes;
}

/**
 * Take all the control points and put them in two arrays, one for the
 * time and one for the value. They will be sorted by time. If repeats
 * are disallowed then the last one for a given time is retained,
 * unless the latter one has a NaN value.
 */

void TimeVaryingFunction::extractSortedPoints(TDoubleVector &retTimes,
                                              TDoubleVector &retValues,
                                              RepeatPolicy repeatPolicy) {
    retTimes.clear();
    retValues.clear();

    vector<double> times;
    vector<double> values;
    unsigned nbPoints = mappedControlPoints.size();
    if (nbPoints == 0) return;

    vector<IndexedPoint> indexedPoints;
    indexedPoints.reserve(nbPoints);

    for (unsigned i = 0; i < nbPoints; i++) {
        indexedPoints.push_back(
            IndexedPoint(boost::apply_visitor(ControlPointVisitor(),
                                              mappedControlPoints[i].first),
                         boost::apply_visitor(ControlPointVisitor(),
                                              mappedControlPoints[i].second)));
    }
    stable_sort(indexedPoints.begin(), indexedPoints.end(),
                compareIndexedPoints);

    // Remove all the NaNs.
    indexedPoints.erase(std::remove_if(indexedPoints.begin(), indexedPoints.end(), [](const IndexedPoint &pt){return std::isnan(pt.time) || std::isnan(pt.value);}), indexedPoints.end());
    // Copy the sorted values in the returned arrays. In case of
    // repeats, keep only the last one.

    times.reserve(indexedPoints.size());
    values.reserve(indexedPoints.size());

    // TODO Redo the look so there's not a case statement at each iteration.

    vector<unsigned> nbRepeats;  // Will be used only for AVERAGE_REPEATS
    times.push_back(indexedPoints[0].time);
    values.push_back(indexedPoints[0].value);
    unsigned cntr = 0;
    nbRepeats.push_back(1);
    for (unsigned i = 1; i < indexedPoints.size(); i++) {
        switch (repeatPolicy) {
            case ALLOW_REPEATS:
                times.push_back(indexedPoints[i].time);
                values.push_back(indexedPoints[i].value);
                cntr++;
                break;

            case AVERAGE_REPEATS:
                if (double_equals(indexedPoints[i].time,
                                  indexedPoints[i - 1].time)) {
                    values.back() += indexedPoints[i].value;
                    nbRepeats.back()++;
                } else {
                    times.push_back(indexedPoints[i].time);
                    values.push_back(indexedPoints[i].value);
                    nbRepeats.push_back(1);
                    cntr++;
                }
                break;

            case DISALLOW_REPEATS:
                if (double_equals(indexedPoints[i].time,
                                  indexedPoints[i - 1].time)) {
                    throw TVFunctionException(
                        string("ERROR: Time varying function ") +
                        functionString +
                        " does not allow duplicate timepoints.");
                } else {
                    times.push_back(indexedPoints[i].time);
                    values.push_back(indexedPoints[i].value);
                    cntr++;
                }
                break;
            default:
                throw XDEProgrammingException(
                    "Unknown repeat policy for time varying function.");
        }
    }

    if (repeatPolicy == AVERAGE_REPEATS) {
        for (unsigned i = 0; i < cntr + 1; i++) values[i] /= nbRepeats[i];
    }

    retTimes.resize(cntr + 1);
    retValues.resize(cntr + 1);
    copy(times.begin(), times.end(), retTimes.begin());
    copy(values.begin(), values.end(), retValues.begin());
}

void TimeVaryingFunction::setMinAndMaxTimepoints() {
    minTimePoint = XDEUtil::XDE_INF;
    maxTimePoint = -XDEUtil::XDE_INF;

    unsigned nbPoints = mappedControlPoints.size();
    if (nbPoints == 0) return;

    for (unsigned i = 0; i < nbPoints; i++) {
        double currentTime = boost::apply_visitor(ControlPointVisitor(),
                                                  mappedControlPoints[i].first);

        if (currentTime < minTimePoint) minTimePoint = currentTime;
        if (currentTime > maxTimePoint) maxTimePoint = currentTime;
    }
}

void TimeVaryingFunction::initialize() {
    mapVariables();
    initializeSpecific();
    setMinAndMaxTimepoints();
    initialized = true;
}
