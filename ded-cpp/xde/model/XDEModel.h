#ifndef XDEMODEL_H
#define XDEMODEL_H

#include <base/ThreadInterface.h>
#include <base/XDEBase.h>
#include <cvode/cvode.h> /* prototypes for CVODE fcts., consts. */
#include <base/XDEBlas.h>
#include <expressions/ExpressionSetEvaluator.h>
#include <expressions/MemorySynchronizer.h>
#include <expressions/MuParserExpressionHandler.h>
#include <extra_parser_functions/TVFunctionMapping.h>
#include <nvector/nvector_serial.h> /* serial N_Vector types, fcts., macros */

#include <parser/XDEParser.h>

#include <parser/HistoryIntervals.h>
#include <parser/DelayedVariablesHandler.h>

#include <iostream>
#include <map>
#include <string>

class XDEModel;
typedef std::shared_ptr<XDEModel> ModelPtr;

/**
 * \brief Implements the models used for DEDiscover.
 *
 * It will contain a parser object to parse the equations, and functions to
 * perform computation on a valid model.
 */

class XDEModel : public ThreadInterface, public XDEBase {
    
   public:
    XDEModel(const std::string &equations);

    static const std::string parserLineNO_flag;
    static const std::string parserOccurence_flag;
    static const std::string parserLineDef_flag;

    virtual ~XDEModel();

    // TODO: create these vectors locally so we can get rid of references to the parser object here.
    const auto &getDependentVariables() const {
        return parser->getDependentVariables();
    }
    const auto &getModelParameters() const {
        return parser->getModelParameters();
    }
    const auto &getMacros() const {
        return parser->getMacros();
    }
    const auto &getCovariates() const {
        return parser->getCovariates();
    }


    const std::vector<double> &getHistoryAtTime(double t);

    const ErrorMessageList &getErrorMessages() const { return errorMessages; }
    ErrorMessageList validateFormula(std::string formula) const;

    std::vector<std::string> getMacroNames();
    std::vector<std::string> getCovariateNames();
    void setTimeVariableValue(double d);

    // TODO AL 2021 In the next 3 the caller needs to have the variables in the
    // right order. Should check that the increased performance is worth the 
    // possible problems.
    void setDependentVariableValues(const TDoubleVector &values);
    void setModelParameterValues(const TDoubleVector &values);
    void setFixedInitialConditionValues(const TDoubleVector &newInitConds);

    void compute(double t, const TDoubleVector &dependentVariableValues,
                 const TDoubleVector &modelParams, TDoubleVector &yout);

    void compute(double t, const TDoubleVector &dependentVariableValues,
                 TDoubleVector &yout);

    // Put a call used for sundials solvers
    void compute(realtype t, N_Vector y, N_Vector ydot);

    void compute(double t, TDoubleVector &yout);

    void compute(double t, const TDoubleVector &y,
                 const TDoubleVector &modelParams,
                 const TDoubleMatrix &delayVariables, TDoubleVector &yout);

    void compute(double t, const TDoubleVector &y,
                 const TDoubleMatrix &delayVariables, TDoubleVector &yout);

    std::string getTimeVariableName() { return "t"; }
    auto getMemorySynchronizer() {
        return memorySynchronizer;
    }

    void computeMacros(const double &timepoint);

    TDoubleMatrix computeCovariatesAtTimepoints(const std::vector<double> &timepts);
    TDoubleMatrix computeMacrosAtTimepoints(const std::vector<double> &timepts,
                                            const TDoubleMatrix &depVarValues);

    // Must be called before any computation is performed on the TV functions,
    // includes both the functions assigned through mapping and those hard-coded
    // in the equations.
    void initializeTVFunctions();  // {parser -> initializeTVFunctions();}
    void initializeConstantTVFunctions();
    void initializeCovariates();

    void processTimepoints(const TDoubleVector &timepoints);

    void initializeInitialConditionValues(
        TDoubleVector &workingInitialConditionValues);
    void computeCovariates();

    std::vector<double> getUniqueTimes(double t);
    auto nbDelayExpressions(){return delayedVariableHandler -> nbDelayExpressions();}

    void setDelayedVariableValues(const TDoubleMatrix &delayedVariables);

    bool containsDelays() const { if (delayedVariableHandler != NULL) return delayedVariableHandler->hasDelays(); else return false; }

    unsigned getNbVariables() const { return getDependentVariables().size(); }
    const auto &getAllVariables() const { return allVariables; }

   protected:
    XDEParserPtr parser;
    MemorySynchronizerPtr memorySynchronizer;
    ExpressionSetEvaluatorPtr constantEquations;
    ExpressionSetEvaluatorPtr timeDependentEquations;

    int derivativeResultBlockID;
    int dependentVariablesBlockID;
    int modelParametersBlockID;
    
    void validateEquations() {
        if (errorMessages.size() > 0)
            throw XDEException(
                "Trying to perform an operation on a model with no equations "
                "or with errors in the equations.");
    }

    void init();

   private:
    ErrorMessageList errorMessages;
    MuParserExpressionHandlerPtr expressionHandler;

    HistoryIntervalsPtr<std::string, int> historyIntervalsPtr;
    DelayedVariableHandlerPtr<unsigned> delayedVariableHandler;

    void createEquationsObjects();
    int delayedVariablesBlockID;

    void computeMacrosAtTime(int timeIndex, const std::vector<double> &timepts,
                             const TDoubleMatrix &depVarValues);

    void collectAllVariables();

    std::vector<std::string> allVariables;
   
    // std::vector<std::pair<std::string, unsigned> > delayExpressions;
    // std::vector<std::vector<HistoryElement>> history;

};

#endif  // XDEMODEL_H
