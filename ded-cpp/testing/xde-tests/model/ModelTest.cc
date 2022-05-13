#include "doctest.h"
#include <unistd.h>
#include <string>

#include <model/XDEModel.h>


SCENARIO("Simple Model Computation") {

  GIVEN("A simple Model") {
    std::string s = "dx/dt=1\ndy/dt=MAC\n";
    XDEModel model = XDEModel(s);

    //    WHEN("call sleep with this duration") {
    //  int ret = sleep(duration) ;

      THEN("it's expected nobody interupted our rest") {
        CHECK(model.getDependentVariables().size() == 2);
      }
  }

  GIVEN("An empty model") {
    XDEModel model = XDEModel("");

      THEN("There should be one error message.") {
        CHECK(model.getErrorMessages().size() == 1);
      }
  }
}



