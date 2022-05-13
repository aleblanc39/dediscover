/**
 * Program to test muParser at the most basic level.
 */

#include <muParser.h>

#include <iostream>
#include <string>
#include <doctest.h>

using namespace std;


SCENARIO("Basic usage of muParser") {
mu::Parser parser;  

    double x, y, a, b, X;
    parser.DefineVar("x", &x);
    parser.DefineVar("y", &y);
    parser.DefineVar("a", &a);
    parser.DefineVar("b", &b);
    parser.DefineVar("X", &X);

    GIVEN("An expression with two additions") {
        a = 6;
        b = -5;
        parser.SetExpr("x=a+1, y=b+2");
        parser.Eval();

        THEN("Computed value of x should be 7"){CHECK(x==7);}
        THEN("Computed value of 7 should be -3"){CHECK(y==-3);}
    }

    //TODO Check the status on this. Was it ever valid with muparser?
    // GIVEN("An expression with two additions, with a last var assignation") {
    //     a = 6;
    //     b = -5;
    //     parser.SetExpr("X=(x=a+1, y=b+2, y)");
    //     parser.Eval();

    //     THEN("Computed value of x should be 7"){CHECK(x==7);}
    //     THEN("Computed value of y should be -3"){CHECK(y==-3);}
    //     THEN("Computed value of X should be -3"){CHECK(X==-3);}

    // }
}




