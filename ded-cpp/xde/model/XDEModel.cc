#include "XDEModel.h"

#include <base/XDEMessage.h>
#include <base/XDEUtil.h>
#include <mapping/ParsingResults.h>
#include <muParser.h>
#include <parser/ParserException.h>
#include <parser/XDEParser.h>
#include <statistics/MathUtil.h>

#include <algorithm>
#include <boost/cast.hpp>
#include <iterator>
#include <vector>

using namespace std;
using namespace std::placeholders;

XDEModel::XDEModel(const string &equations) {
    XDEMessage::log(XDEMessage::DEBUG4,
                    string("Model used:\n").append(equations));


    expressionHandler = std::make_shared<MuParserExpressionHandler>();
    parser = std::make_shared<XDEParser>(expressionHandler, equations);

    historyIntervalsPtr = parser -> getHistoryIntervals();
    delayedVariableHandler = parser -> getDelayedVariableHandler();

    if (equations.empty()) {
        errorMessages.push_back(
            ErrorMessageType(0, "The equations string is empty."));
        return;
    }

    if (parser->getErrorMessages().size() > 0) {
        ErrorMessageList erl = parser->getErrorMessages();
        copy(erl.begin(), erl.end(), back_inserter(errorMessages));
        return;
    }
    memorySynchronizer = expressionHandler->getMemorySynchronizer();
    init();
}

XDEModel::~XDEModel() {
    XDEMessage::log(XDEMessage::DEBUG2, "Into XDEModel destructor.");
}

void XDEModel::init() {
    try {
        XDEMessage::log(XDEMessage::DEBUG3, "Init called from xdemodel\n");

        modelParametersBlockID =
            memorySynchronizer->createBlock(getAllNames(getModelParameters()));
    
        dependentVariablesBlockID = memorySynchronizer->createBlock(
            getAllNames(getDependentVariables()));
        createEquationsObjects();
        
        initializeConstantTVFunctions();
        collectAllVariables();

    } catch (ParserException &e) {
        XDEMessage::log(XDEMessage::DEBUG,
                        "An error was uncovered while parsing the equations.");
        errorMessages.push_back(ErrorMessageType(e.getLineNO(), e.getMsg()));
    } catch (XDEException &e) {
        XDEMessage::log(XDEMessage::DEBUG,
                        string("Parsing exception: ").append(e.getMsg()));
        errorMessages.push_back(ErrorMessageType(0, e.getMsg()));
    } catch (...) {
        errorMessages.push_back(
            ErrorMessageType(0,
                             "Unrecognized error encounter while initializing "
                             "the model. Please contact the developers."));
    }
}

void XDEModel::collectAllVariables() {
    allVariables = getAllNames(getDependentVariables());
    auto modelParams = getAllNames(getModelParameters());
    allVariables.insert(allVariables.end(), modelParams.begin(),
                        modelParams.end());
    auto macroNames = getMacroNames();
    allVariables.insert(allVariables.end(), macroNames.begin(),
                        macroNames.end());
    allVariables.push_back(getTimeVariableName());
}

void XDEModel::setTimeVariableValue(double d) {
    validateEquations();
    memorySynchronizer->updateMemory(getTimeVariableName(), d);
}

void XDEModel::setDependentVariableValues(const TDoubleVector &values) {
    validateEquations();
    memorySynchronizer->setValues(dependentVariablesBlockID,
                                  &(values.data()[0]));
}

void XDEModel::setModelParameterValues(const TDoubleVector &values) {
    validateEquations();
    if (values.size() == 0) return;
    memorySynchronizer->setValues(modelParametersBlockID, &(values.data()[0]));
}

/**
 * Compute values for the covariates. Assumes they have been initialized.
 * * TODO: Strange  that computeMacros will take a time, not computeCovariates
 * 
 */
void XDEModel::computeCovariates() {
    for (CovariatePtr p : getCovariates()) {
        double d = p->getTVFunction()->compute(expressionHandler->compute(
            p->getExpressionObject()->getEquationID()));
        memorySynchronizer->updateMemory(p->getName(), d);
    }
}

/**
 * TODO: Dependencies in macros? Do we need to set the order for the macros?
 * 
 * @param timepoint 
 */
void XDEModel::computeMacros(const double &timepoint) {
    setTimeVariableValue(timepoint);
    validateEquations();
    computeCovariates();

    double macroValue;
    for (MacroPtr p : getMacros()) {
        macroValue = expressionHandler->compute(
            p->getExpressionObject()->getEquationID());
        memorySynchronizer->updateMemory(p->getName(), macroValue);
    }
}

