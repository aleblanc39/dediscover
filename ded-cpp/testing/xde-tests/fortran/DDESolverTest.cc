#include <model/XDEModel.h>
#include <base/XDEUtil.h>
#include <unistd.h>

#include <dde_solver.hpp>
#include <string>

#include "doctest.h"

using namespace std;

static void printVec(const std::vector<double> &v) {
    for (auto i: v)
        std::cerr << i << ", ";
    std::cerr << std::endl;
}

SCENARIO("Testing one iteration of a DDE Solver") {
    static XDEModel model(
        "dx/dt = 2*x +x[t-1]\n\
        dy/dt = 3*y + x[t-alpha]\n\
        dz/dt = 3*y[t-2] + x[t-2] + 4\n\
        x[t] = -3, -100<t<0\n\
        y[t] = 22, -100<t<0");

    GIVEN("A Simple Model") {
        TDoubleVector modelParamValues(1);
        modelParamValues(0) = 4.0;
        model.setModelParameterValues(modelParamValues);

        using dde_solver::DdeIntegrator;
        using dde_solver::DdeSol;
        using dde_solver::DdeUserOption;


        /*
         * Functor for RHS of DDEs
         */

        static std::vector<double> ret;
        static TDoubleVector yp;
        struct FnDdes {
            // y contains current values, z contains delays.

            
            std::vector<double> operator()(const double& t,
                                           const std::vector<double>& y,
                                           const std::vector<double>& z) const {
                cerr << "Computing fndes at time " << t << endl;
                cerr << "y: "; printVec(y);
                cerr << "z: "; printVec(z);

                auto nvar = y.size();
                auto nDelays = z.size() / y.size();
                ret.resize(nvar);
                yp.resize(nvar);
                TDoubleMatrix mat(nvar, nDelays);
                unsigned ptr = 0;
                for (int j = 0; j < nDelays; j++) {
                    for (int i = 0; i < nvar; i++) {
                        mat(i, j) = z[ptr++];
                    }
                }

                model.compute(t, XDEUtil::getTVectorDouble(y), mat, yp);
                for (int i = 0; i < nvar; i++) ret[i] = yp(i);
                return ret;
            }
        };

        /*
         * Functor for history
         */
        struct FnHistory {

            std::vector<double> operator()(const double& t) const {
                cerr << "Computing history at " << t << endl;

                if (t == 0) {
                    cerr << "t is 0\n";
                    vector<double> ret = {1,2,3};
                    cerr << "Length of return vector: " << ret.size() << endl;
                    return ret;
                } else {
                    vector<double> ret = model.getHistoryAtTime(t);
                    cerr << "t is not zero. Returned size: " << ret.size() << endl;
                    return ret;
                }
            }
        };

        /*
         * Functor for events
         */
        struct FnEvent {
            std::vector<double> operator()(const double& t,
                                           const std::vector<double>& y,
                                           const std::vector<double>& dydt,
                                           const std::vector<double>& z) const {
                cerr << "Computing event  at " << t << endl;

                return {t};
            }
        };

        // Locate all the delays used at least once
        struct FnDelay {
            std::vector<double> operator()(const double& t, 
                                           const int& nbDelays,
                                           const std::vector<double>& y) const {
                cerr << "Into FnDelay with nbDelays, t, size of y :" << nbDelays << "," << t << ", " << y.size() << endl;
                std::cerr << "Values of y:  \n";
                for (auto d: y) {cerr << d << ", ";}
                cerr << endl;

                vector<double> ret = model.getUniqueTimes(t);
                cerr << "REturning unique times of size " << ret.size() << endl;
                return ret;
            }
        };

        FnDdes ddes;
        FnHistory his;
        FnEvent ef;
        FnDelay fnd;

        // std::vector<double> delay{0.74, 0.84};  // a single delay

        /*
         * user options. @c nullptr_t corresponding to optional
         * Fortran arguments that are not present. Here they are
         * @c CHANGE_FCN, @c OUT_FCN, and @c user_trim_get.
         */
        using Dde = DdeUserOption<FnDdes, FnDelay, FnHistory, FnEvent,
                                  nullptr_t, nullptr_t, nullptr_t>;
        cerr << "Creating o1\n";
        cerr << "Nb Delay Expressions: " << model.nbDelayExpressions() << endl;
        Dde o1(ddes, fnd, his, ef, nullptr, nullptr, nullptr);
        o1.nvar = {(int)(model.getDependentVariables().size()), (int) (model.nbDelayExpressions()), 1};
        for (auto i: o1.nvar) {
            cerr << i << endl;
        }

        vector<double> tspan;
        double d = 0;
        while (d <= 5.0) {
            tspan.push_back(d);
            d+= 0.1;
        }

        o1.tspan = tspan;

        // solution is in @c DdeSol.
        cerr << "Calling DDEIntegrator\n";
        DdeSol sol = DdeIntegrator<Dde>(o1)();
        cerr << sol.t.size() << endl;
        for (auto i: sol.y) {
            cerr << i << ", ";
        }


        THEN("These should be some answer") { CHECK(sol.t.size() > 0); }
    }
}
