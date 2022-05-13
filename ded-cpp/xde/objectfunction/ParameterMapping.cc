#include <objectfunction/ParameterMapping.h>

#include <boost/bind.hpp>
#include <base/XDEUtil.h>
using namespace std;



/**
 * Will create a mapping between the positions of the parameters used
 * in performSimulation which is sent by the mathstat optimization
 * code and the values expected by the mathstat simulation code.
 *
 * 
 *
 */


// TODO probably does not need the estimatedParameters argument, since it can be obtained from parameterSet.
void ParameterMapping::initializeMapping(TDoubleVector &modelParameterValues,
					 TDoubleVector &initialConditionValues,
					 vector<double *> &parameterMapping,
					 EstimationParameterSetPtr parameterSet,
					 ModelPtr model) {

    vector<EstimationSymbolAttributes> estimatedParameters = parameterSet -> getParameters(&EstimationSymbolAttributes::isEstimated);


    parameterMapping.resize(estimatedParameters.size());
    vector<string> parameterNames = getAllNames(model -> getModelParameters());
    initializeMapping(parameterMapping, modelParameterValues, parameterNames, parameterSet);

    parameterNames = getAllNames(model -> getDependentVariables());
    initializeMapping(parameterMapping, initialConditionValues, parameterNames, parameterSet);
}

/**
 * This method will be called for the model parameters and the initial
 * conditions. The first and third arguments will be modified.
 */

void ParameterMapping::initializeMapping(vector<double *> &parameterMapping, 
					 TDoubleVector &parameterValues,
					 const vector<string> &paramNames,					 
					 EstimationParameterSetPtr parameterSet) {

    parameterValues.resize(paramNames.size());
    unsigned ptr = 0;

    vector<EstimationSymbolAttributes> estimatedParameters = parameterSet -> getParameters(&EstimationSymbolAttributes::isEstimated);


    for (const string s : paramNames) {
	EstimationSymbolAttributes attr = parameterSet->getParameter(s);
	parameterValues[ptr] = attr.getInitialValue();
	// Check if this parameter is estimated. If so establish the mapping.
	vector<EstimationSymbolAttributes>::const_iterator it = 
	    find_if(estimatedParameters.begin(), 
		    estimatedParameters.end(), 
		    boost::bind(&EstimationSymbolAttributes::getName, _1) == s);

	if (it != estimatedParameters.end()) {
	    // Need to find the index where this parameter is in the list of 
	    // estimated parameters, since the optimizer will
	    // use them in that order.
	    parameterMapping[it - estimatedParameters.begin()] =  &(parameterValues.data()[ptr]);
	}
	ptr++;
    }
}


