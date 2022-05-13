#ifndef XDEOBJECTFUNCTION_H_
#define XDEOBJECTFUNCTION_H_

#include <algorithms/XDEAlgorithm.h>



#include "DataGenerator.h"

class XDEObjectiveFunction;

typedef std::shared_ptr<XDEObjectiveFunction> ObjectiveFunctionPtr;

/**
 * \brief Base class for the objective function.
 *
 * All the method derived from this class will compute an objective
 * function by performing the simulation ona solver object, and
 * comparing the resulting values with the data from a datatable
 * object which may be mapped.
 *
 * Some preprocessing is required to establish the parameters to be
 * estimated and some intialization of the mapping/datatable pair is
 * required.
 *
 * NOTE: The preprocess step must be called from the program using
 * this function. Although it can called as the first step of the
 * objective function computation it would result in a huge waste of
 * time since it is likely that the computation will be performed
 * multiple times without changes to the datatable or the list of
 * estimated parameters.
 */

class XDEObjectiveFunction : public XDEAlgorithm {
   public:
    XDEObjectiveFunction() {}

    XDEObjectiveFunction(DataGeneratorPtr d) : dataGenerator(d) {}

    virtual ~XDEObjectiveFunction();

    double computeFunction(const TDoubleVector &parameters);

    // Given the parameter values, fill up the array fs with
    // (y_0-x_0, y_1-x_2, ..., y_k - x_k)
    // where y is the data from the data table, x the mapped simulation data.
    //
    unsigned computeLSTerms(double *parameters, double *fx);
    unsigned computeNumObservations();

    virtual double estimateResultingObjectSize() { return 0; }

    virtual void preProcess() = 0;
    auto getDataGenerator() { return dataGenerator; }

    void setDataGenerator(const DataGeneratorPtr &p) { dataGenerator = p; }

   protected:
    virtual double performComputation(const TDoubleVector &paras);
    virtual double computeSum(const TDoubleVector &parameters);
    DataGeneratorPtr dataGenerator;
    unsigned nbCalls;

    static std::string getNormalizedParameterName() { return "Normalize data"; }

   private:
};

#endif /*XDEOBJECTFUNCTION_H_*/