/**
 * Compute one iteration.
 *
 * Will compute one iteration. The caller (normally the solver) will
 * provide the current values for the variables and the model
 * parameters.
 *
 * There is room for optimization here: the model provides the model
 * parameter values to the solver. If we know that they will not
 * change then we can ignore them.
 *
 * This method will also handle the thread listening.
 */

void XDEModel::compute(double t, const TDoubleVector &y,
                       const TDoubleVector &modelParas, TDoubleVector &yout) {
    // cerr <<"XDEModel::compute t = " << t << endl;
    // cerr <<"y is: " << y << endl;
    validateEquations();
    handleThread();
    setTimeVariableValue(t);
    xde_resize(yout, y.size());

    memorySynchronizer->setValues(dependentVariablesBlockID, &(y.data()[0]));
    if (modelParas.size() > 0)
        memorySynchronizer->setValues(modelParametersBlockID,
                                      &(modelParas.data()[0]));

    timeDependentEquations->compute();
    memorySynchronizer->getValues(derivativeResultBlockID, &(yout.data()[0]));
    // cerr << "yout is: " << yout << " at " << t << endl;
}

void XDEModel::compute(double t, TDoubleVector &yout) {
    handleThread();
    TDoubleVector modelParams(getModelParameters().size());
    TDoubleVector initialConditions(yout.size());
    memorySynchronizer->getValues(dependentVariablesBlockID,
                                  &(initialConditions.data()[0]));

    if (modelParams.size() > 0)
        memorySynchronizer->getValues(modelParametersBlockID,
                                      &(modelParams.data()[0]));

    setTimeVariableValue(t);
    timeDependentEquations->compute();
    memorySynchronizer->getValues(derivativeResultBlockID, &(yout.data()[0]));
}


// BUG HERE: Calling multiple times with same parameters result in second
// computation being different from first one, but all subsequent being fine.
// May be in MuParser code itself...
void XDEModel::compute(double t, const TDoubleVector &y, TDoubleVector &yout) {
    validateEquations();
    handleThread();
    setTimeVariableValue(t);
    xde_resize(yout, y.size());

    memorySynchronizer->setValues(dependentVariablesBlockID, &(y.data()[0]));
    timeDependentEquations->compute();
    memorySynchronizer->getValues(derivativeResultBlockID, &(yout.data()[0]));
}

void XDEModel::compute(realtype t, N_Vector y, N_Vector ydot) {
    double tt = (double)t;
    // TODO Make more efficient. Allocate ret only once, reuse each call.

    unsigned n = getDependentVariables().size();
    TDoubleVector yy(n);
    TDoubleVector yydot(n);

    for (unsigned i = 0; i < n; i++) yy[i] = NV_Ith_S(y, i);
    compute(tt, yy, yydot);
    for (unsigned i = 0; i < n; i++) NV_Ith_S(ydot, i) = yydot[i];
}

/**
 * Will set the values for both muparser and the precompiled
 * models. Setting values for precompiled models may not be used, but
 * the muparser always will, since it can be used to compute the
 * macros for plotting.
 */
void XDEModel::setFixedInitialConditionValues(
    const TDoubleVector &newInitConds) {
    XDE_ASSERT(newInitConds.size() == (unsigned)getDependentVariables().size());
    // const vector<DependentVariablePtr> &initConds = dependentVariables;
    unsigned cntr = 0;
    for (DependentVariablePtr s : getDependentVariables()) {
        memorySynchronizer->updateMemory(
            expressionHandler->getInitialConditionString(s->getName()),
            newInitConds[cntr++]);
    }
}

vector<string> XDEModel::getMacroNames() { return getAllNames(getMacros()); }
vector<string> XDEModel::getCovariateNames() {
    return getAllNames(getCovariates());
}

/**
 * There are two types of TV functions: inserted in the equations, or mapped
 * through a covariate. If mapped through a covariate then we can save time by
 * not initializing the unmapped ones.
 */

void XDEModel::initializeCovariates() {
    for (CovariatePtr p : getCovariates()) {
        if (p->isMapped() /**&& p->getTVFunctionMapping()->isDirty()**/) {
            p->getTVFunction()->initialize();
        }
    }
}

void XDEModel::initializeTVFunctions() {
    // Just initialize the non-constant ones. The constant ones are initialized
    // through another call.
    // cerr << "Initializing NON-constant TV Functions.\n";
    // TODO AL2021: Spell out here difference between constan and non-constant
    // TVF

    XDEMessage::log(XDEMessage::DEBUG3,
                    "Initializing the non-constant TV functions");
    for (const auto &u : expressionHandler->getParsedTVFunctions()) {
        TimeVaryingFunctionPtr tvf =
            TimeVaryingFunctionList::getInstance()->getFunction(u);
        if (tvf->containsVariableControls()) {
            tvf->initialize();
        }
    }
}

