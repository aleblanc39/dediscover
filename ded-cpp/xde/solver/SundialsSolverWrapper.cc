#include "SundialsSolverWrapper.h"

//#include <cvode/cvode_dense.h>
#include <cvode/cvode_diag.h>
//#include <cvode/cvode_spbcgs.h>
// #include <cvode/cvode_spgmr.h>
// #include <cvode/cvode_sptfqmr.h>

#include <boost/make_shared.hpp>

using namespace std;

static void check_flag(void *flagvalue, int opt);

const std::string SundialsSolverWrapper::rtolParamName = "R Tolerance";
const std::string SundialsSolverWrapper::atolParamName = "A Tolerance";

const std::string SundialsSolverWrapper::cvdenseOpt = "CVDense";
const std::string SundialsSolverWrapper::cvdiagOpt = "CVDiag";
const std::string SundialsSolverWrapper::cvpgmrOpt = "CVpgmr";
const std::string SundialsSolverWrapper::cvpbcgOpt = "CVpbcg";
const std::string SundialsSolverWrapper::cvptfgmrOpt = "CVptfgmr";

const std::string SundialsSolverWrapper::stiffOpt = "Stiff";
const std::string SundialsSolverWrapper::nonStiffOpt = "Non-Stiff";

const std::string SundialsSolverWrapper::initStepSizeParameterName =
    "Init Step Size";
const std::string SundialsSolverWrapper::minStepSizeParameterName =
    "Min Step Size";
const std::string SundialsSolverWrapper::maxStepSizeParameterName =
    "Max Step Size";

const std::string SundialsSolverWrapper::problemTypeParameterName =
    "Problem type";
const std::string SundialsSolverWrapper::maxErrTestFailsParameterName =
    "Max Number of Error Test Failures";
const std::string SundialsSolverWrapper::maxNonlinItersParameterName =
    "Max Number of  Linear Iterations";
const std::string SundialsSolverWrapper::maxNumStepsParameterName =
    "Max Number of Steps";
const std::string SundialsSolverWrapper::maxConvFailsParameterName =
    "Max Number of Convergence Failures";
const std::string SundialsSolverWrapper::nonlinConvCoefParameterName =
    "Safety Factor";

const std::string SundialsSolverWrapper::linearSolverParameterName =
    "Linear Solver";
// Option for preconditioning
const std::string SundialsSolverWrapper::precNoneOpt = "None";
const std::string SundialsSolverWrapper::precLeftOpt = "Left";
const std::string SundialsSolverWrapper::precRightOpt = "Right";
const std::string SundialsSolverWrapper::precBothOpt = "Both";

const std::string SundialsSolverWrapper::preconditioningParameterName =
    "Preconditioning";
const std::string SundialsSolverWrapper::maxKrylovDimensionParameter =
    "Max dimension of Krylov Space";

