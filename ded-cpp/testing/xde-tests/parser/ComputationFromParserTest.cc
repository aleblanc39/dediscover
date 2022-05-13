
#include <TestUtils.h>
#include <base/XDEMessage.h>
#include <base/XDEUtil.h>
#include <expressions/MuParserExpressionHandler.h>
#include <parser/XDEParser.h>

#include <iostream>
#include <vector>

#include "doctest.h"

using namespace std;

static string equations =
    "temp_var= k*T1*V\n\
dT1/dt  = lambda - dT*T1  + temp_var\n\
dTT/dt  = 24 + temp_var - delta*TT\n\
dV/dt   = N*delta*TT + c*V\n\
\n\
T1[t0] = 5326.23\n\
TT[t0] = 774.92\n\
V[t0] = 5595.81\n";

SCENARIO("A parsed model with many equations and values assigned") {
    ExpressionHandlerPtr handler(new MuParserExpressionHandler());
    XDEParser parser(handler, equations);
    auto depVars = parser.getDependentVariables();


    GIVEN ("We assign values to the variables") {
        int t = 5;
        handler->setVariableValue("t", t);
        handler->setVariableValue("T1", 2.5);
        handler->setVariableValue("TT", 2);
        handler->setVariableValue("V", 1);

        handler->setVariableValue("lambda", 5);
        handler->setVariableValue("k", 10);
        handler->setVariableValue("dT", 20);
        handler->setVariableValue("delta", 30);
        handler->setVariableValue("N", 40);
        handler->setVariableValue("c", 50);
        THEN("Macro temp_var should be set to 25"){CHECK(handler->compute(parser.getMacros()[0]->getExpressionObject()->getEquationID())== 25);}
    }

    GIVEN("We compute the derivatives") {
        int t = 5;
        handler->setVariableValue("t", t);
        handler->setVariableValue("T1", 2.5);
        handler->setVariableValue("TT", 2);
        handler->setVariableValue("V", 1);

        handler->setVariableValue("lambda", 5);
        handler->setVariableValue("k", 10);
        handler->setVariableValue("dT", 20);
        handler->setVariableValue("delta", 30);
        handler->setVariableValue("N", 40);
        handler->setVariableValue("c", 50);

        handler->setVariableValue("temp_var", 25);
        THEN("V should be set to 2450"){
            auto param = find_if(depVars.begin(), depVars.end(), 
            [](const DependentVariablePtr &p) {return p->getName() == "V";});
            CHECK(handler->compute(
                              (*param)->getDerivativeExpression()->getEquationID())== 2450);
        }

        THEN("TT should be set to -11"){
            auto param = find_if(depVars.begin(), depVars.end(), 
            [](const DependentVariablePtr &p) {return p->getName() == "TT";});
            CHECK(handler->compute(
                              (*param)->getDerivativeExpression()->getEquationID())== -11);
        }
        THEN("T1 should be set to -20"){
            auto param = find_if(depVars.begin(), depVars.end(), 
            [](const DependentVariablePtr &p) {return p->getName() == "T1";});
            CHECK(handler->compute(
                              (*param)->getDerivativeExpression()->getEquationID())== -20);
        }
    }
}