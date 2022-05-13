#ifndef XDERESULTOBJECT_H
#define XDERESULTOBJECT_H

#include <vector>
#include <base/AlgorithmResult.h>
#include <base/XDEBlas.h>


class SolverResult;

typedef std::shared_ptr<SolverResult> SolverResultPtr;

/**
 * \brief Class to handle the results of the simulation.
 *
  */

class SolverResult: public AlgorithmResult {

 public:

    SolverResult();

    SolverResult(const std::vector<std::string> &varNames,
		 const TDoubleVector &timepoints,
		 const TDoubleMatrix &data): variableNames(varNames),
	timepoints(timepoints), data(data) {}

    void setData(const TDoubleMatrix &newData);
    void setTimepoints(const TDoubleVector &newTimepoints);
    void setVariableNames(const std::vector<std::string> &newVarNames);

    unsigned getNbTimepoints() {return timepoints.size();}
    unsigned getNbVariables() {return variableNames.size();}

    TDoubleVector getTimepoints() {return timepoints;}
    TDoubleMatrix getData() {return data;}
    std::vector<std::string> getVariableNames() {return variableNames;}



 private:
    std::vector<std::string> variableNames;
    TDoubleVector timepoints;
    TDoubleMatrix data;
    std::string timeVariableName;

};

#endif