SundialsSolverWrapper::SundialsSolverWrapper() {
    // controlParameters.clear();
    auto v = getProblemTypes();

    addToControlParameters(std::make_shared<StringParameter>(
        problemTypeParameterName, v[0], v, "", "", false, false,
        "Type of problem to simulate."));

    // auto linearTypes = getLinearSolverNames();
    // addToControlParameters(std::make_shared<StringParameter>(
    //     linearSolverParameterName, linearTypes[0], linearTypes, "", "", false,
    //     false, "Linear solver to use."));

    // vector<string> preconditioningOptions = getPreconditioningOptions();
    // controlParameters.push_back(GeneralParameterPtr
    // 				(new
    // StringParameter(getPreconditioningParameterName(),
    // 						     preconditioningOptions[0],
    // 						     preconditioningOptions,
    // 						     "",
    // 						     "",
    // 						     false,
    // 						     false,
    // 						     "Preconditioning. Only
    // applies to
    // "
    // 						     + cvpgmrOpt()
    // 						     + ", " + cvpbcgOpt()
    // 						     + " or " + cvptfgmrOpt()
    // 						     + " linear solvers"
    // 						     )));

    // controlParameters.push_back(GeneralParameterPtr
    // 				(new IntParameter
    // 				 (getMaxKrylovDimensionParameterName(),
    // 				  5,
    // 				  1,
    // 				  10,
    // 				  true,
    // 				  true,
    // 				  "Maximum dimension of Krylov Space. Applies
    // only to
    // "
    // 				  + cvpgmrOpt()
    // 				  + ", " + cvpbcgOpt()
    // 				  + " or " + cvptfgmrOpt()
    // 				  + " linear solvers"
    // 				  )));

    addToControlParameters(std::make_shared<IntParameter>(
        maxNumStepsParameterName, 500, 1, 0, false, false,
        "The maximum number of steps to be taken \
by the solver in its attempt to reach the next output time. Entering 0 will use the default of 500, a negative value disables the test."));

    addToControlParameters(std::make_shared<IntParameter>(
        maxErrTestFailsParameterName, 7, 1, 0, true, false,
        "Maximum number of error failures allowed in one "
        "step. Must be greater then 0."));

    addToControlParameters(std::make_shared<IntParameter>(
        maxNonlinItersParameterName, 3, 1, 0, true, false,
        "Maximum number of non-linear solver iterations "
        "allowed per step. Must be greater then 0."));

    addToControlParameters(std::make_shared<IntParameter>(
        maxConvFailsParameterName, 10, 1, 0, true, false,
        "Maximum number of allowable non-linear solver convergence failures "
        "per step. Must be greater then 0."));

    addToControlParameters(std::make_shared<DoubleParameter>(
        nonlinConvCoefParameterName, 0.1, 0.0001, 0, true, false,
        "Safety factor. Must be greater than 0."));

    addToControlParameters(std::make_shared<DoubleParameter>(
        initStepSizeParameterName, 0.001, 0.0, 0.0, true, false,
        "initial step size, default 0.001; it is required to be greater than "
        "zero;"));

    addToControlParameters(std::make_shared<DoubleParameter>(
        minStepSizeParameterName, 1.0e-15, 0.0, 0.0, true, false,
        "minimum step size, default 1.0e-15; it is required to be greater than "
        "zero and less than the maximum step size;"));

    addToControlParameters(std::make_shared<DoubleParameter>(
        maxStepSizeParameterName, 1.0, 0.0, 0.0, true, false,
        "maximum step size, default 1.0; it is required to be greater than "
        "zero and less than the whole time range;"));

    addToControlParameters(std::make_shared<DoubleParameter>(
        rtolParamName, 1.0e-3, 0.0, 0.0, true, false,
        "Relative error tolerance, default 0.001; it is required to be greater "
        "than zero. To increase accuracy, rtol can be reduced;"));

    addToControlParameters(std::make_shared<DoubleParameter>(
        atolParamName, 1.0e-6, 0.0, 0.0, true, false,
        "Absolute error tolerance, default 1.0e-06; it is required to be "
        "greater than zero. To increase accuracy, atol can be reduced;"));
}

SundialsSolverWrapper::~SundialsSolverWrapper() {}

/**
 * The error handling function for cvode.
 */

static void ErrorHandlingFunction(int error_code, const char *module,
                                  const char *function, char *msg,
                                  void *eh_data) {
    XDEMessage::log(XDEMessage::WARNING, "Error from Sundials: " + string(msg));
}

int SundialsSolverWrapper::runSundial(realtype t, N_Vector y, N_Vector ydot,
                                      void *user_data) {
    XDEModel *model = (XDEModel *)user_data;
    model->compute(t, y, ydot);
    return 0;
}

/**
 *
 */
