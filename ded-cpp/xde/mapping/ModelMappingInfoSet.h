#ifndef MODELMAPPINGINFOSET_H
#define MODELMAPPINGINFOSET_H
#include <memory>
#include "MappingInfoSet.h"

#include <model/XDEModel.h>

class ModelMappingInfoSet;
typedef std::shared_ptr<ModelMappingInfoSet> ModelMappingInfoSetPtr;


/**
 * \brief Extension to the MappingInfoSet class using information from the
 * XDEModel object.
 *
 * It allows to extract all the information about the variables used in the
 * model without having to explicitely assign them from outside. In addition,
 * it allows the possibility to use
 * macro containing delayed variable in the mapping.
 *
 * Since the XDEModel already uses a MemorySynchronizer object it will
 * be used, as opposed to one that must be provided to the base class.

 */ 
class ModelMappingInfoSet: public MappingInfoSet {

 public:
    //ModelMappingInfoSet();
    //ModelMappingInfoSet(ModelPtr model);
    ModelMappingInfoSet(ModelPtr model, DataTableMappingPtr datatableMapping);
    ~ModelMappingInfoSet(){};

    ModelPtr getModel(){return model;}

    TDoubleVector compute(const TDoubleVector &rowData,
			  int timept);

    TDoubleMatrix compute(const TDoubleMatrix &matrixData,
			  const TDoubleVector &timepts);


    // AL2020: not used?
    // void setMacroValues(const TDoubleVector &macroValues);

 private:
    bool variableNamesValid;
    ModelPtr model;
    //void assignVariableNames();
    int assignVariablesToMappings();

};



#endif
