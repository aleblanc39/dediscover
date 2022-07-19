#ifndef SambineDDEWrapper_H_
#define SambineDDEWrapper_H_

//#include <solver/AdaptiveStepSizeSolver.h>
#include <solver/DDEWrapper.h>

/**
 * \brief Wrapper aound the delayed-differential equations methods
 * implemented by Sambine and Thompson..
 *
 */

class SambineDDEWrapper : public DDEWrapper {
   public:
    const static std::string relativeError;// = "R Tolerance";
    const static std::string absoluteError;// = "R Tolerance";
    const static std::string hinit;// = "Init Step Size";
    const static std::string hmax;// = "Max Step Size";
    const static std::string maxSteps;// = "Max Steps";


    static std::vector<GeneralParameterPtr> getControlParameters();

    SambineDDEWrapper(){}
    virtual ~SambineDDEWrapper() {}

    // std::string getShortMethodDescription() {
    //     return s_getShortMethodDescription();
    // }
    static std::string s_getShortMethodDescription() {
        return "Delay differential equation solver from Sambine & Thompson";
    }

    static std::string s_getLongMethodDescription() {
        return "Implements DDE solver developed by Sambine and Thompson.";
    }
    // std::string getLongMethodDescription() {
    //     return s_getLongMethodDescription();
    // }

    static std::string s_getMethodName() { return "Sambine DDE Solver"; }
    //std::string getMethodName() { return s_getMethodName(); }

    static auto s_getMethodAttributes() {
        auto attrs = XDESolverWrapper::getAttributes();
        attrs.push_back("DDE");
        return attrs;
    }

   protected:
    void performSimulation(ModelPtr model,
                           const TDoubleVector& modelParameterValues,
                           const TDoubleVector& initialConditionValues,
                           TDoubleVector& tout, TDoubleMatrix& yout);
};

#endif /*SambineDDEWrapper_H_*/