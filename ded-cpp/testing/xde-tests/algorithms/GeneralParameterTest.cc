
#include <algorithms/ConcreteParameters.h>

#include <boost/assign/std/vector.hpp>
#include <exception>
#include <iostream>
#include <typeinfo>
#include <vector>

#include "doctest.h"
using namespace boost::assign;

using namespace std;

SCENARIO("General parameter testing") {
    IntParameter par("IntParam1", 0, 0, 300, true, false, "Desc for param 1");

    GeneralParameter *p = &par;
    GIVEN("An IntParameter Created") {
        THEN("It is an integer param: ") { CHECK(par.getType() == "GP"); }
        THEN("The ptr to param is the type id of a IntParameter: ") {
            CHECK(typeid(*p) == typeid(IntParameter));
        }
        THEN("The ptr to param is  not the type id of a IntParameter: ") {
            CHECK(typeid(*p) != typeid(DoubleParameter));
        }

        GIVEN("Assigning a string value") {
            p->setStringValue("10");
            THEN("Returns a string correctly") {
                CHECK(p->getStringValue() == "10");
            }
            THEN("Returns an int correctly") { CHECK(par.getValue() == 10); }
        }
        GIVEN("Assigning a int value") {
            par.setValue(15);
            THEN("Returns a string correctly") {
                CHECK(p->getStringValue() == "15");
            }
        }
        GIVEN("Assigning a negative int as string") {
            p->setStringValue("-10");
            THEN("Returns an int correctly") { CHECK(par.getValue() == -10); }
        }
        GIVEN("A bad value passed") {
            THEN("Throw exception on non-number") {
                CHECK_THROWS(p->setStringValue("a-10"));
            }
            THEN("Throw exception on double") {
                CHECK_THROWS(p->setStringValue("5.5"));
            }
        }
    }
}

SCENARIO("INT Parameter with options") {
    vector<int> options = {5, 15, 25, 35};

    IntParameter par2("IntParam2", 0, options, -3, 300, true, false,
                      "Desc for param 2");

    THEN("Validate min value") { CHECK(par2.getMin() == -3); }
    THEN("Validate max value") { CHECK(par2.getMax() == 300); }
    THEN("Validate that we can return the list of Strings") {
        CHECK(par2.getStringOptions().size() == 4);
    }
}

// TODO Test other types of parameters.
