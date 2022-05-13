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
dPrey/dt = (birth - predation * Predator) * Prey\n\
dPredator/dt = (growth * Prey - death) * Predator\n";

int main(int, char**) {
    ModelPtr model =  std::make_shared<XDEModel>(equations);
    cerr << "Model compiled\n";

    // Some shortcut here: Currently variable order is ASCII alphabetical

    for (auto s: model->getModelParameters()) std::cerr << s->getName() << ", "; std::cerr << std::endl;
    for (auto s: model->getDependentVariables()) std::cerr << s->getName() << ", "; std::cerr << std::endl;


    vector<double> depVars = {2, 12};
    vector<double> modelParams = {4.0, 0.75, 0.25, 2.0};

    TDoubleVector dv(depVars.size());
    TDoubleVector mp(modelParams.size());

    copy(depVars.begin(), depVars.end(), dv.begin());
    copy(modelParams.begin(), modelParams.end(), mp.begin());


    model -> setModelParameterValues(mp);
    TDoubleVector y0;

    SundialsSolverWrapper solver;


    //vector<double> tspan = {1911,1913,1915,1917,1919,1921,1923,1925,1927,1929,1931,1933,1935,1937};
    vector<double> tspan = {115.0, 115.1, 115.2, 115.3, 115.4, 115.5, 115.6, 115.7, 115.8, 115.9, 116.0};

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