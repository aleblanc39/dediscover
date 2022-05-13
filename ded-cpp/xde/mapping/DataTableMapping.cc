#include <mapping/DataTableMapping.h>
#include <iostream>

using namespace std;

// TODO AL 2021: Doesn't look like it's used now. Keep anyway -- looks like it could be used.
DataTableMapping::DataTableMapping(const std::vector<double> &timepoints, const std::vector<ColumnData> &columnData)
    :timepoints(timepoints), columnData(columnData) {
    XDE_ASSERT(columnData.size() > 0);
    XDE_ASSERT(timepoints.size() == columnData[0].values.size())

    matrixData.resize(timepoints.size(), columnData.size(), false);


    for (unsigned i = 0; i < columnData.size(); i++) {

        for (unsigned j = 0; j < timepoints.size(); j++) {
            matrixData(j, i) = columnData[i].values[j];
        }
    }
}


 DataTableMapping::DataTableMapping(const std::vector<double> &timepoints, 
                const std::vector<string> &formulas, 
                const TDoubleMatrix &data): timepoints(timepoints) {

        XDE_ASSERT(timepoints.size() == data.size1());
        XDE_ASSERT(formulas.size() == data.size2());

        matrixData.resize(timepoints.size(), formulas.size(), false);

        for (unsigned i = 0; i < formulas.size(); i++) {
            std::vector<double> coldata;

            for (unsigned j = 0; j < timepoints.size(); j++) {
                // Probably more efficient way. No desire to debug mathstat code again.
                matrixData(j, i) = data(j, i);
                coldata.push_back(matrixData(j, i));
            }
            columnData.push_back(ColumnData(formulas[i], coldata));

        }
 }
