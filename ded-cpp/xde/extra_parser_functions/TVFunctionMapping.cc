#include "TVFunctionMapping.h"
#include <base/XDEUtil.h>

using namespace std;

TVFunctionMapping::TVFunctionMapping(std::string symbol)
    : mappedSymbol(symbol) {
    dirty = true;
}

TVFunctionMapping::~TVFunctionMapping() {}

/**
 * Initialize the time-varying function. For now only add the control
 * points.
 */
void TVFunctionMapping::assignControlPoints(
    TimeVaryingFunctionPtr tvFunctionPtr,
    const Vec &points) {
    // There will always be a mapping defined. If there is no function
    // then there is nothing to do.
    if (tvFunctionPtr.get() == NULL) return;
    tvFunctionPtr->reset();

    for (const auto &u : points) {
        tvFunctionPtr->addControlPoint(u.first, u.second);
    }
}