void SundialsSolverWrapper::performSimulation(
    ModelPtr model, const TDoubleVector &modelParameterValues,
    const TDoubleVector &initialConditionValues, TDoubleVector &tout,
    TDoubleMatrix &yout) {
    int flag;
    unsigned nbVariables = initialConditionValues.size();

    tout.resize(tspan.size());
    yout.resize(tspan.size(), nbVariables);
    setMatrixValue(yout, XDEUtil::XDE_NAN);

    N_Vector nvInitialConditionValues = N_VNew_Serial(nbVariables);
    for (unsigned i = 0; i < nbVariables; i++) {
        NV_Ith_S(nvInitialConditionValues, i) = initialConditionValues[i];
    }

     model->setModelParameterValues(modelParameterValues);


    // std::cerr << "perform sunduials simulation\n";
    // for (auto d: modelParameterValues) std::cerr << d << ", "; std::cerr << std::endl;
    // for (auto d: initialConditionValues) std::cerr << d << ", "; std::cerr << std::endl;
    // for (auto d: tspan) std::cerr << d << ", "; std::cerr << std::endl;


    string problemType = getStringParameterValue(problemTypeParameterName);
    double atol = getDoubleParameterValue(atolParamName);
    double rtol = getDoubleParameterValue(rtolParamName);
    int maxNumSteps = getIntParameterValue(maxNumStepsParameterName);
    int maxErrTestFails = getIntParameterValue(maxErrTestFailsParameterName);
    int maxNonlinIters = getIntParameterValue(maxNonlinItersParameterName);
    int maxConvFails = getIntParameterValue(maxConvFailsParameterName);
    double nlscoef = getDoubleParameterValue(nonlinConvCoefParameterName);
    double hinit = getDoubleParameterValue(initStepSizeParameterName);
    double hmin = getDoubleParameterValue(minStepSizeParameterName);
    double hmax = getDoubleParameterValue(maxStepSizeParameterName);

    // TODO Some test here. Safety Factor  != 0

    void *cvode_mem = NULL;

    if (problemType == stiffOpt) {
        cvode_mem = CVodeCreate(CV_BDF);
    } else {
        cvode_mem = CVodeCreate(CV_ADAMS);
    }

    check_flag((void *)cvode_mem, 0);

    // Set the error handling function
    flag = CVodeSetErrHandlerFn(cvode_mem, &ErrorHandlingFunction, NULL);
    check_flag(&flag, 12);

    // Initialize the memory
    flag = CVodeInit(cvode_mem, SundialsSolverWrapper::runSundial, startTime,
                     nvInitialConditionValues);
    check_flag(&flag, 1);

    // Tolerance specification

    flag = CVodeSStolerances(cvode_mem, atol, rtol);
    check_flag((void *)cvode_mem, 4);

    // Set max steps, [initial, min, max] step size

    flag = CVodeSetMaxNumSteps(cvode_mem, maxNumSteps);
    check_flag(&flag, 40);

    flag = CVodeSetInitStep(cvode_mem, hinit);
    check_flag(&flag, 41);

    flag = CVodeSetMinStep(cvode_mem, hmin);
    check_flag(&flag, 42);

    flag = CVodeSetMaxStep(cvode_mem, hmax);
    check_flag(&flag, 43);

    flag = CVodeSetMaxErrTestFails(cvode_mem, maxErrTestFails);
    check_flag(&flag, 43);

    flag = CVodeSetMaxNonlinIters(cvode_mem, maxNonlinIters);
    check_flag(&flag, 43);

    flag = CVodeSetMaxConvFails(cvode_mem, maxConvFails);
    check_flag(&flag, 43);

    flag = CVodeSetNonlinConvCoef(cvode_mem, nlscoef);
    check_flag(&flag, 43);

    // Lots of options for linear solver. Do in separate method
    // for readibility
    initializeLinearSolver(cvode_mem, nbVariables);

    // The user data is whatever we want to send.
    // In our case is a pointer to the model object that will
    // perform the computation.
    flag = CVodeSetUserData(cvode_mem, (void *)(model.get()));
    check_flag(&flag, 6);

    tout[0] = tspan[0];
    double t;

    // TODO Probably some easier vector copy here.
    for (unsigned i = 0; i < nbVariables; i++) {
        yout(0, i) = NV_Ith_S(nvInitialConditionValues, i);
    }

    unsigned j = 1;
    unsigned nbTspan = tspan.size();
    bool errorDetected = false;
    tout = tspan;
    while (j < nbTspan && !errorDetected) {
        double d = tspan[j];
        flag = CVode(cvode_mem, d, nvInitialConditionValues, &t, CV_NORMAL);
        // Some checks

        if (flag == CV_SUCCESS) {
            XDE_ASSERT(double_equals(t, d));
            for (unsigned i = 0; i < nbVariables; i++) {
                yout(j, i) = NV_Ith_S(nvInitialConditionValues, i);
            }
            //std::cerr << "cvode computed ok\n";
            //tout[j] = d;
            j++;
        } else {
            //std::cerr << "An error has been detected " << flag << std::endl;
            errorDetected = true;
        }
    }

    // Free memory
    N_VDestroy_Serial(nvInitialConditionValues);
    CVodeFree(&cvode_mem);
    //      std::cerr << "Sundials: done with perform simulation\n";
}

