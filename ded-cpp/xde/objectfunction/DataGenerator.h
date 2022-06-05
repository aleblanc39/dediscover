#ifndef DATAGENERATOR_H
#define DATAGENERATOR_H

#include <algorithms/GeneralParameter.h>
#include <base/CBIMDefinitions.h>
#include <base/XDEBlas.h>
#include <model/XDEModel.h>
#include <parameters/EstimationParameterSet.h>
#include <mapping/DataTableMapping.h>

class DataGenerator;

typedef std::shared_ptr<DataGenerator> DataGeneratorPtr;

/**
 * \brief Base for the classes that are used to generate the data used
 * by the objective function.
 *
 * The main idea is to pre-process as much as possible so it will be very fast
 * to send to the objective function all the pairs (observes, computed).
 */

class DataGenerator {
   public:
    /**
     * \brief Structure to handle the time-value pairs of the data
     * points to compute.
     */

    typedef struct {
       public:
        void resizeAll(unsigned u) {
            observed.resize(u);
            computed.resize(u);
            timepoints.resize(u);
        }

        /**
         * \brief Resize structure with the number of timepoints different
         * then the number of data points.
         *
         * This case arises with the discretization methods.
         */

        void resizeTimeAndData(unsigned t, unsigned u) {
            observed.resize(u);
            computed.resize(u);
            timepoints.resize(t);
        }

        unsigned size() { return observed.size(); }
        TDoubleVector timepoints;
        TDoubleVector observed;
        TDoubleVector computed;
    } DoubleVectorPair;

    typedef enum { NONE, LINEAR, QUADRATIC } ScalingMode;

    DataGenerator(ModelPtr model, EstimationParameterSetPtr paramSet, DataTableMappingPtr datatable);

    // Two pre-process methods. One takes 0 to two parameters. The
    // second one takes control parameters as defined in the
    // XDEAlgorithm class to allow more flexibility.
    virtual void preProcess(bool applyLog = false, ScalingMode scaling = NONE);
    virtual void preProcess(const std::vector<GeneralParameterPtr> &params);

    virtual void preprocessSpecific() = 0;
    virtual void preprocessSpecific(
        const std::vector<GeneralParameterPtr> &params) = 0;

    virtual unsigned numObservations() = 0;
    virtual ~DataGenerator();

    // TODO
    // Declare as returning a pointer to a constant vector instead. Since it
    // will be called many times it may be worth the extra effort to reduce the
    // number of times a vector is created/destroyed.
    virtual DoubleVectorPair generateData(const std::vector<double> &parameters) = 0;

    TDoubleVector getFixedData() { return returnedPairs.observed; }
    TDoubleVector getUniqueTimepoints() { return returnedPairs.timepoints; }

    double (DataGenerator::*transformData)(double);
    double no_op(double d) { return d; }

    double __log(double d) { return log10(d); }

    unsigned getNbParameters() { return parameterMapping.size(); }
    unsigned getNbOut() { return returnedPairs.size(); }

   protected:
    ModelPtr model;
    DataTableMappingPtr mapping;

    TDoubleVector initialConditionValues;
    TDoubleVector modelParameterValues;
    std::vector<double *> parameterMapping;

    // Vector that will be returned upon a call to generateData().
    // Will be set to the correct size in the constructor, then reused as
    // needed.

    DoubleVectorPair returnedPairs;
    bool preprocessed;

    // TODO Rename this one. Could mean anything.
    void initialize(bool applyLog = false);
    TDoubleVector multiplier;
    virtual void computeMultiplier(ScalingMode scaling) = 0;

    // std::vector<GeneralParameterPtr> controlParameters;
   private:
};

#endif
