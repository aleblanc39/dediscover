#include <solver/SambineDDEWrapper.h>
#include <solver/SolverFactory.h>
#include <solver/SundialsSolverWrapper.h>
#include <unistd.h>

#include <string>
#include <vector>

#include "doctest.h"

using namespace std;

SCENARIO("Create solvers from a Solver Factory") {
    GIVEN("A Factory") {
        SolverFactory factory;
        vector<string> knownSolvers = factory.getKnownMethods();
        string expectedSolvers[] = {SundialsSolverWrapper::s_getMethodName(),
                                    SambineDDEWrapper::s_getMethodName()};

        THEN("There should be some solvers created properly")
        CHECK(knownSolvers.size() ==
              sizeof(expectedSolvers) / sizeof(expectedSolvers[0]));
        CHECK(typeid(*factory.createMethod(SundialsSolverWrapper::s_getMethodName())) == typeid(SundialsSolverWrapper));
        CHECK(typeid(*factory.createMethod(SambineDDEWrapper::s_getMethodName())) == typeid(SambineDDEWrapper));
                  
    }
}
