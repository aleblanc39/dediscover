#ifndef MAPPINGINFOSET_H
#define MAPPINGINFOSET_H

#include <base/XDEBlas.h>

#include <expressions/MemorySynchronizer.h>
#include <mapping/MappingFormula.h>


#include <set>
#include <string>
#include <vector>

class MappingInfoSet;
typedef std::shared_ptr<MappingInfoSet> MappingInfoSetPtr;

class MappingInfoSet {
   public:
    virtual ~MappingInfoSet();

    MappingInfoSet(DataTableMappingPtr dataTableMapping,
                   const std::vector<std::string> &varNames,
                   MemorySynchronizerPtr memorySynchronizer);

    TDoubleMatrix getMappedData();

    TDoubleVector compute(const TDoubleVector &rowData);     
    TDoubleMatrix compute(const TDoubleMatrix &matrixData);  
    

    TDoubleMatrix compute(const TDoubleMatrix &matrixData1,
                          const TDoubleVector &vectorData,
                          const TDoubleMatrix &matrixData2,
                          const TDoubleVector &lastValue);


    
    /**
     * Compute the equation with the input broken down.
     *
     * Compute the equations where the input consists of three vectors
     * of double and one double value. Coincidently, this is just what
     * is needed when the formula can consist of dependent variables,
     * model parameters, and the time.
     *
     * TODO AL2020 Still needed?
     */
    TDoubleVector compute(const TDoubleVector &rowData1,
                          const TDoubleVector &rowData2,
                          const TDoubleVector &rowData3,
                          const double &lastValue);

protected:
    std::vector<MappingFormula> currentMappingInfoSet;
    std::vector<std::string> modelVariableNames;
    void createMappingInfo(DataTableMappingPtr dataTableMapping);

   private:
    // TODO define a value for non assigned block
    MemorySynchronizerPtr memorySynchronizer;


    void createFullVector(TDoubleVector &fullVector,
                          const TDoubleVector &rowData1,
                          const TDoubleVector &rowData2,
                          const TDoubleVector &rowData3,
                          const double &lastValue);

    void createFullMatrix(TDoubleMatrix &fullMatrix,
                          const TDoubleMatrix &matrixData1,
                          const TDoubleVector &vectorData,
                          const TDoubleMatrix &matrixData2,
                          const TDoubleVector &lastValue);

    void setVariableValues(const TDoubleVector &values);
};

#endif
