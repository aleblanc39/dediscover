#include "SolverResult.h"

#include <base/XDEException.h>
#include <base/XDEUtil.h>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

using namespace std;

SolverResult::SolverResult() { timeVariableName = "Time"; }


void SolverResult::setData(const TDoubleMatrix &newData) { data = newData; }

void SolverResult::setTimepoints(const TDoubleVector &newTimepoints) {
    timepoints = newTimepoints;
}

void SolverResult::setVariableNames(const vector<string> &newVarNames) {
    variableNames.assign(newVarNames.begin(), newVarNames.end());
}


