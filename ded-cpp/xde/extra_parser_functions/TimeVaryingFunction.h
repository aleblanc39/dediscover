#ifndef TIMEVARYINGFUNCTION
#define TIMEVARYINGFUNCTION

#include <algorithms/XDEAlgorithm.h>
#include <base/XDEUtil.h>
#include <expressions/MemorySynchronizer.h>


#include <boost/variant.hpp>
#include <set>
#include <vector>

class TimeVaryingFunction;
typedef std::shared_ptr<TimeVaryingFunction> TimeVaryingFunctionPtr;

typedef boost::variant<double, double *, std::string> TimeVaryingEntry;
typedef boost::variant<double, double *, std::string> MappedTimeVaryingEntry;

/**
 * \brief Visitor class for the control points.
 *
 * The control points can either be a double or a pointer to double. The class
 * will return the double value or the double content.
 */
class ControlPointVisitor : public boost::static_visitor<double> {
   public:
    double operator()(double d) const { return d; }
    double operator()(double *p) const { return *p; }
    double operator()(std::string ) const {
        throw XDEException(
            "The control point visitor should not contain a string.");
    }
};

/**
 * \brief Visitor class to recognize when the argument is a string in a visitor.
 *
 * The class returns an empty string if an argument
 * is a double or a pointer to a double, and the string if the
 * argument is a string.
 *
 */

class ControlPointModifier : public boost::static_visitor<std::string> {
   public:
    std::string operator()(double ) const { return ""; }
    std::string operator()(double *) const { return ""; }

    std::string operator()(std::string s) const { return s; }
};

/**
 * \brief Pure virtual at the base class for all the time varying
 * functions.
 *
 * It will abstract the process of initializing the
 * function with a series of pairs <time, value> and of computing at a
 * specific time t.
 *
 * Each time or value can be either a fixed value or a pointer to
 * a double. The reason for the latter is to allow to set a pointer to
 * where a model parameter is stored, making it easier for the user of
 * this class to perform the work.
 */

class TimeVaryingFunction : public XDEAlgorithm {
   public:
    typedef std::pair<TimeVaryingEntry, TimeVaryingEntry> ControlPoint;
    typedef std::pair<MappedTimeVaryingEntry, MappedTimeVaryingEntry>
        MappedControlPoint;

    virtual ~TimeVaryingFunction();

    TimeVaryingFunction();

    TimeVaryingFunction(std::string fs) : functionString(fs) {
        fromEquations = false;
        disabled = false;
        minTimePoint = XDEUtil::XDE_INF;
        maxTimePoint = -XDEUtil::XDE_INF;
        outOfRangeValue = XDEUtil::XDE_NAN;
        useOutOfRangeFromControl = true;
        initialized = false;
        // Set to true for now, as it it safer. Can change default to false
        // later when everyting else works.
        _containsVariableControls = true;
    }

    TimeVaryingFunction(std::string functionString, MemorySynchronizerPtr mem)
        : functionString(functionString), memorySynchronizer(mem) {
        fromEquations = false;
        disabled = false;
        minTimePoint = XDEUtil::XDE_INF;
        maxTimePoint = -XDEUtil::XDE_INF;
        outOfRangeValue = XDEUtil::XDE_NAN;
        useOutOfRangeFromControl = true;
        initialized = false;
        // Set to true for now, as it it safer. Can change default to false
        // later when everything else works.
        _containsVariableControls = true;
    }

    virtual double compute(double t);
    virtual double computeFunction(const double &x) = 0;

    virtual void reset();
    void addControlPoint(ControlPoint &cp);  // Why not a const here?
    void addControlPoint(TimeVaryingEntry a, TimeVaryingEntry b);

    // For simplicity right now this function will only accept vectors
    // of double. Will generalize to accept vectors of
    // TimeVaryingEntry's if there is a need.
    // Will ignore the entries where either the timepoint or the data
    // is NaN
    void addControlPoints(const TDoubleVector &t, const TDoubleVector &v);

    void addSingleArgument(const TimeVaryingEntry &tve);
    void addBasisCoeff(const TimeVaryingEntry &tve);

    void setVariable(std::string v) {
        variable = v;
        addSymbolUsed(v);
    }
    std::string getVariable() { return variable; }

    void setFunctionString(std::string s) { functionString = s; }

    // void mapVariables(MemorySynchronizerPtr mem);
    void mapVariables();
    std::vector<std::string> getSymbolsUsed();

    const auto  &getFunctionString() { return functionString; }
    void addSymbolUsed(std::string s);
    void initialize();

    void setMemorySynchronizer(MemorySynchronizerPtr m) {
        memorySynchronizer = m;
    }
    MemorySynchronizerPtr getMemorySynchronizer() { return memorySynchronizer; }

    void enableFunction() { disabled = false; }
    void disableFunction() { disabled = true; }
    bool isDisabled() { return disabled; }


    TDoubleVector getUsedTimepoints();

    bool containsVariableControls() { return _containsVariableControls; }

    void setContainsVariableControls(bool b) { _containsVariableControls = b; }

    void setFromEquations(bool b) { fromEquations = b; }
    bool isFromEquations() { return fromEquations; }

   protected:
    typedef enum {
        DISALLOW_REPEATS,
        ALLOW_REPEATS,
        AVERAGE_REPEATS
    } RepeatPolicy;

    virtual void initializeSpecific() = 0;  // Force creation, even if empty.

    std::vector<ControlPoint> controlPoints;
    std::vector<MappedControlPoint> mappedControlPoints;

    std::vector<TimeVaryingEntry> singleParameters;
    std::vector<TimeVaryingEntry> basisCoeffs;

    // FunctionString refers to the function as written in the equations,
    std::string functionString;
    std::string variable;
    MemorySynchronizerPtr memorySynchronizer;

    void extractSortedPoints(TDoubleVector &times, TDoubleVector &values,
                             RepeatPolicy repeatPolicy);

    double outOfRangeValue;
    double minTimePoint;
    double maxTimePoint;

    std::string getOutOfRangeValueOption() {
        return "Out of range value option.";
    }

   private:
    std::set<std::string> symbolsUsed;
    void setMinAndMaxTimepoints();
    bool disabled;
    bool useOutOfRangeFromControl;
    bool initialized;
    bool fromEquations;  // Quick hack here: will indicate whether the function
                         // was created from the equations in a model.
    // Set to true if one of the control points is a model parameter.
    // Will prevent from reinitializing too often.
    bool _containsVariableControls;
    TimeVaryingEntry outOfRangeEntry;
    MappedTimeVaryingEntry mappedOutOfRangeEntry;
};

#endif
