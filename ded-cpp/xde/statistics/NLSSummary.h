#ifndef NLSSUMMARY_H_
#define NLSSUMMARY_H_

#include "XDEConfidenceInterval.h"
#include <boost/assign/list_of.hpp> 


/**
 * \brief Wrapper around the NLSSummary code from mathstat
 *
 * The NLSSummary implements the confidence interval computation using the 
 * Fisher's method.
 */


class NLSSummary: public XDEConfidenceInterval {

    public:
	NLSSummary(){}
	virtual ~NLSSummary(){}

	ConfidenceIntervalResultPtr evaluate(
				const std::vector<GeneralParameterPtr> &controlParams,
    			OptimizationResultsPtr estimatorResult,
				DataTableMappingPtr datatableMapping,
    			EstimationParameterSetPtr parameterSet,
    			ModelPtr model,
				SolverPtr solver);


	const TDoubleMatrix &getCovarianceMatrix() const {return covarianceMatrix;}

	static std::string s_getMethodName() {
	    return "Fisher's Information";
	}

	std::string getMethodName() {return s_getMethodName();}
	std::string getShortMethodDescription(){return s_getShortMethodDescription();}
	std::string getLongMethodDescription() {return s_getLongMethodDescription();}
	static std::string s_getShortMethodDescription(){return "Fisher's Information (Asymptotic)";}
	static std::string s_getLongMethodDescription() {return "This method generates *approximate* confidence intervals using Fisher's Information method. This method is only valid under assumptions that are unlikely to hold for most real problems, so the computed values should be considered only approximations.";}


	static std::vector<std::string> s_getMethodAttributes() {
        return boost::assign::list_of("two-step");
    }


 protected:
	TDoubleMatrix covarianceMatrix;
	static std::vector<double> performSimulation(const std::vector<double> &params,
				       NLSSummary *nls);



};

typedef std::shared_ptr<NLSSummary> NLSSummaryPtr;

#endif /* NLSSUMMARY_H_ */