void SundialsSolverWrapper::initializeLinearSolver(void *cvode_mem,
                                                   unsigned nbVariables) {
    // int krylov = ((IntParameter*)
    // getCtrlParameter(getMaxKrylovDimensionParameterName()).get())->getValue();
    //string linearSolver = getStringParameterValue(linearSolverParameterName);
    // string preconditioning = ((StringParameter *)
    // 			      getCtrlParameter(getPreconditioningParameterName()).get())->getValue();

    // The new version of Sundials has only one "built-in" linear solver. From
    // the retlease note:
    /**
     Removed package-specific, linear solver-specific, solver modules (e.g.
     CVDENSE, KINBAND, IDAKLU, ARKSPGMR) since their functionality is entirely
     replicated by the generic Dls/Spils interfaces and
     SUNLINEARSOLVER/SUNMATRIX modules. The exception is CVDIAG, a diagonal
     approximate Jacobian solver available to CVODE and CVODES.
     */
    int flag;
    flag = CVDiag(cvode_mem);

    // if (linearSolver == cvdenseOpt()) {
    //     flag = CVDense(cvode_mem, nbVariables);
    //     check_flag(&flag, 5);
    // } else if (linearSolver == cvdiagOpt()) {
    //     flag = CVDiag(cvode_mem);
    //     check_flag(&flag, 6);
    // } else if (linearSolver == cvpgmrOpt()) {
    //     // 	flag = CVSpgmr(cvode_mem,
    //     // getPreconditioningMap()[preconditioning], krylov);
    //     check_flag(&flag,
    //     // 7); } else if (linearSolver == cvpbcgOpt()) { 	flag =
    //     // CVSpbcg(cvode_mem, getPreconditioningMap()[preconditioning],
    //     krylov);
    //     // 	check_flag(&flag, 8);
    //     // } else if (linearSolver == cvptfgmrOpt()) {
    //     // 	flag = CVSptfqmr(cvode_mem,
    //     // getPreconditioningMap()[preconditioning], krylov);
    //     check_flag(&flag,
    //     // 9);
    //     throw XDEException("Unknown linear solver: " + linearSolver);
    // }
}

/**
 * Method copied from cvode examples.
 */

static void check_flag(void *flagvalue, int opt) {
    int *errflag;

    /* Check if SUNDIALS function returned NULL pointer - no memory allocated */
    if (opt == 0 && flagvalue == NULL) {
        throw XDEException("SUNDIALS_ERROR: Can not allocate memory");
    } else if (opt == 1) {
        errflag = (int *)flagvalue;
        if (*errflag < 0) {
            throw XDEException("SUNDIALS_ERROR: Failed");
        }
    }

    else if (opt == 2 && flagvalue == NULL) {
        throw XDEException("SUNDIALS function returned null pointer");
    }

    else if (opt == 4 && flagvalue == NULL) {
        throw XDEException("SUNDIALS: problems with atol/rtol specifications");
    }

    else if (opt == 5 && flagvalue == NULL) {
        throw XDEException("SUNDIALS: problem with linear solver.");
    }

    else if (opt == 6 && flagvalue == NULL) {
        throw XDEException("SUNDIALS: problem with user data.");
    }
}
