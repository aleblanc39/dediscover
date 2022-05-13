/**
 * Validate a formula after a model has been parsed.
 */

#include <model/XDEModel.h>
#include <unistd.h>

#include <string>

#include "doctest.h"

SCENARIO("Validating a formula against a tested model") {
    XDEModel model(
        "temp_var= k*T1*V\n\
        dT1/dt  = lambda - dT*T1  + temp_var\n\
        dTT/dt  = 24 + temp_var - delta*TT\n\
        dV/dt   = N*delta*TT + c*V\n\
        \n\
        T1[t0] = 5326.23\n\
        TT[t0] = 774.92\n\
        V[t0] = 5595.81\n");

    THEN ("Formula T1 should not generate an error"){CHECK(model.validateFormula("T1").size() == 0);}
    THEN ("Formula T2 should generate an error"){CHECK(model.validateFormula("T2").size() > 0);}
    THEN ("Formula T1 + V should not generate an error"){CHECK(model.validateFormula("T1 + V").size() == 0);}
    THEN ("Formula T1 +  should  generate one or more errors"){CHECK(model.validateFormula("T1 + ").size() > 0);}

}

// TODO AL 2021 