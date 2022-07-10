#ifndef EXPRESSIONHANDLER_H
#define EXPRESSIONHANDLER_H

#include <expressions/DelayedVariable.h>
#include <expressions/MemorySynchronizer.h>
#include <parser/ParseTreeComponents.h>
#include <parser/ProcessedModelExpression.h>


#include <map>
#include <string>
#include <vector>

class ExpressionHandler;
typedef std::shared_ptr<ExpressionHandler> ExpressionHandlerPtr;

/**
 * \brief Pure virtual base class to handle the expressions.
 *
 * The objective
 * will be to hide all the details of expression handling from the
 * other class, making it easier to substitute different approaches to
 * evaluating the expressions from the model, including using the
 * muParser, generating and compiling C code, or generating R code.
 *
 * The interface will define that the equations are ordered by the
 * order in which they are entered.
 *
 *
 */

class ExpressionHandler {
   public:
    ExpressionHandler(){}
    virtual ~ExpressionHandler(){}

    virtual int addExpression(std::string expression) = 0;
    virtual double compute(unsigned equationNO) const = 0;
    virtual std::string getExpression(unsigned equationNO) = 0;

    virtual void setVariableValue(std::string symbol, double value) = 0;
    virtual double getVariableValue(std::string symbol) = 0;

    virtual ProcessedModelExpressionPtr generateCode(
        const ParseTreeComponents::ExpressionPtr expression,
        int lineNO = -1) = 0;

    virtual std::vector<DelayedVariablePtr> getDelayedVariables() = 0;
    virtual std::map<std::string, unsigned> getDelayedVariableExpressions() = 0;

    std::vector<std::string> getExpressions();
    virtual MemorySynchronizerPtr getMemorySynchronizer() {
        return memorySynchronizer;
    }

    static auto getInitialConditionString(const std::string &x) {
        return std::string("____INIT__") + x;
    }

    void addDependentVariable(const std::string &v) {
        dependentVariables.insert(v);
    }

    bool isDependentVariable(const std::string &v) const {
        return (dependentVariables.find(v) != dependentVariables.end());
    }

    /** Remap how the variables map to memory, for instance after parsing new
     * equations. 
     * TODO: Check if really necessary.
     * */
    virtual void remapMemory() = 0;

   protected:
    // TODO Have this object inherit a pure virtual that handles the dep. vars,
    // acts as an interface.
    std::set<std::string> dependentVariables;
    MemorySynchronizerPtr memorySynchronizer;
};
#endif
