/**
 * Program to test the spirit parser under various conditions.
 */

#include <TestUtils.h>
#include <parser/SpiritParser.h>

#include <iostream>

using namespace ParseTreeComponents;
using namespace std;

#include <doctest.h>

SCENARIO(
    "Small examples of DED code sent to spirit parser, will validate error/no "
    "error") {
    GIVEN("A simple model with two statement") {
        string eqns = "dx/dt=3*x+1\nY=f(4)";
        THEN("There should be no error returned") {
            CHECK(SpiritParser::parseEquations(eqns)->getNbErrors() == 0);
        }
    }
}

SCENARIO("Model with history functions, will validate error/no error") {
    GIVEN("A simple model with history functions") {
        string eqns = "dx/dt=3*x+1\nY=f(4)\nx[t]=2, -INF<t<t0";
        THEN("There should be no error returned") {
            CHECK(SpiritParser::parseEquations(eqns)->getNbErrors() == 0);
        }
    }
}

SCENARIO("Model with covariates, will validate error/no error") {
    GIVEN("A simple model with covariates") {
        string eqns = "\
            dx/dt=1\n\
            A = X\n\
            Covariate{X, t-alpha}\n";
        THEN("There should be no error returned") {
            CHECK(SpiritParser::parseEquations(eqns)->getNbErrors() == 0);
        }
    }
}

SCENARIO("Model with error on first line, will validate error/no error") {
    GIVEN("A simple model with an error on first line") {
        string eqns = "dx/dt=x;\n";
        THEN("There should some error returned") {
            CHECK(SpiritParser::parseEquations(eqns)->getNbErrors() > 0);
        }
    }
}

SCENARIO("Model with if statement, will validate error/no error") {
    GIVEN("A simple model with an if statement") {
        THEN("There should no error returned") {
            string eqns = "dx/dt = if(a,b,c)\n";
            CHECK(SpiritParser::parseEquations(eqns)->getNbErrors() == 0);
        }
    }
}

SCENARIO("Model with piecewise function, will validate error/no error") {
    GIVEN("A simple model with a piecewise function") {
        THEN("There should no error returned") {
            string eqns = "\
                    dx/dt = x + alpha2\n\
                    dz/dt = Piecewise(2, x<3, 0)\n";
            CHECK(SpiritParser::parseEquations(eqns)->getNbErrors() == 0);
        }
    }
}


SCENARIO("Model with time varying functions, will validate error/no error") {
    GIVEN("A simple model with a time varying function") {
        THEN("There should no error returned") {
            string eqns = "\
                    dx/dt=3*x+1\n\
                    Y=sin(4)\n\
                    x[t]=2, -INF<t<t0\n\
                    MAC1 = x + locf(t, [a,v], [4,5])\n";
            CHECK(SpiritParser::parseEquations(eqns)->getNbErrors() == 0);
        }
    }
}

SCENARIO("Model with time varying functions error, will validate error/no error") {
    GIVEN("A simple model with a time varying function containing an error") {
        THEN("There should be an  error returned") {
            string eqns = "dx/dt=3*x+1\n\
                            Y=f(4)\n\
                            x[t]=2, -INF<t<t0\n\
                            MAC1 = x + locf(t,[a,v])"; 
            CHECK(SpiritParser::parseEquations(eqns)->getNbErrors() > 0);
        }
    }
}

SCENARIO("Model with a covariate. There was a bug with a space between the word Covariate and the first {") {
    GIVEN("A model with covariate") {
        THEN("There should be no parsing error") {
            string eqns = "dx/dt=3*x+1\n\
                            Covariate{X, t-2}\n\
                            Covariate {Y, t-2}\n"; 
            CHECK(SpiritParser::parseEquations(eqns)->getNbErrors() == 0);
        }
    }
}

