#include "MappingInfoSet.h"

#include <base/XDEException.h>
#include <base/XDEUtil.h>

#include <boost/bind.hpp>
#include <iostream>
#include <iterator>
#include <mapping/NewWorkingData.h>
using namespace std;

MappingInfoSet::~MappingInfoSet() { }

MappingInfoSet::MappingInfoSet(DataTableMappingPtr dataTableMapping,
                               const std::vector<std::string> &varNames,
                               MemorySynchronizerPtr memSynchronizer)
    : modelVariableNames(varNames), memorySynchronizer(memSynchronizer) {
        createMappingInfo(dataTableMapping);
}

void MappingInfoSet::createMappingInfo(DataTableMappingPtr dataTableMapping) {
    for (const ColumnData &cd : dataTableMapping->getColumnData()) {
        currentMappingInfoSet.push_back(MappingFormula(modelVariableNames, cd.formula, memorySynchronizer));
    }
}




TDoubleMatrix MappingInfoSet::compute(const TDoubleMatrix &matrixData) {
    int nRows = matrixData.size1();
    int nCols = (int)currentMappingInfoSet.size();
    unsigned nVars = (int)modelVariableNames.size();
    XDE_ASSERT(matrixData.size2() == nVars);
    XDE_ASSERT(nRows > 0);

    TDoubleMatrix retMatrix(nRows, nCols);
    TDoubleVector rowData(nVars);
    TDoubleVector computedRowData;
    for (int i = 0; i < nRows; i++) {
        for (unsigned j = 0; j < nVars; j++) rowData[j] = matrixData(i, j);
        computedRowData = compute(rowData);
        for (unsigned j = 0; j < nVars; j++)
            retMatrix(i, j) = computedRowData[j];
    }
    return retMatrix;
}


TDoubleVector MappingInfoSet::compute(const TDoubleVector &rowData) {
    setVariableValues(rowData);
    TDoubleVector ret(currentMappingInfoSet.size());
    for (unsigned i = 0; i < ret.size(); i++) {
        ret[i] = currentMappingInfoSet[i].compute();
    }
    return ret;
}



void MappingInfoSet::setVariableValues(const TDoubleVector &values) {
    for (unsigned i = 0; i < values.size(); i++)
        memorySynchronizer->updateMemory(modelVariableNames[i], values[i]);
}



