#include "XDECSVFile.h"

using namespace std;

#include <base/CBIMDefinitions.h>
#include <base/XDEException.h>
#include <base/XDEUtil.h>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

std::string XDECSVFile::getDefaultTimeColumnName() { return "Time"; }

XDECSVFile::XDECSVFile() {}

XDECSVFile::~XDECSVFile() {}

vector<string> XDECSVFile::getAllHeaders() {
    vector<string> ret(getNbVariables() + 1);
    ret[0] = getTimeColumnName();
    copy(variableNames.begin(), variableNames.end(), ret.begin() + 1);
    return ret;
}

void XDECSVFile::setData(const TDoubleMatrix &newData) { data = newData; }

void XDECSVFile::setTimepoints(const std::vector<double> &newTimepoints) {
    timepoints = newTimepoints;
}

void XDECSVFile::setVariableNames(const std::vector<std::string> &newVarNames) {
    variableNames = newVarNames;
}

/**
 * Will keep it simple for now. Operation is performed if either:
 * - Current object is completely empty, or
 * - Variables are the same, or
 * - Timepoints are the same, no duplicate variable names.
 */
void XDECSVFile::addData(const std::vector<std::string> &_variableNames,
                         const std::vector<double> &_timepoints,
                         const TDoubleMatrix &_data) {
    if (variableNames.size() == 0 || timepoints.size() == 0) {
        setData(_data);
        setTimepoints(_timepoints);
        setVariableNames(_variableNames);
    }

    // There is already existing data
    // First check if variable names are the same

    unsigned size1 = _data.size1();
    unsigned size2 = _data.size2();

    if (_variableNames.size() == variableNames.size() &&
        equal(variableNames.begin(), variableNames.end(),
              _variableNames.begin())) {
        // Variable names are identical. Just append timepoints. OK to have
        // duplicates.
        unsigned currentSize = timepoints.size();
        timepoints.resize(currentSize + _timepoints.size());
        copy(_timepoints.begin(), _timepoints.end(),
             timepoints.begin() + currentSize);
        // Copy the data at the bottom

        // Won't make any assumption on how the data is laid out in the matrix
        // class
        unsigned baseRow = data.size1();
        data.resize(data.size1() + _data.size1(), data.size2());
        for (unsigned i = 0; i < size1; i++) {
            for (unsigned j = 0; j < size2; j++) data(baseRow, j) = _data(i, j);
            baseRow++;
        }
    } else if (timepoints.size() == _timepoints.size() &&
               equal(timepoints.begin(), timepoints.end(), _timepoints.begin(),
                     ptr_fun(double_equals))) {
        // In this case do not allow for duplicate variable names.
        // Probably a shorter way.
        for (const auto &s : variableNames) {
            if (find(_variableNames.begin(), _variableNames.end(), s) !=
                _variableNames.end())
                throw XDEException(
                    "Error in adding data to XDECSVFile: Variable names are "
                    "not disjoint or equal.\n");
        }
        variableNames.insert(variableNames.end(), _variableNames.begin(),
                             _variableNames.end());
        unsigned baseCol = data.size2();
        data.resize(data.size1(), data.size2() + _data.size2());
        for (unsigned j = 0; j < size2; j++) {
            for (unsigned i = 0; i < size1; i++) data(i, baseCol) = _data(i, j);
            baseCol++;
        }
    } else {
        throw XDEException(
            "Error when adding data to CSV file: incompatible objects.\n");
    }
}

void XDECSVFile::addData(XDECSVFile *otherFile) {
    if (otherFile != NULL)
        addData(otherFile->getVariableNames(), otherFile->getTimepoints(),
                otherFile->getData());
}
