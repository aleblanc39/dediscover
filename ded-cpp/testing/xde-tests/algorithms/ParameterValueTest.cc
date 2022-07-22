#include <algorithms/ParameterValue.h>
#include<any>
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

SCENARIO("Testing ParameterVAlueSet") {

    GIVEN("A ParameterValueSet") {



        
    }


}
