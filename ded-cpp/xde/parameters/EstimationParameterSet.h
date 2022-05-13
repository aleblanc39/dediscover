#ifndef ESTIMATION_PARAMETER_SET_H
#define ESTIMATION_PARAMETER_SET_H

#include <parameters/ParameterSet.h>
#include <parameters/EstimationSymbolAttributes.h>


/**
 * \brief Class to handle the set of parameters used for 
 * parameter estimation.
 */

class EstimationParameterSet: public ParameterSet<EstimationSymbolAttributes> {

 public:
    EstimationParameterSet(){}
    ~EstimationParameterSet(){}

    auto nbEstimatedParameters(){return countParameters(&EstimationSymbolAttributes::isEstimated);}
    auto isEstimated(std::string s){return (getParameter(s).isEstimated());}


};

typedef std::shared_ptr <EstimationParameterSet> EstimationParameterSetPtr;

#endif // ESTIMATION_PARAMETER_SET_H
