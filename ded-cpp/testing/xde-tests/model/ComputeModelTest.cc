#include "doctest.h"
#include <unistd.h>
#include <string>

#include <model/XDEModel.h>


SCENARIO("Parse a Simple model") {

  GIVEN("A simple Model") {
    std::string s = "dx/dt=1\ndy/dt=MAC\n";
    XDEModel model = XDEModel(s);


      THEN("Check number of different types of symbols.") {
        CHECK(model.getDependentVariables().size() == 2);
      }
  }
}



