#ifndef MODELPARAMETER_H
#define MODELPARAMETER_H


#include "ModelSymbol.h"

/**
 * \brief Class to handle model parameters from the model.
 */

class ModelParameter: public ModelSymbol {
 public:

    ModelParameter(std::string name, unsigned lineNO = 100000);

    ~ModelParameter();


};

typedef std::shared_ptr<ModelParameter> ModelParameterPtr;


#endif
