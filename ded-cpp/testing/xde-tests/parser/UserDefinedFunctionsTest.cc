#include <TestUtils.h>

#include <parser/XDEParser.h>
#include <doctest.h>

using namespace std;



SCENARIO("Testing with user defined functions.") {
    ExpressionHandlerPtr ex(new MuParserExpressionHandler());

    GIVEN("A model with a valid user defined function") {
        string str1 = "dx/dt=1\n\
                Z = x + gamma_p(a, 0.1)\n\
                B = gamma_p(0.2, -6)\n";

        THEN ("Parsed model should have no error") {XDEParser p(ex, str1); CHECK(p.getErrorMessages().size()== 0);}
    }
}
