#include "ModelMappingInfoSet.h"
using namespace std;

ModelMappingInfoSet::ModelMappingInfoSet(ModelPtr model,
                                         DataTableMappingPtr datatableMapping)
    : model(model), MappingInfoSet(datatableMapping, model->getAllVariables(),
                     model->getMemorySynchronizer()) {}


/**
  Given matrixData that contain the simulation result, compute the mapping at
  each of the timepoints given by timepts. Hope the order is right...
*/
TDoubleMatrix ModelMappingInfoSet::compute(const TDoubleMatrix &matrixData,
                                      const TDoubleVector &timepts) {

    auto nRows = matrixData.size1();
    auto nCols = matrixData.size2();
    auto nmappings = currentMappingInfoSet.size();

    XDE_ASSERT(nRows > 0);
    TDoubleMatrix retMatrix(nRows, nmappings);
    TDoubleVector rowData(nCols);
    for (auto i = 0; i < nRows; i++) {
        for (auto j = 0; j < nCols; j++) rowData[j] = matrixData(i, j);
        TDoubleVector newRow = compute(rowData, timepts[i]);
        XDE_ASSERT(newRow.size() == retMatrix.size2());
        for (auto j = 0; j < nmappings; j++) retMatrix(i, j) = newRow[j];
    }
    return retMatrix;
}


TDoubleVector ModelMappingInfoSet::compute(const TDoubleVector &rowData, int timepoint) {
    XDE_ASSERT(model != NULL);
    XDE_ASSERT((int)rowData.size() == model->getDependentVariables().size());

    // TODO AL 2021: This is risky -- this setTimeVariableValue is easy to forget, 
    // maybe too much information required form caller. Should find a better way
    // to ensure the timepoint is right. 
    // A more standard way to assign time variable before any computation.

    // But it may be used in the mapping. Maybe a reference to mem synchronizer should be
    // added to mapping info

    // Maybe should redesign the handling of the mapping from scratch.


    model-> setDependentVariableValues(rowData);
    model-> setTimeVariableValue(timepoint);
    model-> computeMacros(timepoint);

    TDoubleVector ret(currentMappingInfoSet.size());
    TDoubleVector::iterator it = ret.begin();
    for (auto &mip : currentMappingInfoSet) {
        (*it++) = (mip.compute());
    }
    return ret;
}

