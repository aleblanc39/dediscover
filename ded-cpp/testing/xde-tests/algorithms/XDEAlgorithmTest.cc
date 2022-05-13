
#include <TestUtils.h>
#include <algorithms/XDEAlgorithm.h>
#include <solver/SundialsSolverWrapper.h>
#include "doctest.h"


SCENARIO("XDEAlgorithm Parameters testing") {
	SolverPtr solver(new SundialsSolverWrapper());
		double newValue = 0.1234;
		std::string paramName = SundialsSolverWrapper::rtolParamName;

		GIVEN("Assign a double value to a control parameter") {
			solver->setParameterValue(paramName, newValue);
			THEN("Check that we retrieve the same value"){CHECK_EQ(solver->getDoubleParameterValue(paramName),newValue);}

		}

}

// TODO Test other base function of XDEAlgorithm