/**
 * Base class for the XDE parser.
 */

#ifndef XDEPARSER_H_
#define XDEPARSER_H_

#include <base/XDEMessage.h>
#include <base/XDEUtil.h>
#include <expressions/ExpressionHandler.h>

#include <algorithm>

#include <map>
#include <set>
#include <string>
#include <vector>

#include <parser/HistoryIntervals.h>
#include <parser/DelayedVariablesHandler.h>
#include "Covariate.h"
#include "DependentVariable.h"
#include "HistoryInterval.h"
#include "Macro.h"
#include "ModelParameter.h"
#include "ParseTree.h"
#include "ProcessedModelExpression.h"

namespace ptc = ParseTreeComponents;

typedef boost::variant<DependentVariablePtr, MacroPtr, CovariatePtr>
    ModelSymbolVariant;

typedef std::pair<HistoryInterval, int> HistoryElement;

/**
 * \brief visitor for the ModelSymbolVariant.
 */

class UsedSymbolsVisitor
    : public boost::static_visitor<std::set<std::string> > {
   public:
    std::set<std::string> operator()(DependentVariablePtr p) const {
        if (p->getInitialConditionExpression() != NULL) {
            return p->getInitialConditionExpression()->getSymbolsUsed();
        }
        return std::set<std::string>();
    }

    std::set<std::string> operator()(MacroPtr p) const {
        if (p->getExpressionObject() != NULL) {
            return p->getExpressionObject()->getSymbolsUsed();
        }
        return std::set<std::string>();
    }

    std::set<std::string> operator()(CovariatePtr p) const {
        if (p->getExpressionObject() != NULL) {
            return p->getExpressionObject()->getSymbolsUsed();
        }
        return std::set<std::string>();
    }
};

/**
 * \brief Class to parse the model
 *
 * Will use the SpiritParser class to parse the model, then
 * turn the result into useful information. All the information
 * that the XDEModel class will receive about the model comes
 * from an object from this class.
 *
 */

class XDEParser  {
   public:
    virtual ~XDEParser();

    XDEParser(ExpressionHandlerPtr e, std::string eqns) : expressionHandler(e) {
        parseModel(eqns);
    }


    const auto &getDependentVariables() const { return dependentVariables;}
    const auto &getModelParameters() const {return modelParameters;}
    const auto &getMacros() const { return macros; }
    const auto &getCovariates() const {return covariates;}

    const auto &getOrderOfComputation() const {
        return orderOfComputation;
    }

    auto getHistoryIntervals() const {return historyIntervals;}
    const auto &getDelayedVariableHandler(){return delayedVariableHandler;}


    std::vector<DelayedVariablePtr> getDelayedVariables() {
        return expressionHandler->getDelayedVariables();
    }
    
    const ErrorMessageList &getErrorMessages() { return errorMessages; }

   protected:
    
    std::map<std::string, int> delayedVariableExpressions;

    void validateDelayedVariables();
    HistoryIntervalsPtr<std::string, int>  extractHistory();
    DelayedVariableHandlerPtr<unsigned> createDelayedVariableObj();

    // std::vector<ModelParameterPtr> modelParametersVector;
    // std::vector<DependentVariablePtr> dependentVariablesVector;
    // std::vector<MacroPtr> macrosVector;
    // std::vector<CovariatePtr> covariatesVector;

    std::vector<std::string> orderOfComputation;

    std::map<std::string, DependentVariablePtr> dependentVariables;
    std::map<std::string, MacroPtr> macros;
    std::map<std::string, ModelParameterPtr> modelParameters;
    std::map<std::string, CovariatePtr> covariates;

    // std::map<std::string, int> allSymbols;

    bool isDependentVariable(const std::string &varName);
    bool isMacro(const std::string &varName);
    bool isModelParameter(const std::string &varName);
    bool isCovariate(const std::string &varName);

    bool validDependentVariable(const std::string &varName);
    bool validMacroName(const std::string &varName);

    ExpressionHandlerPtr expressionHandler;

    static std::string getMacroTVFlagName(std::string macroName) {
        return std::string("___").append(macroName);
    }

   private:
    ParseTreePtr parseTree;
    void parseModel(const std::string &equations);

    ErrorMessageList errorMessages;

    static std::set<std::string> unavailableWords;
    void extractSymbols();
    void processStatements();
    void processMacroCycles();
    ProcessedModelExpressionPtr createProcessedModelExpression(
        const ptc::StatementPtr &st);
    void addModelParameters(ProcessedModelExpressionPtr exp, unsigned lineNO);
    bool isTimeDependent(unsigned i, const std::vector<ModelSymbolVariant> &v);
    HistoryIntervalsPtr<std::string, int> historyIntervals;
    DelayedVariableHandlerPtr<unsigned> delayedVariableHandler;



};

#endif /*XDEPARSER_H_*/
