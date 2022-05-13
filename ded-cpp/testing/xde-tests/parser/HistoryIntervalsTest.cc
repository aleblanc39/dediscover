#include <doctest.h>
#include <parser/HistoryIntervals.h>

using namespace std;
using namespace std::placeholders;

static vector<int> args = {3, 7, 55, 11, 2};
static vector<double> vals = {-2, 3.5, 5.5, 102.4, 7.3};

class TestClass {
   public:
    map<int, double> fResults;
    TestClass() {
        for (unsigned i = 0; i < args.size(); i++) {
            fResults[args[i]] = vals[i];
        }
    }

    static double compute(const TestClass c, int n) {
        if (c.fResults.find(n) == c.fResults.end()) {
            return 0;
        }
        return c.fResults.at(n);
    }
};

SCENARIO("Examples of HistoryInterval objects") {
    GIVEN("A HistoryIntervals object with 3 variables, ") {
        vector<string> varNames = {"x", "y", "z"};

        multimap<string, pair<HistoryInterval, int>> intervals = {
            {"x", pair(HistoryInterval(-5, -4.2, true, false), args[0])},
            {"x", pair(HistoryInterval(-4.2, 0, true, true), args[1])},
            {"x", pair(HistoryInterval(-11, -8, true, true), args[2])},
            {"y", pair(HistoryInterval(2, 5, true, true), args[3])},
            {"y", pair(HistoryInterval(-12, 0, true, true), args[4])}};

        TestClass testClass;

        HistoryIntervals<string, int> hi(varNames, intervals);

        std::function<double(int)> myfunc =
            std::bind(&TestClass::compute, testClass, _1);

        vector<double> valuesAt0 = {3.5, 7.3, 0};
        vector<double> valuesAt2 = {0, 102.4, 0};
        vector<double> valuesAtM100 = {0, 0, 0};

        THEN("Values at 0") { CHECK_EQ(hi.getHistory(0, myfunc), valuesAt0); }
        THEN("Values at 2") { CHECK_EQ(hi.getHistory(2, myfunc), valuesAt2); }
        THEN("Values at -100") {
            CHECK_EQ(hi.getHistory(-100, myfunc), valuesAtM100);
        }
    }
}