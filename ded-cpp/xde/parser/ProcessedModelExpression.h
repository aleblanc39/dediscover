#ifndef PROCESSEDMODELEXPRESSION_H
#define PROCESSEDMODELEXPRESSION_H

#include <string>
#include <vector>
#include <set>
#include <memory>



/**
 * \brief Class that stores information on an expression, 
 * such as the list of symbols used, whether it is time varying,
 *
 * The class also holds an association with the id of the expression
 * created from this expression, its original format, the transformed
 * format (used in MuParser).
 */
class ProcessedModelExpression {

 public:

    typedef std::set<std::string> SymbolsSetType;


    ProcessedModelExpression(std::string expr);

    void addSymbolUsed(std::string s){symbolsUsed.insert(s);}
    void addFunctionUsed(std::string s){functionsUsed.insert(s);}
    void addDelayedVariableUsed(std::string s){delayedVariablesUsed.insert(s);}

    std::string getOriginalExpression(){return originalExpression;}
    std::string getTransformedExpression(){return transformedExpression;}

    bool isTimeVarying(){return timeVarying;}
    void setTimeVarying(bool b) {timeVarying = b;}

    void setDisplayExpression(std::string e) {
	displayExpression = e;
    }

    std::string getDisplayExpression() {
	if (displayExpression.length() > 0)
	    return displayExpression;
	return originalExpression;
    }

    void setTransformedExpression(const std::string s){transformedExpression = s;}
    SymbolsSetType getSymbolsUsed(){return symbolsUsed;}
    SymbolsSetType getFunctionsUsed(){return functionsUsed;}
    SymbolsSetType getDelayedVariablesUsed(){return delayedVariablesUsed;}

    void setEquationID(unsigned u){equationID = u;}
    unsigned getEquationID(){return equationID;}

 private:

    unsigned equationID;
    std::string originalExpression;
    std::string transformedExpression;

    std::set<std::string> symbolsUsed;
    std::set<std::string> functionsUsed;
    std::set<std::string> delayedVariablesUsed;

    std::string displayExpression;

    bool timeVarying;

};
typedef std::shared_ptr<ProcessedModelExpression> ProcessedModelExpressionPtr;

#endif  // PROCESSEDMODELEXPRESSION_H
