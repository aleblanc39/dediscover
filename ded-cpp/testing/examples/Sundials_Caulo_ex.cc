/**
 * Testing the operation of caulobacter, tracking a bug in the computation.
 * Simulating twice give two different results...
 * */

#include <model/XDEModel.h>
#include <solver/SundialsSolverWrapper.h>

#include <boost/make_shared.hpp>


#include <iostream>

using namespace std;

static string equations =
    "\n\
dc/dt = k11*J11^n11/(J11^n11+c^n11)*g+k12*c^n12/(J12^n12+c^n12)-(k14+k15*vp^n15/(vp0^n15+vp^n15))*c\n\
dg/dt = (k21*J21^n21/(J21^n21+(c)^n21)-k22*g)\n\
di/dt = (k31*c-k32*i)\n\
dz/dt = (k41*(1-z)/(0.01+(1-z))-(k42+k43*(i/i0)^n43)*z/(0.1+z))\n\
dv/dt = (k51*c+k52*vp-k53*(1-z)*v-k54*v)\n\
dvp/dt = (-k52*vp+k53*(1-z)*v-k54*vp)\n\
";

int main(int, char**) {
    ModelPtr model =  std::make_shared<XDEModel>(equations);
    cerr << "Model compiled\n";

    // Some shortcut here: Currently variable order is ASCII alphabetical

    vector<double> depVars = {0.11, 0.83, 0.23, 1.1, 0.35, 1};
    vector<double> modelParams = {
        0.4,   0.4,   0.2,  0.85, 0.004,   0.0455, 0.002,   0.15, 0.046,
        0.022, 0.075, 0.07, 0.8,  0.00010, 0.6,    0.00905, 0.03, 0.13,
        0.003, 2.0,   2.0,  2.0,  2.0,     4.0,    0.5};

    TDoubleVector dv(depVars.size());
    TDoubleVector mp(modelParams.size());

    copy(depVars.begin(), depVars.end(), dv.begin());
    copy(modelParams.begin(), modelParams.end(), mp.begin());


    model -> setModelParameterValues(mp);
    TDoubleVector y0;

    SundialsSolverWrapper solver;


    vector<double> tspan;
    for (double d = 0; d <=120; d+=0.1) tspan.push_back(d);
    TDoubleVector td(tspan.size());
    copy(tspan.begin(), tspan.end(), td.begin());

    solver.setTSpan(td);
    TDoubleVector tout;
    TDoubleMatrix yout;
    solver.solve(model, mp, dv, tout, yout);

    cerr << "Returned matrix size: " << yout.size1() << ", " << yout.size2() << endl;


    for (unsigned i = 0; i < yout.size1(); i++) {
        for (unsigned j = 0; j < yout.size2(); j++) {
            cerr << yout(i, j) << ", ";
        }
        cerr << endl;
        if (i > 15) break;

    }

}