#include <base/XDEBlas.h>
#include <doctest.h>
#include <statistics/MathUtil.h>

#include <vector>

SCENARIO("Testing straightLineInterploation") {
    GIVEN("Results with 4 variables and 6 timepoints") {
        unsigned nbVars = 4;
        std::vector<double> timepoints = {0, 1, 1.1, 2, 4, 5};
        std::vector<double> matrixData = {
            0,  1,  2,  3, 
            4,  5,  6,  7,
            8,  9,  10, 11, 
            12, 13, 14, 15,
            16, 17, 18, 19, 
            20, 21, 22, 23};

        TDoubleMatrix y(timepoints.size(), nbVars);
        TDoubleVector ret(nbVars);

        std::copy(matrixData.begin(), matrixData.end(), y.data().begin());

        THEN("Validate that the matrix is build correctly") {
            CHECK_EQ(y(4, 1), 17);
        }

        THEN("Validate left extrapolation") {
            MathUtil::straightLineInterpolation(timepoints, -1, y, ret);
            CHECK_EQ(y(0, 0), ret[0]);
            CHECK_EQ(y(0, 1), ret[1]);
            CHECK_EQ(y(0, 2), ret[2]);
            CHECK_EQ(y(0, 3), ret[3]);
        }

        THEN("Validate right extrapolation") {
            auto nt = timepoints.size()-1;
            MathUtil::straightLineInterpolation(timepoints, 8, y, ret);
            CHECK_EQ(y(nt, 0), ret[0]);
            CHECK_EQ(y(nt, 1), ret[1]);
            CHECK_EQ(y(nt, 2), ret[2]);
            CHECK_EQ(y(nt, 3), ret[3]);
        }

        THEN("Validate half way") {
            MathUtil::straightLineInterpolation(timepoints, 3, y, ret);
            CHECK_EQ(14, ret[0]);
            CHECK_EQ(15, ret[1]);
            CHECK_EQ(16, ret[2]);
            CHECK_EQ(17, ret[3]);
        }

        THEN("Validate 3/4 of way") {
            MathUtil::straightLineInterpolation(timepoints, 3.5, y, ret);
            CHECK_EQ(15, ret[0]);
            CHECK_EQ(16, ret[1]);
            CHECK_EQ(17, ret[2]);
            CHECK_EQ(18, ret[3]);
        }
        THEN("Validate on point") {
            MathUtil::straightLineInterpolation(timepoints, 1.1, y, ret);
            CHECK_EQ(8, ret[0]);
            CHECK_EQ(9, ret[1]);
            CHECK_EQ(10, ret[2]);
            CHECK_EQ(11, ret[3]);
        }

    }
}