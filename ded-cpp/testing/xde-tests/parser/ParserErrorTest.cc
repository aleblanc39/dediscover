#include <base/XDEException.h>
#include <expressions/MuParserExpressionHandler.h>
#include <parser/ParserException.h>
#include <parser/XDEParser.h>

#include <exception>
#include <iostream>

#include "doctest.h"
using namespace std;

SCENARIO("Tests possible errors in model.") {


    GIVEN("An empty string") {
        XDEParser parser(ExpressionHandlerPtr(new MuParserExpressionHandler()), "");
        THEN ("Should have 0 errors"){CHECK(parser.getErrorMessages().size() == 0);}
    }


    GIVEN("There is a model with no error") {
        XDEParser parser(ExpressionHandlerPtr(new MuParserExpressionHandler()),
                         string("dx/dt=1\n\
			x[t0]=1\n\
			dX/dt = 3*X[t0] + alpha * Y - X[alpha - 1]\n\
			dX_x/dt = 3*X[t0] + X_x[t-1] + alpha * Y - X[alpha - 1]\n	\
			dY/dt = 2*Y[t0] + ind(0 <=t< 12)*X[t-10] + beta\n\
			dZ/dt = 3*Y[t-8]  * t0 + beta + ind(0<=t<=5)*X[t-5]\n\
			// Comment the whole line\n			     \
			Y[t0] = MAC1\n\
			MAC1 = 1 + alpha\n\
			MAC2 = t\n\
			Y[t] = 1/t, -INF < t < -5\n\
			Y[t] = 1/t^2, -5 <= t < 0\n\
			X[t0] = 3   # Another comment\n\
			X[t] = 2*t + alpha, -10 <= t < 0\n\
			X[t]=-100, -INF<t< t0\n\
			Y[t]=0, -INF<=t<=0"));
        THEN("There should be no error") {
            CHECK(parser.getErrorMessages().size() == 0);
        }
    }

    GIVEN("A dependent variable is defined twice") {
        XDEParser parser(ExpressionHandlerPtr(new MuParserExpressionHandler()),
                         "dX/dt = 3*X[t0] + alpha * Y\n\
         dX/dt = 3*X[t0] + alpha * Y\n\
		dY/dt = 2*Y[t0] + ind(0 <=t< 12)*X[t-10] + beta\n\
		dZ/dt = 3*Y[t-8]  * beta + ind(0<=t<=5)*X[t-5]\n\
		Y[t] = 1/t, -INF < t < -5\n\
		Y[t] = 1/t^2, -5 <= t < 0\n\
		X[t] = 2*t + alpha, -10 <= t < 0\n");

        THEN("Will be one error") {
            CHECK(parser.getErrorMessages().size() == 1);
        }
    }

    GIVEN("A Model with only one bad line") {
        XDEParser parser(ExpressionHandlerPtr(new MuParserExpressionHandler()),
                         "dx/d = 3*x\n");

        THEN("Will be some errors") {
            CHECK(parser.getErrorMessages().size() > 0);
        }
    }

    GIVEN("A Model with the indicator function as a dependent variable") {
        XDEParser parser(ExpressionHandlerPtr(new MuParserExpressionHandler()),
                "dX/dt = 3*X[t0] + alpha * Y\n\
                dY/dt = 2*Y[t0] + ind(0 <=t< 12)*X[t-10] + beta\n\
                dind/dt = 3*Y[t-8]  * beta + ind(0<=t<=5)*X[t-5]\n\
                Y[t] = 1/t, -INF < t < -5\n\
                Y[t] = 1/t^2, -5 <= t < 0\n\
                X[t] = 2*t + alpha, -10 <= t < 0\n");

        THEN("Will be  some errors") {
            cerr << parser.getErrorMessages()[0].second << endl;
            CHECK(parser.getErrorMessages().size() > 0);
        }
    }



 GIVEN("A Model with a cycle in the macros") {
        XDEParser parser(ExpressionHandlerPtr(new MuParserExpressionHandler()),
                " dx/dt = 1 \n\
                X = x + Z \n\
                 Y = X - 1 \n\
                 Z = Y/2\n");

        THEN("Will be  some errors") {
            cerr << parser.getErrorMessages()[0].second << endl;
            CHECK(parser.getErrorMessages().size() > 0);
        }
    }

}

