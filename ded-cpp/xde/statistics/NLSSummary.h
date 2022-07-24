#ifndef NLSSUMMARY_H_
#define NLSSUMMARY_H_

#include "XDEConfidenceInterval.h"
#include <algorithms/ParameterValue.h>

class NLSSummary: public XDEConfidenceInterval {

    public:
	NLSSummary(){}
	virtual ~NLSSummary(){}

	ConfidenceIntervalResultPtr evaluate(
    			OptimizationResultsPtr estimatorResult,
				DataTableMappingPtr datatableMapping,
    			EstimationParameterSetPtr parameterSet,
    			ModelPtr model,
				SolverPtr solver, const ParameterValueMap &pvm = ParameterValueMap());


	const TDoubleMatrix &getCovarianceMatrix() const {return covarianceMatrix;}

	static std::string s_getMethodName() {
	    return "Fisher's Information";
	}

	static std::string s_getShortMethodDescription(){return "Fisher's Information (Asymptotic)";}
	static std::string s_getLongMethodDescription() {return "This method generates *approximate* confidence intervals using Fisher's Information method. This method is only valid under assumptions that are unlikely to hold for most real problems, so the computed values should be considered only approximations.";}


	static std::vector<std::string> s_getMethodAttributes() {
        return {};
    }


 protected:
	TDoubleMatrix covarianceMatrix;
	static std::vector<double> performSimulation(const std::vector<double> &params,
				       NLSSummary *nls);



};

typedef std::shared_ptr<NLSSummary> NLSSummaryPtr;

#endif /* NLSSUMMARY_H_ */
