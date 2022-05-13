#include "doctest.h"
#include <unistd.h>
#include <string>

#include <expressions/DelayedVariable.h>

SCENARIO("Delayed Variable Creation") {

  GIVEN("Two delayed variable assignations") {

    THEN("The names should have different numbers.") {
        DelayedVariable dv1("var1", 4);
        DelayedVariable dv2("var2", 7);
        CHECK(dv1.getInternalName() == "var1___0");
        CHECK(dv2.getInternalName() == "var2___1");
        CHECK(dv1.getExpressionID() == 4);
        CHECK(dv2.getExpressionID() == 7);

    }
    
  }
}


    