#include <extra_parser_functions/TVFunctionMapping.h>
#include <assert.h>
#include <iostream>
#include <extra_parser_functions/LOCF.h>
TVFunctionMapping tvfMapping("x");



int main() {
    assert(tvfMapping.getMappedSymbol().compare("x")==0);
    TimeVaryingFunctionPtr tvf(new LOCF());

    Vec points;
    points.push_back(std::pair<double, double>(0, 3));
    points.push_back(std::pair<double, double>(1, 8));
    points.push_back(std::pair<double, double>(2, 4));

    tvfMapping.assignControlPoints(tvf, points);
    tvf -> initialize();

    
    for (double d = 0; d<4; d+=0.25) {
        std::cerr << d << ": " << tvf->compute(d) << std::endl;
    }


}















