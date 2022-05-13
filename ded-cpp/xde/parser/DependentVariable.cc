
#include "DependentVariable.h"

using namespace std;

DependentVariable::DependentVariable(string name) : ModelSymbol(name) {
}

DependentVariable::DependentVariable(string name, unsigned lineNO, int occurenceIndex) : 
    ModelSymbol(name, lineNO, occurenceIndex) {
}



DependentVariable::~DependentVariable() {
}


void DependentVariable::assignDerivative(ProcessedModelExpressionPtr s) {
    derivativeExpression = s;
}

void DependentVariable::assignInitialCondition(ProcessedModelExpressionPtr s) {
    initialConditionExpression = s;
}


