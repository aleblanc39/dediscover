#include <base/XDEMessage.h>
#include <base/MemoryBank.h>
#include <doctest.h>
#include <expressions/MuParserExpressionEvaluator.h>
#include <expressions/MemoryBankSynchronizer.h>

#include <iostream>

using namespace std;

SCENARIO("Testing computation of the evaluator function") {
    MemoryBank memBank;
    memBank.setValue("x", 3);
    memBank.setValue("y", 6);

    MemorySynchronizerPtr memSync(new MemoryBankSynchronizer(&memBank));
    GIVEN("An expression with an indicator string") {
        //  Expression will be 2*x + y + I(t>=x^2)*5
        string indicator =
            MuParserExpressionEvaluator::getIndicatorString("t", ">=", "x^2");
        string expression = "2*x + y + ";
        expression.append(indicator).append("*5");
        MuParserExpressionEvaluator expr(expression, memSync);
        
        THEN("Computed value at t==6 will be 12"){memBank.setValue("t", 6); CHECK(expr.evaluate() == 12);}
    }
}



// void testIndicatorFunctions() {
    
    
//     int testNO = 0;

//     memBank.setValue("t", 6);
//     evaluate(testNO++, &expr, 12);

//     memBank.setValue("t", 23);
//     evaluate(testNO++, &expr, 17);
// }

// void evaluate(int testNO, ExpressionEvaluator *expr, double expectedResult) {
//     if (expr->evaluate() != expectedResult) {
//     }
// }

// /**
//  * Test different cases for the indicator string.
//  * Here will just all sign combinations. More extensive tests using
//  * variables will be performed in another method.
//  */

// void testIndicatorString() {
//     // TODO tests with NaN, INF
//     testIndicatorString(ExpressionEvaluator::getIndicatorString("0", ">", "1"),
//                         0);
//     testIndicatorString(ExpressionEvaluator::getIndicatorString("0", ">=", "1"),
//                         0);
//     testIndicatorString(ExpressionEvaluator::getIndicatorString("0", "<", "1"),
//                         1);
//     testIndicatorString(ExpressionEvaluator::getIndicatorString("0", "<=", "1"),
//                         1);
//     testIndicatorString(ExpressionEvaluator::getIndicatorString("0", "=", "1"),
//                         0);
//     testIndicatorString(ExpressionEvaluator::getIndicatorString("1", ">", "1"),
//                         0);
//     testIndicatorString(ExpressionEvaluator::getIndicatorString("1", ">=", "1"),
//                         1);
//     testIndicatorString(ExpressionEvaluator::getIndicatorString("1", "<", "1"),
//                         0);
//     testIndicatorString(ExpressionEvaluator::getIndicatorString("1", "<=", "1"),
//                         1);
//     testIndicatorString(ExpressionEvaluator::getIndicatorString("1", "=", "1"),
//                         1);
//     testIndicatorString(ExpressionEvaluator::getIndicatorString("1", ">", "0"),
//                         1);
//     testIndicatorString(ExpressionEvaluator::getIndicatorString("1", ">=", "0"),
//                         1);
//     testIndicatorString(ExpressionEvaluator::getIndicatorString("1", "<", "0"),
//                         0);
//     testIndicatorString(ExpressionEvaluator::getIndicatorString("1", "<=", "0"),
//                         0);
//     testIndicatorString(ExpressionEvaluator::getIndicatorString("1", "=", "0"),
//                         0);

//     // In ExpressionEvaluator the function to evaluate the expression
//     // with 2 operators calls the function with one operator twice, so
//     // we won't go through every single case.
//     testIndicatorString(
//         ExpressionEvaluator::getIndicatorString("1", "<", "0", "<", "1"), 0);
//     testIndicatorString(
//         ExpressionEvaluator::getIndicatorString("-1", "<", "0", "<", "1"), 1);
//     testIndicatorString(
//         ExpressionEvaluator::getIndicatorString("-1", ">", "0", ">", "1"), 0);
//     testIndicatorString(
//         ExpressionEvaluator::getIndicatorString("-1", "<", "0", "<=", "0"), 1);
//     testIndicatorString(
//         ExpressionEvaluator::getIndicatorString("-1", "<", "0", "<", "0"), 0);
//     testIndicatorString(
//         ExpressionEvaluator::getIndicatorString("0", "<", "0", "<=", "0"), 0);
// }

// void testIndicatorString(string indicatorString, double expectedResult) {
//     MemoryBank memBank;
//     MemorySynchronizerPtr memSync(new MemoryBankSynchronizer(&memBank));
//     ExpressionEvaluator expr(indicatorString, memSync);

//     if (expr.evaluate() != expectedResult) {
//         cout << "Indicator string test failed with expression: "
//              << indicatorString << endl;
//         cout << "Expecting " << expectedResult << ". Returned "
//              << expr.evaluate() << endl;
//         errorsFound = true;
//     }
// }
