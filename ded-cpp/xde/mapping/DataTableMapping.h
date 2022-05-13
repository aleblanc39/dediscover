#ifndef DATATABLEMAPPING
#define DATATABLEMAPPING


#include <model/XDEModel.h>

#include <vector>

class DataTableMapping;
class ColumnData;


typedef std::shared_ptr<DataTableMapping> DataTableMappingPtr;


class ColumnData {
   public:
    const std::string formula;
    const std::vector<double> values;

    ColumnData(const std::string &formula, const std::vector<double> &values):formula(formula), values(values) {
    }
};



class DataTableMapping {
   public:
    DataTableMapping(const std::vector<double> &timepoints, const std::vector<ColumnData> &columnData);
    DataTableMapping(const std::vector<double> &timepoints, const std::vector<std::string> &formulas, const TDoubleMatrix &data);


    const auto &getTimepoints() {return timepoints;}
    const auto &getColumnData() {return columnData;}

    const auto &dataAsMatrix() {return matrixData;}

   private:
    std::vector<double> timepoints;
    std::vector<ColumnData> columnData;
    TDoubleMatrix matrixData;

};



#endif // DATATABLEMAPPING