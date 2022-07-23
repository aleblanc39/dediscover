#ifndef LEASTSQUARE_H_
#define LEASTSQUARE_H_

#include "objectfunction/XDEObjectiveFunction.h"
#include <algorithms/ConcreteParameters.h>


/**
 * \brief Class to compute least squares for use by the objective function.
 *
 * Despite the class name this class can also use the absolute value instead
 * of the square root when computing the function. In addition it can use the
 * log of the values, and/or normalize the values across the column of data.
 */

class LeastSquare : public XDEObjectiveFunction {
   public:
    LeastSquare(){}
    LeastSquare(DataGeneratorPtr d) : XDEObjectiveFunction(d) {}

    static std::vector<GeneralParameterPtr> getControlParameters() {
        std::vector<std::string> v = getValidFunctions();
        return {
            std::make_shared<StringParameter>(
                getAppliedFunctionParameterName(), v[1], v, "", "", false,
                false, "Function applied to the difference of the values"),

            std::make_shared<BoolParameter>(
                getNormalizedParameterName(), false, false, false, false, false,
                "Indicates whether the data will be normalized."),

            std::make_shared<BoolParameter>(
                getLogValuesParameterName(), false, false, false, false, false,
                "Indicates whether the log of the values will be used.")};
    }

    ~LeastSquare(){}

    // Methods from XDEAlgorithm

    // std::string getShortMethodDescription() {
    //     return s_getShortMethodDescription();
    // }
    static std::string s_getShortMethodDescription() {
        return "Least Squares Fitting";
    }

    // std::string getLongMethodDescription() {
    //     return s_getLongMethodDescription();
    // }
    static std::string s_getLongMethodDescription() {
        return std::string(
            "Least squares fitting directly to the observed data.  Implied "
            "statistical model:  All observed data are independent, have "
            "additive measurement error, and alll measurement errors share a "
            "common distribution that is symmetric about 0.  For example, for "
            "observed values x[i] i=1..n, observed_x[i] = true_x[i] + "
            "error_x[i], where the error_x[i] are iid Normal(0, sigma^2) with "
            "a common sigma^2 across all variables.  ");
    }

    // std::string getMethodName() { return s_getMethodName(); }
    static std::string s_getMethodName() { return "Least Squares"; }

    static std::string getLogValuesParameterName() { return "Log values"; }
    static std::string getAppliedFunctionParameterName() {
        return "Applied Function";
    }

    void preProcess();

    static std::vector<std::string> s_getMethodAttributes() {
        return {};
    }

    

   protected:
   private:
    static std::vector<std::string> getValidFunctions() {
        return {"Absolute value", "Square"};
    }
};

#endif /*LEASTSQUARE_H_*/
