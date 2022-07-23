#ifndef SUNDIALS_SOLVER_WRAPPER_H
#define SUNDIALS_SOLVER_WRAPPER_H

#include <cvode/cvode.h>            /* prototypes for CVODE fcts., consts. */
#include <nvector/nvector_serial.h> /* serial N_Vector types, fcts., macros */
#include <sundials/sundials_iterative.h>

#include "ODEWrapper.h"

/**
 * \brief Wrapper around the Sundials solvers.
 *
 */

class SundialsSolverWrapper : public ODEWrapper {
   public:
    // Options for linear solver
    const static std::string cvdenseOpt;
    const static std::string cvdiagOpt;
    const static std::string cvpgmrOpt;
    const static std::string cvpbcgOpt;
    const static std::string cvptfgmrOpt;

    const static std::string stiffOpt;
    const static std::string nonStiffOpt;

    const static std::string initStepSizeParameterName;
    const static std::string minStepSizeParameterName;
    const static std::string maxStepSizeParameterName;

    const static std::string rtolParamName;
    const static std::string atolParamName;

    const static std::string problemTypeParameterName;

    const static std::string maxErrTestFailsParameterName;
    const static std::string maxNonlinItersParameterName;
    const static std::string maxNumStepsParameterName;
    const static std::string maxConvFailsParameterName;
    const static std::string nonlinConvCoefParameterName;

    const static std::string linearSolverParameterName;
    // Option for preconditioning
    const static std::string precNoneOpt;
    const static std::string precLeftOpt;
    const static std::string precRightOpt;
    const static std::string precBothOpt;

    const static std::string
        preconditioningParameterName;  //        return "Preconditioning";
    const static std::string
        maxKrylovDimensionParameter;  //    return "Max dimension of Krylov
                                      //    Space";

    SundialsSolverWrapper();
    virtual ~SundialsSolverWrapper();

    static std::string s_getDescription() {
        return "Class to implement the SundialsSolver wrapper.";
    }

    static std::string s_getShortMethodDescription() {
        return "Solvers provided by Sundials";
    }

   static std::string s_getLongMethodDescription() {
        return std::string("Sundials solvers.")
            .append("Can be used for both stiff and non-stiff problems..");
    }

    static std::string s_getMethodName() { return "SundialsSolver Solver"; }

    static int runSundial(realtype t, N_Vector y, N_Vector ydot,
                          void *user_data);

    static std::vector<GeneralParameterPtr> getControlParameters() {
        auto v = getProblemTypes();

        return {std::make_shared<StringParameter>(
                    problemTypeParameterName, v[0], v, "", "", false, false,
                    "Type of problem to simulate."),

                std::make_shared<IntParameter>(
                    maxNumStepsParameterName, 500, 1, 0, false, false,
                    "The maximum number of steps to be taken \
by the solver in its attempt to reach the next output time. Entering 0 will use the default of 500, a negative value disables the test."),

                std::make_shared<IntParameter>(
                    maxErrTestFailsParameterName, 7, 1, 0, true, false,
                    "Maximum number of error failures allowed in one "
                    "step. Must be greater then 0."),

                std::make_shared<IntParameter>(
                    maxNonlinItersParameterName, 3, 1, 0, true, false,
                    "Maximum number of non-linear solver iterations "
                    "allowed per step. Must be greater then 0."),

                std::make_shared<IntParameter>(
                    maxConvFailsParameterName, 10, 1, 0, true, false,
                    "Maximum number of allowable non-linear solver convergence "
                    "failures "
                    "per step. Must be greater then 0."),

                std::make_shared<DoubleParameter>(
                    nonlinConvCoefParameterName, 0.1, 0.0001, 0, true, false,
                    "Safety factor. Must be greater than 0."),

                std::make_shared<DoubleParameter>(
                    initStepSizeParameterName, 0.001, 0.0, 0.0, true, false,
                    "initial step size, default 0.001; it is required to be "
                    "greater than "
                    "zero;"),

                std::make_shared<DoubleParameter>(
                    minStepSizeParameterName, 1.0e-15, 0.0, 0.0, true, false,
                    "minimum step size, default 1.0e-15; it is required to be "
                    "greater than "
                    "zero and less than the maximum step size;"),

                std::make_shared<DoubleParameter>(
                    maxStepSizeParameterName, 1.0, 0.0, 0.0, true, false,
                    "maximum step size, default 1.0; it is required to be "
                    "greater than "
                    "zero and less than the whole time range;"),

                std::make_shared<DoubleParameter>(
                    rtolParamName, 1.0e-3, 0.0, 0.0, true, false,
                    "Relative error tolerance, default 0.001; it is required "
                    "to be greater "
                    "than zero. To increase accuracy, rtol can be reduced;"),

                std::make_shared<DoubleParameter>(
                    atolParamName, 1.0e-6, 0.0, 0.0, true, false,
                    "Absolute error tolerance, default 1.0e-06; it is required "
                    "to be "
                    "greater than zero. To increase accuracy, atol can be "
                    "reduced;")};
    }

   protected:
    virtual void performSimulation(ModelPtr model,
                                   const TDoubleVector &modelParameterValues,
                                   const TDoubleVector &initialConditionValues,
                                   TDoubleVector &tout, TDoubleMatrix &yout,
                                   const ParameterValueMap &pvm);

   private:
    // Options for stiff/non-stiff

    static std::vector<std::string> getProblemTypes() {
        return {stiffOpt, nonStiffOpt};
    }

    static std::vector<std::string> getLinearSolverNames() {
        return {cvdenseOpt, cvdiagOpt};
        /* (cvpgmrOpt()) */
        /* (cvpbcgOpt()) */
        /* (cvptfgmrOpt()) */
    }

    static std::vector<std::string> getPreconditioningOptions() {
        return {precNoneOpt, precLeftOpt, precRightOpt, precBothOpt};
    }

    static std::map<std::string, int> getPreconditioningMap() {
        return {{precNoneOpt, (int)PREC_NONE},
                {precLeftOpt, (int)PREC_LEFT},
                {precRightOpt, (int)PREC_RIGHT},
                {precBothOpt, (int)PREC_BOTH}};
    }

    void initializeLinearSolver(void *cvode_mem, unsigned nbVariables);
};

#endif  // SUNDIALS_SOLVER_WRAPPER_H
