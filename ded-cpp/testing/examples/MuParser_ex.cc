// Playing around with muparser

#include <iostream>
#include <string>

#include "muParser.h"
#include "muParserDef.h"
#include <math.h>   
using namespace std;

static string equations = "v4^3.1, v2^7";

int main(int, char**) {
    mu::Parser parser;

    double v1, v2, v3, v4;
    double r1, r2, r3, r4;


    parser.DefineVar("v1", &v1);
    parser.DefineVar("v2", &v2);
    parser.DefineVar("v3", &v3);
    parser.DefineVar("v4", &v4);

    parser.DefineVar("r1", &r1);
    parser.DefineVar("r2", &r2);
    parser.DefineVar("r3", &r3);
    parser.DefineVar("r4", &r4);
    parser.SetExpr(equations);
 

    v2=3.0;
    v4=2.0;
    int nresults;
    double *results = parser.Eval(nresults);
    //cerr << "r1, r3 now " << r1 << ",  " << r3 << endl;

    cerr << "returned values:\n";
    for (int i = 0; i < nresults; i++) {
        cerr << results[i] << ", ";
    }
    cerr << endl;

    cerr << "Second call\n";
    results = parser.Eval(nresults);
    for (int i = 0; i < nresults; i++) {
        cerr << results[i] << ", ";
    }
    cerr << endl;


    cerr << "Testing exp in c++\n";
    double x = pow(v2, 5.0);
    cerr << x << endl;


}