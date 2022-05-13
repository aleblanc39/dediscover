#include "IntermediateSymbol.h"


IntermediateSymbol::IntermediateSymbol(std::string symbolName) : ModelSymbol(symbolName){}
IntermediateSymbol::IntermediateSymbol(std::string symbolName, unsigned lineNO, int occurenceIndex):
    ModelSymbol(symbolName, lineNO, occurenceIndex){}



void IntermediateSymbol::assignExpression(ProcessedModelExpressionPtr e) {
    expression = e;
}

