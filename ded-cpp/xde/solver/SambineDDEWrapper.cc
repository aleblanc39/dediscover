#include <algorithms/ConcreteParameters.h>
#include <solver/SambineDDEWrapper.h>
#include <solver/SandineDDEStruct.h>

#include <boost/make_shared.hpp>

#include <dde_solver.hpp>
#include <iostream>

using namespace std;

const std::string SambineDDEWrapper::relativeError = "R Tolerance";
const std::string SambineDDEWrapper::absoluteError = "A Tolerance";
const std::string SambineDDEWrapper::hinit = "Init Step Size";
const std::string SambineDDEWrapper::hmax = "Max Step Size";
const std::string SambineDDEWrapper::maxSteps = "Max Steps";

SambineDDEWrapper::SambineDDEWrapper() {
    addToControlParameters(std::make_shared<DoubleParameter>(
        relativeError, 1.0e-3, 0.0, 0.0, true, false,
        "relative error tolerance, default 0.001; it is required to be greater "
        "than zero. To increase accuracy, rtol can be reduced;"));

    addToControlParameters(std::make_shared<DoubleParameter>(
        absoluteError, 1.0e-6, 0.0, 0.0, true, false,
        "absolute error tolerance, default 1.0e-06; it is required to be "
        "greater than zero. To increase accuracy, atoll can be reduced;"));

    addToControlParameters(std::make_shared<DoubleParameter>(
        hinit, 0.001, 0.0, 0.0, true, false,
                            "initial step size, default 0.001; it is required "
                            "to be greater than zero;"));

    addToControlParameters(std::make_shared<DoubleParameter>(
        hmax, 1.0, 0.0, 0.0, true, false,
        "maximum step size, default 1.0; it is required to be greater than "
        "zero and less than the whole time range;"));

    addToControlParameters(std::make_shared<IntParameter>(
        maxSteps, (int)1.0e5, 1, 0, true, false,
        "maximum number of steps, default 10000; it is required to be greater "
        "than one;Computation will stop if maximum steps exceeded."));
}

void SambineDDEWrapper::performSimulation(
    ModelPtr model, const TDoubleVector& modelParameterValues,
    const TDoubleVector& initialConditionValues, TDoubleVector& tout,
    TDoubleMatrix& yout) {
    XDE_ASSERT(initialConditionValues.size() > 0);

    /** TODO: ModelParamterValues should have already been assigned. Should
     * remove parameter in all methods.
     * */

    using dde_solver::DdeIntegrator;
    using dde_solver::DdeSol;
    using dde_solver::DdeUserOption;

    FnDdes ddes(model);
    FnHistory his(model, initialConditionValues, tspan);
    FnEvent ef;
    FnDelay fnd(model);

    /*
     * user options. @c nullptr_t corresponding to optional
     * Fortran arguments that are not present. Here they are
     * @c CHANGE_FCN, @c OUT_FCN, and @c user_trim_get.
     */
    using Dde = DdeUserOption<FnDdes, FnDelay, FnHistory, FnEvent, nullptr_t,
                              nullptr_t, nullptr_t>;
    Dde o1(ddes, fnd, his, ef, nullptr, nullptr, nullptr);

    o1.nvar = {model->getDependentVariables().size(),
               model->nbDelayExpressions(), 1};

    o1.tspan = XDEUtil::getDoubleVector(tspan);
    o1.rerr = {getDoubleParameterValue(relativeError)};
    o1.aerr = {getDoubleParameterValue(absoluteError)};
    o1.opts.hinit =getDoubleParameterValue(hinit);
    o1.opts.hmax = getDoubleParameterValue(hmax);
    o1.opts.max_steps = getIntParameterValue(maxSteps);
    DdeSol sol = DdeIntegrator<Dde>(o1)();

    tout.resize(sol.t.size());
    std::copy(sol.t.begin(), sol.t.end(), tout.begin());

    yout.resize(sol.t.size(), model->getDependentVariables().size());

    unsigned ptr = 0;
    for (unsigned j = 0; j < yout.size2(); j++) {
        for (unsigned i = 0; i < yout.size1(); i++) {
            yout(i, j) = sol.y[ptr++];
            std::cerr << yout(i, j) << ", ";
        }
        std::cerr << endl;
    }
}