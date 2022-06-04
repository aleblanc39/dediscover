#ifndef DELAYED_VARIABLES_HANDLER_H
#define DELAYED_VARIABLES_HANDLER_H

#include <base/XDEException.h>
#include <base/XDEBlas.h>

#include <vector>

using dvector = std::vector<double>;

template <typename T>
std::vector<std::pair<unsigned, unsigned>> getDelayVarLocations(
    const std::vector<std::string> &varNames,
    const std::vector<std::pair<std::string, T>> delayVars,
    const std::vector<T> eqnIds) {
    std::vector<std::pair<unsigned, unsigned>> locations;

    for (auto p : delayVars) {
        auto it = std::find(varNames.begin(), varNames.end(), p.first);

        if (it == varNames.end()) {
            throw XDEException("Problem build delayed variables.");
        }
        unsigned loc = it - varNames.begin();
        unsigned loc_second =
            std::find(eqnIds.begin(), eqnIds.end(), p.second) - eqnIds.begin();

        locations.push_back(std::pair<unsigned, unsigned>(loc, loc_second));
    }
    return locations;
}

template <typename T>
class DelayedVariableHandler {
   public:
    // Pairs in delayExpressions are <varName, eqnID in ExpressionHandler>
    DelayedVariableHandler(
        // TODO VALIDATE if order of var name is important.
        const std::vector<std::string> &varNames,
        const std::vector<std::pair<std::string, T>> delayVars,
        const std::vector<T> eqnIds);

    const dvector &getUniqueTimes(std::function <double(T)> func) {
        unsigned cntr = 0;
        for (const auto &id: equationIDs) {
            returnedUniqueTimes[cntr++] = func(id);
        }
        return returnedUniqueTimes;
    }

    const std::vector<double> &arrangeDelayVariableValues(const TDoubleMatrix &values);
    bool hasDelays() const {return equationIDs.size() > 0;}
    unsigned nbDelayExpressions() const {return returnedUniqueTimes.size();}

   private:
    std::vector<T> equationIDs;
    std::vector<std::pair<unsigned, unsigned>> delayedVariableLocations;

    std::vector<double> returnedDelayVals;
    std::vector<double> returnedUniqueTimes;

};

template <typename T>
DelayedVariableHandler<T>::DelayedVariableHandler(

    const std::vector<std::string> &varNames,
    const std::vector<std::pair<std::string, T>> delayVars,
    const std::vector<T> eqnIds) {

    returnedDelayVals.resize(delayVars.size());
    returnedUniqueTimes.resize(eqnIds.size());

    equationIDs.resize(eqnIds.size());
    std::copy(eqnIds.begin(), eqnIds.end(), equationIDs.begin());

    delayedVariableLocations =
        getDelayVarLocations(varNames, delayVars, eqnIds);

}

/**
 * Take the delay variables from a matrix of size #vars * delayEquations and put them
 * in a vector of the same order as the delayedVariables passed to constructor.
 * Often most of the entries in the matrix will be unused.
 * */

template <typename T>
const std::vector<double> &DelayedVariableHandler<T>::arrangeDelayVariableValues(
    const TDoubleMatrix &values) {
    for (unsigned i = 0; i < returnedDelayVals.size(); i++) {
        returnedDelayVals[i] = values(delayedVariableLocations[i].first,
                                      delayedVariableLocations[i].second);
    }

    return returnedDelayVals;
}


template <typename T> using  DelayedVariableHandlerPtr = std::shared_ptr<DelayedVariableHandler<T>> ;
#endif  // DELAYED_VARIABLES_HANDLER_H