/**
 * Same function as above, but handles the functions with no variable control
 * points.
 */

void XDEModel::initializeConstantTVFunctions() {
    XDEMessage::log(XDEMessage::DEBUG3,
                    "Initializing the constant TV functions");

    for (const auto &u : expressionHandler->getParsedTVFunctions()) {
        TimeVaryingFunctionPtr tvf =
            TimeVaryingFunctionList::getInstance()->getFunction(u);
        if (!tvf->containsVariableControls()) {
            cerr << "Will initialize TVF\n";
            tvf->initialize();
        }
    }
}


// TODO AL2021 Still necessary? Looks like it. Called from XDESolverWrapper
void XDEModel::processTimepoints(const TDoubleVector &timepoints) {
    memorySynchronizer->updateMemory("t0", timepoints[0]);
    memorySynchronizer->updateMemory("tend", timepoints[timepoints.size() - 1]);
}

/**
 * The input vector will contain the values for the fixed initial
 * conditions. Upon return it will also contain the values for those
 * that are computed. Will assign values to the macros in the process.
 *
 * Will assume here that the model parameter values have already been assigned.
 */

void XDEModel::initializeInitialConditionValues(
    TDoubleVector &workingInitialConditionValues) {
    // First set the fixed values
    setFixedInitialConditionValues(workingInitialConditionValues);
    constantEquations->compute();

    unsigned cntr = 0;
    for (auto p : getDependentVariables()) {
        if (p->getInitialConditionExpression().get() != NULL) {
            workingInitialConditionValues[cntr] = memorySynchronizer->getValue(
                ExpressionHandler::getInitialConditionString(p->getName()));
        }
        cntr++;
    }
}


void XDEModel::compute(double t, const TDoubleVector &y,
                       const TDoubleVector &modelParams,
                       const TDoubleMatrix &delayedVariablesValues,
                       TDoubleVector &yout) {
                           
    validateEquations();
    setDelayedVariableValues(delayedVariablesValues);
    compute(t, y, modelParams, yout);
}

void XDEModel::compute(double t, const TDoubleVector &y,
             const TDoubleMatrix &delayedVariables, TDoubleVector &yout) {
    validateEquations();
    setDelayedVariableValues(delayedVariables);
    compute(t, y, yout);
}

/**
 * Set the delayed variable values.
 *
 * Set the delayed variable values based on their position in the
 * matrix.
 * 
 * Called from model, after history values have been written.
 */

void XDEModel::setDelayedVariableValues(const TDoubleMatrix &values) {
    if (!delayedVariableHandler->hasDelays()) return;

    const auto d = delayedVariableHandler -> arrangeDelayVariableValues(values);
    memorySynchronizer->setValues(delayedVariablesBlockID,
                                  &(d.data()[0]));
}


/**
 * Compute the macros at one specific timepoint given the dependent
 * variable values at a series of timepoints. Will perform
 * interpolation on the dependent variable values if required. The
 * resulting values are stored in the Macro objects, and the vector of
 * macro objects is returned as well.
 *
 * timeIndex is the index in the timepts array where we want to
 * compute the macros.
 * 
 * Only invoked (indirectly) be the JNI, most likely for display.
 */

void XDEModel::computeMacrosAtTime(int timeIndex, const vector<double> &timepts,
                                   const TDoubleMatrix &depVarValues) {
    XDE_ASSERT(depVarValues.size2() ==
               (unsigned)getDependentVariables().size());
    XDE_ASSERT(timepts.size() == depVarValues.size1());
    validateEquations();

    int nbDelays = delayedVariableHandler -> nbDelayExpressions();
    int nbDepVars = getDependentVariables().size();
    // setDependentVariableValues(depVarValues.getRow(timeIndex));
    setDependentVariableValues(XDEUtil::getRow(depVarValues, timeIndex));

    // First need to set the values of the delayed variables, if there
    // are any.
    if (nbDelays > 0) {
        TDoubleVector interpolatedValues(nbDepVars);
        vector<double> delays = getUniqueTimes(timepts[timeIndex]);
        TDoubleMatrix delayValues(nbDepVars, nbDelays);
        for (int i = 0; i < nbDelays; i++) {
            if (delays[i] > timepts[0])
                MathUtil::straightLineInterpolation(timepts, delays[i],depVarValues,
                                     interpolatedValues);
            else {
                // AL 2021: Not commented in original code
                // Probably never used, since this is before start of simulation time.
                // for (int varNO = 0; varNO < nbDepVars; varNO++) {
                //     interpolatedValues[varNO] = getHistory(varNO, delays[i]);
                // }
            }

            // This one is commented, though...
            // delayValues.replaceCol(i, interpolatedValues);
            for (int j = 0; j < nbDepVars; j++)
                delayValues(j, i) = interpolatedValues[j];
        }
        setDelayedVariableValues(delayValues);
    }
    // AL2021 Not commented in original code
    // setTimeVariableValue(timepts[timeIndex]);
    computeMacros(timepts[timeIndex]);
}

