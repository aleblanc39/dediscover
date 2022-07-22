#include <expressions/MuParserExpressionHandler.h>
#include <parser/ParseTree.h>

#include <doctest.h>

#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace ParseTreeComponents;




SCENARIO("Testing the MuParserExpressionHandler") {

    GIVEN("3 separate exprexxions") {
        MuParserExpressionHandler exprHandler;
        vector<string> expressions = {"2*x + y -3", "x^2 - 3", "3 * z * log2(z_64)"};
        vector<int> expressionIDs;

        for (auto expr: expressions) {
            expressionIDs.push_back(exprHandler.addExpression(expr));
        }
        THEN("There should be 3 expressions"){CHECK(exprHandler.getExpressions().size() == 3);}

        exprHandler.setVariableValue("x", 11.5);
        exprHandler.setVariableValue("y", 31.1);
        exprHandler.setVariableValue("z", -12);
        exprHandler.setVariableValue("z_64", 64);

        THEN("Computation of first expression yields 51.1"){CHECK(exprHandler.compute(expressionIDs[0]) == 51.1);}
        THEN("Computation of second expression yields 129.25"){CHECK(exprHandler.compute(expressionIDs[1]) == 129.25);}
        THEN("Computation of third expression yields -216"){CHECK(exprHandler.compute(expressionIDs[2]) == -216);}


    }
}
