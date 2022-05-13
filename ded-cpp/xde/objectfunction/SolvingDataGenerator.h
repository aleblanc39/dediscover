#ifndef SOLVINGDATAGENERATOR_H
#define SOLVINGDATAGENERATOR_H

#include <mapping/DataTableMapping.h>
#include <mapping/NewWorkingData.h>
#include <solver/XDESolverWrapper.h>

#include "DataGenerator.h"

/**
 * \brief DataGenerator used when the objective function will optimize with a
 * solver.
 *
 * Each time that generateData is called the solver will be invoked. A
 * mapping must be established in the object creation to establish the
 * correspondance between the values in the datatable and the results
 * of the simulation.
 *
 *
 */

class SolvingDataGenerator : public DataGenerator {
   public:
    SolvingDataGenerator(ModelPtr model, SolverPtr solver,
                         EstimationParameterSetPtr e,
                         DataTableMappingPtr mapping, bool applyLog = false,
                         ScalingMode scaling = NONE);
    virtual ~SolvingDataGenerator();

    void preprocessSpecific(const std::vector<GeneralParameterPtr> &params);
    void preprocessSpecific();

    unsigned numObservations() { return returnedPairs.size(); }

    DoubleVectorPair generateData(const std::vector<double> &parameters);

   protected:
    void computeMultiplier(ScalingMode scaling);

   private:
    TDoubleVector tspan;
    TDoubleVector uniqueTimepoints;

    unsigned firstTimepoint;
    unsigned lastTimepoint;

    SolverPtr solver;
    NewWorkingDataPtr workingData;
};

#endif