/**
 * Compute the values of the macros at the timepoints specified in the
 * timepts vector. The input matrix has one row for each timepoint and
 * one column for each dependent variable, in the order specified in
 * the MSV dependentVariables.
 *
 * The return matrix contains one row for each timepoint, and one
 * column for each macro in the order specified by the Macro vector.
 * 
 * Currently only invoked by the JNI code for display.
 */

TDoubleMatrix XDEModel::computeMacrosAtTimepoints(
    const vector<double> &timepts, const TDoubleMatrix &depVarValues) {
    XDE_ASSERT(depVarValues.size2() ==
               (unsigned)getDependentVariables().size());
    XDE_ASSERT(timepts.size() == depVarValues.size1());
    validateEquations();
    unsigned nbTimepoints = timepts.size();
    unsigned nbMacros = getMacros().size();

    TDoubleMatrix ret(nbTimepoints, nbMacros);
    for (unsigned i = 0; i < nbTimepoints; i++) {
        computeMacrosAtTime(i, timepts, depVarValues);
        for (unsigned j = 0; j < nbMacros; j++)
            ret(i, j) = memorySynchronizer->getValue(getMacros()[j]->getName());
    }
    return ret;
}

TDoubleMatrix XDEModel::computeCovariatesAtTimepoints(
    const vector<double> &timepts) {
    unsigned nbTimepoints = timepts.size();
    unsigned nbCovs = getCovariates().size();
    TDoubleMatrix ret(nbTimepoints, nbCovs);
    for (unsigned i = 0; i < nbTimepoints; i++) {
        setTimeVariableValue(timepts[i]);
        computeCovariates();
        unsigned j = 0;
        for (CovariatePtr p : getCovariates()) {
            ret(i, j++) = memorySynchronizer->getValue(p->getName());
        }
    }
    return ret;
}

/**
 * Method to create the equation objects that will be used during the
 * computation.
 *
 * Must end with the statement
 *  expressionHandler -> remapMemory()
 *
 * Two objects are created:
 *  constantEquations
 *  timeDependentEquations
 *
 * AL2021:
 *
 */

