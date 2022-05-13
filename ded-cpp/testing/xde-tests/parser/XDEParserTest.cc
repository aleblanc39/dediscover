/**
 * Program to test the XDEParser.
 */

#include <TestUtils.h>
#include <base/XDEException.h>
#include <base/XDEMessage.h>
#include <base/XDEUtil.h>
#include <expressions/MuParserExpressionHandler.h>
#include <parser/ParserException.h>
#include <parser/XDEParser.h>

#include <fstream>
#include <iostream>
#include <vector>

#include "doctest.h"

using namespace std::placeholders;

using namespace std;


SCENARIO("We parse a valid model") {
    GIVEN("A model with delays") {
        string equationsWithDelays =
            "\
# Simple set of equations.\n\
temp_var= 1.3e-2 * k*T1*V\n\
MAC1 = T1[t-10] * TT[t - 5]\n\
dT1/dt  = lambda - dT*T1[t-5] - 4.2e-1 + temp_var\n\
dTT/dt  = 24 + temp_var - delta*TT + MAC1\n\
dV/dt   = N*delta*TT - sin(3) + c*V\n\
\n\
# The initial conditions \n\
T1[t0] = 5326.23\n\
TT[t0] = 774.92\n\
V[t0] = 5595.81\n";

        MuParserExpressionHandlerPtr eh(new MuParserExpressionHandler());

        try {
            XDEParser parser(eh, equationsWithDelays);

            auto depVars = parser.getDependentVariables();
            auto macros = parser.getMacros();
            auto modelParameters = parser.getModelParameters();
            auto covariates = parser.getCovariates();

            THEN("There are 3 dependent variables") {
                CHECK(depVars.size() == 3);
            }
            THEN("There is a dependent variable named T1") {
                CHECK(find_symbol(depVars, "T1")->getName() == "T1");
            }
            THEN("There is a dependent variable named TT") {
                CHECK(find_symbol(depVars, "TT")->getName() == "TT");
            }
            THEN("There is a dependent variable named V") {
                CHECK(find_symbol(depVars, "V")->getName() == "V");
            }
            THEN("There are 6 model parameters") {
                CHECK(modelParameters.size() == 6);
            }
            THEN("There are 2 macros") { CHECK(macros.size() == 2); }

            std::function<double(int)> f =
                std::bind(&MuParserExpressionHandler::compute2, _1, eh);

            THEN("HistoryIntervals returns a vector of 3 values.") {
                CHECK_EQ(parser.getHistoryIntervals()->getHistory(1, f).size(),
                         3);
            }

            THEN("There are 2 delay expressions") {
                CHECK_EQ(2, parser.getDelayedVariableHandler()
                                ->getUniqueTimes(f)
                                .size());
            }

            THEN("There are 3 values returned when computing delays") {
                TDoubleMatrix m(3, 2);
                CHECK_EQ(3, parser.getDelayedVariableHandler()
                                ->arrangeDelayVariableValues(m)
                                .size());
            }
        } catch (ParserException ex) {
            std::cerr << ex.getLineNO() << std::endl;
            std::cerr << ex.getMsg() << std::endl;
            exit(-1);
        }
    }
}

SCENARIO("A model with line continuation") {
    GIVEN("A 3-line model with 2 variables, 1 model param") {
        string model = "dx/dt = alpha \\\n+1\ndy/dt = y/1";
        try {
            MuParserExpressionHandlerPtr eh(new MuParserExpressionHandler());
            XDEParser parser(eh, model);
            THEN("There is no error"){CHECK_EQ(0,parser.getErrorMessages().size());}
            THEN("There are 2 dependent variables") {CHECK_EQ(2, parser.getDependentVariables().size());}
            THEN("There is 1 model parameter") {CHECK_EQ(1, parser.getModelParameters().size());}
        } catch (ParserException ex) {
            std::cerr << ex.getLineNO() << std::endl;
            std::cerr << ex.getMsg() << std::endl;
            exit(-1);
        }

    }
}