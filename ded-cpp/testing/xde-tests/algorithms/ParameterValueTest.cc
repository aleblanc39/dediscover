#include <algorithms/ParameterValue.h>
#include <any>
#include <unordered_set>
#include <vector>
#include "doctest.h"
using namespace std;

SCENARIO("Looking at individual values")
{

    GIVEN("A Parameter=Value with an integer")
    {
        std::string paramName = "param1";
        auto paramValue = ParameterValue(paramName, std::make_any<int>(12));
        THEN("Param name is param1") { CHECK(paramValue.paramName == paramName); }
        THEN("Return value is 12"){CHECK(std::any_cast<int>(paramValue.value) == 12);}
        THEN("Return value is not 13"){CHECK(std::any_cast<int>(paramValue.value) != 13);}
    }

       GIVEN("A Parameter=Value with a vector of integers")
    {
        vector<int> values = {1,2,3};
        std::string paramName = "param1";
        auto paramValue = ParameterValue(paramName, std::make_any<vector<int>>(values));
        THEN("Return value is 12"){CHECK(std::any_cast<vector<int>>(paramValue.value).size() == 3);}
    }
}

SCENARIO("Testing ParameterValueSet") {

    GIVEN("A ParameterValueSet") {
    //     auto values = std::set<ParameterValue> {
    //         {"p1", std::make_any<int>(12)},
    //         {"p2", std::make_any<std::string &>("hello")}
    //     };
        auto values = std::vector<ParameterValue> {
            {"p1", std::make_any<int>(12)},
            {"p2", std::make_any<std::string &>("hello")}
        };

        ParameterValueMap pvs(values);

        THEN("The ParameterValueMap contains an element (p1, 12)") {
            CHECK(std::any_cast<int>(pvs.getValue("p1")) == 12);
        }
        THEN("The ParameterValueMap contains an element (p2, hello)") {
            CHECK(std::any_cast<std::string>(pvs.getValue("p2")).compare("hello") == 0);
        }
        THEN("We recognize an non-existing parameter") {
            CHECK_FALSE(pvs.getValue("noparam").has_value());
        }
        THEN("We recognize an non-existing parameter") {
            CHECK_FALSE(!pvs.getValue("p2").has_value());
        }

        THEN("Check getting a value directly") {
            CHECK(pvs.getIntParameterValue("p1", 40)==12);
        }

        THEN("Check default value ") {
            CHECK(pvs.getIntParameterValue("noparam", 40)==40);
        }


    }
}
