#ifndef NONLINEAR_LEAST_SQUARES_SUMMARY_H_
#define NONLINEAR_LEAST_SQUARES_SUMMARY_H_
#include <vector>
#include <functional>
#include <base/XDEBlas.h>


//====================================================================
//
// Original code by Hongyu Miao
//
// This function summarizes the nonlinear least squares fitting, just
// like the summary function in R. This function calculate confidence 
// intervals, t-statistics, and p-values. 
//
// This function produces system output according to specified parameter values
// and map the output to the same sequence of data
//====================================================================
void  nls_summary(
	const double& alpha,  // significance level
    const std::vector<double>  &_paras_est,  
    const std::vector<double> &data, 
    const double& RSS,
    std::function<std::vector<double>(const std::vector<double>&)> func,
    std::vector<double>& SE,  // stardard error of estimates
    std::vector<double>& LCB,
    std::vector<double>& UCB,  // lower bound and upper bound of C.I.
    std::vector<double>& tstat,
    std::vector<double>& pvals,  // t-statistics and p-values
    const TDoubleMatrix& V = TDoubleMatrix(0, 0));

#endif // end of header file