void XDEModel::createEquationsObjects() {
    vector<string> delayVarNames;
    
    // TODO Remove from here. Put in own method that reurns the id
    if (delayedVariableHandler -> hasDelays()) {
        for (auto dv: expressionHandler->getDelayedVariables()) {
            string var = dv->getInternalName();
            delayVarNames.push_back(var);
        }
        delayedVariablesBlockID =
            memorySynchronizer->createBlock(delayVarNames);
    } else {
        delayedVariablesBlockID = -1;
    }


    constantEquations = std::make_shared<ExpressionSetEvaluator>(memorySynchronizer);
    timeDependentEquations = std::make_shared<ExpressionSetEvaluator> (memorySynchronizer);

    /**
     * Will fill up 2 equations objects. First look at macros and
     * initial conditions in the order provided by the parser. Initial
     * conditions containing an expression and macros used by initial
     * conditions are added to the constantEquations object.
     */
    for (const string &s : parser->getOrderOfComputation()) {
        bool added = false;
        std::vector<MacroPtr> macros = getMacros();
        std::vector<MacroPtr>::iterator it =
            find_if(macros.begin(), macros.end(),
                    [=](MacroPtr p) { return p->getName() == s; });
        if (it != macros.end()) {
            MacroPtr m = *it;
            string eq =
                s + "=" + m->getExpressionObject()->getTransformedExpression();
            if (m->isUsedInInitialCondition()) {
                constantEquations->addExpression(eq);
            } else {
                timeDependentEquations->addExpression(eq);
            }
            added = true;
        }

        if (!added) {
            std::vector<CovariatePtr> covariates = getCovariates();
            std::vector<CovariatePtr>::iterator cit =
                find_if(covariates.begin(), covariates.end(),
                        [=](CovariatePtr p) { return p->getName() == s; });
            if (cit != covariates.end()) {
                CovariatePtr m = *cit;
                string eq = s + "=" + m->getDisplayExpression();
                timeDependentEquations->addExpression(eq);
                added = true;
            }
        }

        if (!added) {
            // Dealing with initial condition. Only add if it
            // contains an expression.
            std::vector<DependentVariablePtr> dependentVariables =
                getDependentVariables();
            vector<DependentVariablePtr>::iterator it = find_if(
                dependentVariables.begin(), dependentVariables.end(),
                [=](DependentVariablePtr p) { return p->getName() == s; });
            assert(it != dependentVariables.end());
            DependentVariablePtr p = *it;
            if (p->getInitialConditionExpression() != NULL) {
                string eq = expressionHandler->getInitialConditionString(s) + "=" +
                            p->getInitialConditionExpression()
                                ->getTransformedExpression();
                constantEquations->addExpression(eq);
            }
        }
    }

    // Need to create temporary variables for the derivatives.
    // Add this method to the memory synchronizer.

    map<string, string> depVarMap;
    for (DependentVariablePtr d : getDependentVariables()) {
        string tmpvar = memorySynchronizer->generateVariable();
        string expr(tmpvar);
        expr.append("=").append(
            d->getDerivativeExpression()->getTransformedExpression());
        timeDependentEquations->addExpression(expr);
        depVarMap[d->getName()] = tmpvar;
    }
    // Create a block containing the result of the derivatives, in the
    // order of the variables in the dependentVariable vector
    vector<string> varNames = getAllNames(getDependentVariables());
    vector<string> derivativeResultNames;
    derivativeResultNames.reserve(varNames.size());
    for (const auto &s : varNames) {
        derivativeResultNames.push_back(depVarMap[s]);
    }

    // TODO Move the block creation to the ExpressionHandler.
    // Then the expression handler will remap the memory for all the expressions
    // it contains. And so there will be no need for an explicit remap from
    // here.

    derivativeResultBlockID =
        memorySynchronizer->createBlock(derivativeResultNames);
    dependentVariablesBlockID = memorySynchronizer->createBlock(varNames);

    // By calling createBlock we have changed the location of some of the
    // variables in the memory bank, and so the muParser objects need to call
    // again the
    //  myobj.DefiveVar(v, memorySynchronizer.getLocation(v))
    // for each variable v.
    //
    // When there is time, callbacks and listeners should be implemented.
    expressionHandler->remapMemory();
}

ErrorMessageList XDEModel::validateFormula(std::string expression) const {
    ErrorMessageList ret;
    try {
        MuParserExpressionEvaluator mup(expression, memorySynchronizer);
        mup.evaluate();
    } catch (XDEException &e) {
        ret.push_back(ErrorMessageType(0, e.getMsg()));
        return ret;
    }

    ParsingResults results(expression);
    auto fNames = results.getFunctionNames();
    for (auto it = fNames.begin(); it != fNames.end(); it++) {
        if (!MuParserExpressionHandler::isFunctionDefined(*it)) {
            ret.push_back(ErrorMessageType(
                0, string("Function is not defined: ").append(*it)));
        }
    }

    auto symbols = results.getSymbolNames();
    for (auto it = symbols.begin(); it != symbols.end(); it++) {
        if (find(allVariables.begin(), allVariables.end(), *it) ==
            allVariables.end())
            ret.push_back(
                ErrorMessageType(0, string("Error in mapping: variable ")
                                        .append(*it)
                                        .append(" is not defined.")));
    }

    // There is no reason to have something of the form X[] in the mapping.
    if (results.getIndexedSymbolNames().size() > 0) {
        ret.push_back(ErrorMessageType(
            0, "There should not be indexed variables in the mapping."));
    }

    return ret;
}



const std::vector<double> &XDEModel::getHistoryAtTime(double t) {
    std::function<double(int)> histFunction = std::bind(&MuParserExpressionHandler::compute2, _1, expressionHandler);
    memorySynchronizer -> updateMemory("t", t);
    return historyIntervalsPtr -> getHistory(t, histFunction);
}


/**
 * Compute the time at which a variable must be computed.
 */

std::vector<double>  XDEModel::getUniqueTimes(double t) {
    validateEquations();
    computeMacros(t);
    std::function<double(int)> histFunction = std::bind(&MuParserExpressionHandler::compute2, _1, expressionHandler);

    return delayedVariableHandler -> getUniqueTimes(histFunction);

}
