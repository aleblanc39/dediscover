/**
 * 
 */

#include <mapping/DataTableMapping.h>
#include <mapping/MappingInfoSet.h>
#include <model/XDEModel.h>
class NewWorkingData;
typedef std::shared_ptr<NewWorkingData> NewWorkingDataPtr;


class NewWorkingData {
   public:
    NewWorkingData(ModelPtr model, DataTableMappingPtr dataTableMapping);

    NewWorkingData(const std::vector<std::string> &varNames, MemorySynchronizerPtr memorySynchronizer, DataTableMappingPtr datatableMapping);

   class TableRowPtr {
       public:
        TableRowPtr(double t, unsigned r) : timepoint(t), row(r) {}
        double timepoint;
        unsigned row;
    };

    typedef std::pair<unsigned, unsigned> LinearToMatrixMapping;
    typedef std::vector<LinearToMatrixMapping> LinearToMatrixMap;

    
    virtual ~NewWorkingData(){};

    TDoubleVector getUniqueTimepoints();
    TDoubleVector getLinearData();
    TDoubleMatrix getMappedData(const TDoubleMatrix &dataToMap);
    void getLinearMappedData(const TDoubleMatrix &dataToMap,
                             TDoubleVector &linearized);

    LinearToMatrixMap getDataMap() { return dataMap; }

   private:


    std::vector<TableRowPtr>  tableRowPtr;
    DataTableMappingPtr dataTableMapping;
    LinearToMatrixMap  dataMap;
    void initializeData();

    std::vector<double>uniqueTimepoints;
    std::vector<double> linearData;
    bool useFirstRow;
    MappingInfoSetPtr mappingInfoSet;

    // TODO replace with lambda
    static bool compareRowEntries(const TableRowPtr &a, const TableRowPtr &b);
};