#include "LOCF.h"

using namespace std;

LOCF::LOCF(std::string fs, MemorySynchronizerPtr mem)
    : TimeVaryingFunction(fs, mem) {
    XDEMessage::log(XDEMessage::DEBUG3, string("Initializing LOCF with ").append(fs));
}

LOCF::LOCF() { cerr << "Creating empy locf\n"; }

LOCF::LOCF(std::string functionString) : TimeVaryingFunction(functionString) {}

LOCF::~LOCF() {}

/**
 *
 * Compute the LOCF. Will (for now) assume that the time points are
 * stored in increasing order. Will set to 0 if smaller than smallest
 * defined point. Also to 0 if there are no control points. Performs a
 * binary search to find the right location.
 */
double LOCF::computeFunction(const double &t) {
    XDEMessage::log(XDEMessage::DEBUG3, string("Computing LOCF with value").append(to_string(t)));

    // muparser may call this function before everything is initialized.
    double ret;
    // If there are no control point or the time is less that the smallest
    // control point return 0;
    unsigned nbPoints = sortedTimeValues.size();
    if (nbPoints == 0 || t < sortedTimeValues[0].first) {
        ret = 0;
    } else if (t >= sortedTimeValues.back().first) {
        ret = sortedTimeValues.back().second;
    } else {
        unsigned lb = 0;
        unsigned ub = nbPoints;
        unsigned current = nbPoints / 2;
        bool done = false;
        while (!done) {
            if (sortedTimeValues[current].first <= t &&
                t < sortedTimeValues[current + 1].first) {
                done = true;
                ret = sortedTimeValues[current].second;
            } else {
                if (t > sortedTimeValues[current].first) {
                    lb = current + 1;
                } else {
                    ub = current - 1;
                }
                current = lb + (ub - lb) / 2;
            }
        }
    }
    return ret;
}

/**
 * We first need to sort the control points
 */
void LOCF::initializeSpecific() {
    XDEMessage::log(XDEMessage::DEBUG3, "Initializing LOCF");
    TDoubleVector times;
    TDoubleVector values;
    extractSortedPoints(times, values, AVERAGE_REPEATS);
    unsigned size = times.size();
    sortedTimeValues.resize(size);
    for (unsigned i = 0; i < size; i++) {
        sortedTimeValues[i] = std::pair<double, double>(times[i], values[i]);
    }
}
