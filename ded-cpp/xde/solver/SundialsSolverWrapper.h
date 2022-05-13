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

    // Methods from XDEAlgorithm
    std::string getShortMethodDescription() {
        return s_getShortMethodDescription();
    }
    static std::string s_getShortMethodDescription() {
        return "Solvers provided by Sundials";
    }

    std::string getLongMethodDescription() {
        return s_getLongMethodDescription();
    }
    static std::string s_getLongMethodDescription() {
        return std::string("Sundials solvers.")
            .append("Can be used for both stiff and non-stiff problems..");
    }

    std::string getMethodName() { return s_getMethodName(); }
    static std::string s_getMethodName() { return "SundialsSolver Solver"; }

    static int runSundial(realtype t, N_Vector y, N_Vector ydot,
                          void *user_data);

   protected:
    virtual void performSimulation(ModelPtr model,
                                   const TDoubleVector &modelParameterValues,
                                   const TDoubleVector &initialConditionValues,
                                   TDoubleVector &tout, TDoubleMatrix &yout);

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
       return { {precNoneOpt, (int)PREC_NONE}, 
                {precLeftOpt, (int)PREC_LEFT},
                {precRightOpt, (int)PREC_RIGHT},
                {precBothOpt, (int)PREC_BOTH}};
    }

    void initializeLinearSolver(void *cvode_mem, unsigned nbVariables);

};

#endif  // SUNDIALS_SOLVER_WRAPPER_H
