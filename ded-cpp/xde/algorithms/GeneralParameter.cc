#include "GeneralParameter.h"

/**
 * This constructor is invoked before the constructor of the concrete class,
 * and so unless the concrete class redefines the type, the type will be set
 * to unknown
 */

GeneralParameter::GeneralParameter() {
	name = "UNDEFINED";
	description = "UNDEFINED";
	
	hasMin = false;
	hasMax = false;
	assignedValidValue = false;
}

GeneralParameter::~GeneralParameter(){}

