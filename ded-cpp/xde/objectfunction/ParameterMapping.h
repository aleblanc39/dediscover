#ifndef PARAMETERMAPPING_H
#define PARAMETERMAPPING_H

#include <model/XDEModel.h>
#include <parameters/EstimationParameterSet.h>

#include <vector>


/**
 * \brief Class to handle the mapping between the parameters transmitted by the
 * mathstat estimation code and the parameter used in the simulation code.
 *
 * The optimization code is only aware of the parameters used for estimation, regardless
 * of whether they are model parameters or initial conditions, and are all stored in one 
 * vector. From this vector the solver must assign values to the initial conditions and 
 * model parameters in order to perform the simulation. The only static method in this class
 * returns the mapping between these vectors.
 *
 */

class ParameterMapping {

 public:

    /**
     * \brief Establish the mapping.
     *
     * The input vectors modelParameterValues and initialConditionsValues will
     * be used in the simulation. In the vector parameterMapping we will store
     * pointers to the above two vector to the locations where the parameters
     * are estimated. Therefore the size of parameterMapping will be the total
     * number of estimated parameters. The parameterSet argument is used to
     * determine which parameters are estimated, while the model argument
     * tells us the order of the parameter in modelParameterValues and
     * initialConditionValues. 
     */
    static void initializeMapping(TDoubleVector &modelParameterValues, // Set with values from parameter set
				  TDoubleVector &initialConditionValues, // Set with values from parameter set
				  std::vector<double *> &parameterMapping, // Vector which will be set.
				  EstimationParameterSetPtr parameterSet,
				  ModelPtr model);


 private:
    
    static void initializeMapping(std::vector<double *> &parameterMapping, 
				  TDoubleVector &parameterValues,
				  const std::vector<std::string> &paramNames,					 
				  EstimationParameterSetPtr parameterSet);

};


#endif
