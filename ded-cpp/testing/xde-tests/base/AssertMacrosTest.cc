/*
 */

#include <iostream>
#include <vector>
#include <base/XDEUtil.h>

#include <exception>

using namespace std;
#include "doctest.h"


SCENARIO("Assert Macro Testing") {
	GIVEN("The basic Test") {
		THEN("True macro") {CHECK_NOTHROW( XDE_ASSERT(true));}
		THEN("True macro") {CHECK_THROWS( XDE_ASSERT(2+3==4));}

	}
}
