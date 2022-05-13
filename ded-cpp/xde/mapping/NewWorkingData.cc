
#include <mapping/NewWorkingData.h>
#include <mapping/ModelMappingInfoSet.h>
#include <vector>

using namespace std;

NewWorkingData::NewWorkingData(ModelPtr model, DataTableMappingPtr dataTableMapping):
     dataTableMapping(dataTableMapping) {
        mappingInfoSet.reset(new ModelMappingInfoSet(model, dataTableMapping));
        initializeData();
}

NewWorkingData::NewWorkingData(const vector<string> &varNames, MemorySynchronizerPtr memorySynchronizer, DataTableMappingPtr dataTableMapping):
     dataTableMapping(dataTableMapping) {
        mappingInfoSet.reset(new MappingInfoSet(dataTableMapping, varNames, memorySynchronizer));
        initializeData();
}


// TODO This is huge
// TODO Remove startTime end
void NewWorkingData::initializeData() {

    auto data = dataTableMapping -> dataAsMatrix();

    // Preprocess the timepoints so we can access the rows in increasing order
    // of time and also remove the timepoints that are NAN or +/- inf

    auto originalTimepoints = getTVector(dataTableMapping -> getTimepoints());
    auto nbTimepoints = originalTimepoints.size();

    tableRowPtr.reserve(nbTimepoints);
    for (unsigned i = 0; i < nbTimepoints; i++) {
        // TODO may not be required anymore if we expect caller to send good data.
        auto v = (originalTimepoints[i] == -XDEUtil::XDE_INF ||
                    ISNAN(originalTimepoints[i]))
                       ? XDEUtil::XDE_INF
                       : originalTimepoints[i];
            tableRowPtr.push_back(TableRowPtr(v, i));
    }

    // Now tableRowPtr has pointers to rows for which the timepoint is within
    // the range. Could be duplicate timepoints.

    if (tableRowPtr.size() < 2) {
        XDEMessage::log(XDEMessage::WARNING,
                        "There are no usable timepoints in the datatable.");
        return;  // No point processing data from here on
    }

    // TODO use boost lambda here instead of creating a function
    // Set the values for the timepoints and uniqueTimepoints vectors.
    // Will remove the INF entries at the end of the timepoints array.

    sort(tableRowPtr.begin(), tableRowPtr.end(),
         NewWorkingData::compareRowEntries);
    uniqueTimepoints.reserve(nbTimepoints);

    // timepoints will contain the timepoints in increasing order. Can
    // be used to set tspan in the solver.
    // uniqueTimepoints contains the same timepoints without the
    // duplicates. In both cases the inf are removed.

    uniqueTimepoints.push_back(tableRowPtr[0].timepoint);

    for (unsigned i = 1; i < tableRowPtr.size(); i++) {
        if (!double_equals(tableRowPtr[i].timepoint, uniqueTimepoints.back()))
            uniqueTimepoints.push_back(tableRowPtr[i].timepoint);
    }

    useFirstRow = true; // TODO Remove? Will always use first row.

    // If startTime not part of dataTable, add it at the beginning
    // if (!double_equals(startTime, uniqueTimepoints[0])) {
    //     uniqueTimepoints.insert(uniqueTimepoints.begin(), startTime);
    //     useFirstRow = false;  // First row in simulation will not be used later.
    // } else {
    //     useFirstRow = true;
    // }

    // Create the dataMap, the linear data and timepoints, and the

    auto nbValidTimepoints = tableRowPtr.size();
    auto nbCols = data.size2();

    dataMap.reserve(nbValidTimepoints);
    linearData.reserve(nbValidTimepoints * nbCols);

    // TODO: Make process of excluding data more flexible. Right now
    // exclude NAN's. May want option to exclude 0's (if used for
    // least squares.

    // The loop will process the matrix in inreasing order of
    // timepoint.

    // dataMap will map an entry in linearData to a matrix that
    // contains the same number of columns as the data and the same
    // unique timepoints in increasing order. Will be used, for
    // instance, to map the results from the simulation, where the
    // timepoints will be unique.

    vector<double>::iterator iter;
    for (auto &trp: tableRowPtr) {
        // Row will be the row value in the original matrix.
        auto row = trp.row;

        // Locate where in the uniqueTimepoints vector the timepoint assigned to
        // this entry is located.
        iter = find_if(uniqueTimepoints.begin(), uniqueTimepoints.end(),
                       bind2nd(ptr_fun(double_equals), trp.timepoint));

        XDE_ASSERT(iter != uniqueTimepoints.end());
        auto indx = iter - uniqueTimepoints.begin();
        // Will be used when mapping simulation results.

        for (unsigned j = 0; j < nbCols; j++) {
            if (!ISNAN(data(row, j))) {
                dataMap.push_back(pair<unsigned, unsigned>(indx, j));
                linearData.push_back(data(row, j));
            }
        }
    }
}



TDoubleVector NewWorkingData::getUniqueTimepoints() {
    return XDEUtil::getTVectorDouble(uniqueTimepoints);
}

TDoubleVector NewWorkingData::getLinearData() {
    return XDEUtil::getTVectorDouble(linearData);
}

/**
 * Will compute the mapping for the result of a simulation. Will
 * assume that dataToMap contains one row per unique timepoint.
 * PROBLEM HERE: Forcing dataToMap to have exact same format as datatable.
 * Creates issues when not using all timepoints in datatable, or when data
 * in datatable does not start at t0.
 */
TDoubleMatrix NewWorkingData::getMappedData(const TDoubleMatrix &dataToMap) {
    XDE_ASSERT(dataToMap.size1() == uniqueTimepoints.size());
    TDoubleVector vec = XDEUtil::getTVectorDouble(uniqueTimepoints);


    return std::dynamic_pointer_cast<ModelMappingInfoSet>(mappingInfoSet) 
            -> compute(dataToMap, vec);
}

/**
 * Will compute the mapping for the result of a simulation, and then
 * linearize the output.
 * Have the returned value as an argument in order to speed up the process.
 *
 */

void NewWorkingData::getLinearMappedData(const TDoubleMatrix &dataToMap,
                                      TDoubleVector &linearized) {
    TDoubleMatrix mapped = getMappedData(dataToMap);
    linearized.resize(dataMap.size());
    TDoubleVector::iterator iter = linearized.begin();
    for (LinearToMatrixMapping &m : dataMap) {
        *iter++ = mapped(m.first, m.second);
    }
}

bool NewWorkingData::compareRowEntries(const TableRowPtr &a,
                                    const TableRowPtr &b) {
    return a.timepoint < b.timepoint;
}

