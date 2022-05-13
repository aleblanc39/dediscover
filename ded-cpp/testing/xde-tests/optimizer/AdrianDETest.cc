#include <optimizer/AdrianDE.h>
#include <unistd.h>

#include <string>

#include "doctest.h"

SCENARIO("Test Basic Functionality") {
    GIVEN("A simple Optimizer") {
        AdrianDE ade;

        THEN("Some methods should be implemented, not be null") {
            CHECK(ade.getMethodName() == AdrianDE::s_getMethodName());
            CHECK(ade.getShortMethodDescription() ==
                  AdrianDE::s_getShortMethodDescription());
        }
    }
}