static string model[] = {

    // 3 - Error at line 5. Using a dep var already used for a macro.
    "dY/dt = 2*Y[t0] + ind(0 <=t< 12)*X[t-10] + beta\n\
        dX/dt = 3*X[t0] + 34e-5  * alpha * Y\n\
        XX=Y+1\n\
\n\
\n\
\n\
\n\
dZ/dt = 3*Y[t-8]  * beta + ind(0<=t<=5)*X[t-5]\n\
	dXX/dt = 3*Y[t-8]  * beta + ind(0<=t<=5)*X[t-5]\n\
	Y[t] = 1/t, -INF < t < -5\n\
	Y[t] = 1/t^2, -5 <= t < 0\n\
	X[t] = 2*t + alpha, -10 <= t < 0\n",

    // 4 - Error with float point number, line 2

    "dY/dt = 2*Y[t0] + ind(0 <=t< 12)*X[t-10] + beta\n\
        dX/dt = 3*X[t0] + 34e * * alpha * Y\n\
        XX=Y+1\n\
	dZ/dt = 3*Y[t-8]  * beta + ind(0<=t<=5)*X[t-5]\n\
	Y[t] = 1/t, -INF < t < -5\n\
	Y[t] = 1/t^2, -5 <= t < 0\n\
	X[t] = 2*t + alpha, -10 <= t < 0\n",

    // 5 - Error with undefined function zzz
    "dY/dt = 2*Y[t0] + ind(0 <=t< 12)*X[t-10] + beta\n\
        dX/dt = 3*X[t0] +  alpha * Y\n\
        XX=Y+1 + zzz(0)\n\
	dZ/dt = 3*Y[t-8]  * beta + ind(0<=t<=5)*X[t-5]\n\
	Y[t] = 1/t, -INF < t < -5\n\
	Y[t] = 1/t^2, -5 <= t < 0\n\
	X[t] = 2*t + alpha, -10 <= t < 0\n",

    // 6 -Error: using function name sin as dep var or model parameter

    "dY/dt = 2*Y[t0] + ind(0 <=t< 12)*X[t-10] + beta\n\
        dX/dt = 3*X[t0] +  sin * Y\n\
        macmacXX=Y+1 + A\n\
	dZ/dt = 3*Y[t-8]  * beta + ind(0<=t<=5)*X[t-5]\n\
	dXX/dt = 3*Y[t-8]  * beta + ind(0<=t<=5)*X[t-5]\n\
	Y[t] = 1/t, -INF < t < -5\n\
	Y[t] = 1/t^2, -5 <= t < 0\n\
	X[t] = 2*t + alpha, -10 <= t < 0\n",

    // 7 - Error: another bad use of function name: sin[1]
    "dY/dt = 2*Y[t0] + ind(0 <=t< 12)*X[t-10] + beta\n\
        dX/dt = 3*X[t0] +  sin[1] * Y\n\
        macXX=Y+1 + A\n\
	dZ/dt = 3*Y[t-8]  * beta + ind(0<=t<=5)*X[t-5]\n\
	dXX/dt = 3*Y[t-8]  * beta + ind(0<=t<=5)*X[t-5]\n\
	Y[t] = 1/t, -INF < t < -5\n\
	Y[t] = 1/t^2, -5 <= t < 0\n\
	X[t] = 2*t + alpha, -10 <= t < 0\n",

    // 8 - Error: using reserved word INF with []
    "dY/dt = 2*Y[t0] + ind(0 <=t< 12)*X[t-10] + beta\n\
        dX/dt = 3*X[t0] +  INF[1] * Y\n\
        macXX=Y+1 + A\n\
	dZ/dt = 3*Y[t-8]  * beta + ind(0<=t<=5)*X[t-5]\n\
	dXX/dt = 3*Y[t-8]  * beta + ind(0<=t<=5)*X[t-5]\n\
	Y[t] = 1/t, -INF < t < -5\n\
	Y[t] = 1/t^2, -5 <= t < 0\n\
	X[t] = 2*t + alpha, -10 <= t < 0\n",

    // 9 - Error: function name sin used as macro
    "dY/dt = 2*Y[t0] + ind(0 <=t< 12)*X[t-10] + beta\n\
        dX/dt = 3*X[t0] +  X[1] * Y\n\
        sin=Y+1 + A\n\
	dZ/dt = 3*Y[t-8]  * beta + ind(0<=t<=5)*X[t-5]\n\
	dXX/dt = 3*Y[t-8]  * beta + ind(0<=t<=5)*X[t-5]\n\
	Y[t] = 1/t, -INF < t < -5\n\
	Y[t] = 1/t^2, -5 <= t < 0\n\
	X[t] = 2*t + alpha, -10 <= t < 0\n",

    // Some of the same tests but in the history functions.

    // 10 - sin used as model param.

    "dY/dt = 2*Y[t0] + ind(0 <=t< 12)*X[t-10] + beta\n\
        dX/dt = 3*X[t0] +  X[1] * Y\n\
        MAC1=Y+1 + A\n\
	dZ/dt = 3*Y[t-8]  * beta + ind(0<=t<=5)*X[t-5]\n\
	dXX/dt = 3*Y[t-8]  * beta + ind(0<=t<=5)*X[t-5]\n\
	X[t] = 2*t + alpha, -INF <= t < 0\n\
	Y[t] = 1/t + sin, -INF < t < -5\n \
	Y[t] = 1/t^2, -5 <= t < 0\n",

    // 11 - Non numerical in boundary conditions.
    "dY/dt = 2*Y[t0] + ind(0 <=t< 12)*X[t-10] + beta\n\
        dX/dt = 3*X[t0] +  X[1] * Y\n\
        MAC1=Y+1 + A\n\
	dZ/dt = 3*Y[t-8]  * beta + ind(0<=t<=5)*X[t-5]\n\
	dXX/dt = 3*Y[t-8]  * beta + ind(0<=t<=5)*X[t-5]\n\
	X[t] = 2*t + alpha, -INF <= t < 0\n\
	Y[t] = 1/t + X, -INF < t < beta\n \
	Y[t] = 1/t^2, -5 <= t < 0\n",

    // 12 - Using non-dependent variable in history
    "dY/dt = 2*Y[t0] + ind(0 <=t< 12)*X[t-10] + beta\n\
        dX/dt = 3*X[t0] +  X[alpha - 1] * Y\n\
        MAC1=Y+1 + A\n\
	dZ/dt = 3*Y[t-8]  * beta + ind(0<=t<=5)*X[t-5]\n\
	dXX/dt = 3*Y[t-8]  * beta + ind(0<=t<=5)*X[t-5]\n\
	Y[t] = 1/t + X, -INF < t < 0\n\
	beta[t] = 2*t + alpha, -INF <= t < 0\n\
	Y[t] = 1/t^2, -5 <= t < 0\n",

    // 13 - Using non-dependent variable in history, beta line 13
    "dY/dt = 2*Y[t0] + ind(0 <=t< 12)*X[t-10] + beta\n\
        dX/dt = 3*X[t0] +  beta[alpha - 1] * Y\n\
        MAC1=Y+1 + A\n\
	dZ/dt = 3*Y[t-8]  * beta + ind(0<=t<=5)*X[t-5]\n\
	dXX/dt = 3*Y[t-8]  * beta + ind(0<=t<=5)*X[t-5]\n\
	Y[t] = 1/t + X, -INF < t < 0\n\
	Y[t] = 1/t^2, -5 <= t < 0\n",

    // 14 - Using non-dependent variable A with initial condition, line 3
    "dY/dt = 2*Y[t0] + ind(0 <=t< 12)*X[t-10] + beta\n\
        dX/dt = 3*X[t0] +  X[alpha - 1] * Y\n\
        MAC1=Y+1 + A[t0]\n\
	dZ/dt = 3*Y[t-8]  * beta + ind(0<=t<=5)*X[t-5]\n\
	dXX/dt = 3*Y[t-8]  * beta + ind(0<=t<=5)*X[t-5]\n\
	Y[t] = 1/t + X, -INF < t < 0\n\
	Y[t] = 1/t^2, -5 <= t < 0\n",

    // 15 - Using non-dependent variable with initial condition
    "dY/dt = 2*Y[t0] + ind(0 <=t< 12)*X[t-10] + beta\n\
        dX/dt = 3*X[t0] +  X[alpha - 1] * Y\n\
        MAC1=Y+1 + A\n\
        MAC2[t0] = 23\n\
	dZ/dt = 3*Y[t-8]  * beta + ind(0<=t<=5)*X[t-5]\n\
	dXX/dt = 3*Y[t-8]  * beta + ind(0<=t<=5)*X[t-5]\n\
	Y[t] = 1/t + X, -INF < t < 0\n\
	Y[t] = 1/t^2, -5 <= t < 0\n",

    // 16 - Another error with float point number?

    "dY/dt = 2*Y[t0] + ind(0 <=t< 12)*X[t-10] + beta\n\
        dX/dt = 3*X[t0] + 34e5a * alpha * Y\n\
        macXX=Y+1\n\
	dZ/dt = 3*Y[t-8]  * beta + ind(0<=t<=5)*X[t-5]\n\
	Y[t] = 1/t, -INF < t < -5\n\
	Y[t] = 1/t^2, -5 <= t < 0\n\
	X[t] = 2*t + alpha, -10 <= t < 0\n",

    // 17 - Test with a missing operator

    "dY/dt = 2*Y[t0] + ind(0 <=t< 12)*X[t-10] + beta\n\
        dX/dt = 3*X[t0] + 34e5 * alpha * Y\n\
        macXX=Y+1 + A 1\n\
	dZ/dt = 3*Y[t-8]  * beta + ind(0<=t<=5)*X[t-5]\n\
	Y[t] = 1/t, -INF < t < -5\n\
	Y[t] = 1/t^2, -5 <= t < 0\n\
	X[t] = 2*t + alpha, -10 <= t < 0\n",

    

    // 19 - Test with a macro as a variable.

    "dx/dt = LOCF(MAC1, (1,2), (3,4))\n\
         MAC1 = 2\n",

    // 20 - Test with initial conditions involving a macro and a cucle

    "dx/dt = locf(t, (1,2), (3,4))\n\
         dy/dt=1\n\
         dz/dt=3\n\
         x[t0] = mac1+1\n\
         mac1 = y[t0] + z[omega]\n\
         y[t0] = omega + x[t0] + alpha +  1\n",

    // 21 - Test with initial conditions involving a dependent variable

    "dx/dt = locf(t, (1,2), (3,4))\n\
         dy/dt=1\n\
         dz/dt=3\n\
         x[t0] = y\n",

    // 22 - Test with an initial condition involving time dependency through
    // macro.

    "dx/dt = locf(t, (1,2), (3,4))\n\
         dy/dt=1\n\
         dz/dt=3\n\
         x[t0] = MAC1\n\
         MAC1 = y",

};