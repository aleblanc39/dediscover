#include <statistics/nls_summary.h>
#include <boost/math/distributions.hpp>
#include <base/XDEException.h>
#include <algorithm>
#include <base/XDEMessage.h>
#include <base/XDEUtil.h>
#include <statistics/MathUtil.h>

void nls_summary(
    const double& alpha,  // significance level
    const std::vector<double>  &_paras_est,  
    const std::vector<double> &data, // From data table
    const double& RSS, 
    std::function <std::vector<double> (const std::vector<double>&) > func,
    std::vector<double>& SE,  // stardard error of estimates
    std::vector<double>& LCB,
    std::vector<double>& UCB,  // lower bound and upper bound of C.I.
    std::vector<double>& tstat,
    std::vector<double>& pvals,  // t-statistics and p-values
    const TDoubleMatrix& V)

{
    // small perturbation
    double EPS = 2.2204460492503131e-16;
    auto paras_est = _paras_est;
    double DELTA = 1.0e-05;
    auto num_paras = paras_est.size();
    auto num_data = data.size();

    if (0.0 >= alpha || alpha >= 1.0) {
        throw XDEException("Value of alpha must be between 0 and 1");
    }
    
    if (RSS < 0) {
        throw XDEException("The value for RSS can not be less than 0.");
    }

    SE.resize(num_paras, 0);
    LCB.resize(num_paras, 0);
    UCB.resize(num_paras, 0);
    tstat.resize(num_paras, 0);
    pvals.resize(num_paras, 0);

    //=======================================================================
    // calculation
    //=======================================================================
    // calculate variance estimate
    double sigma2 = RSS / (double)(num_data - num_paras);
    double sigma = sqrt(sigma2);

    // define the sensitivity matrix and the Fisher information matrix
    TDoubleMatrix SenMat(num_data, num_paras);
    TDoubleMatrix FIM(num_paras, num_paras);

    // model output corresponding to parameter estimates
    // TDoubleVector y(num_data);

    const std::vector<double> y = func(paras_est);

    TDoubleVector yp_minus_y(num_data);
    for (auto i = 0; i < num_paras; i++) {
        double old_value = paras_est[i];

        // add small perturbation
        double deltapara = sqrt(EPS * std::max(DELTA, fabs(paras_est[i])));

        // Scale up delatapara to avoid problem with
        // floating-point loss of precision when adding
        // numbers of vastly different scales
        while (paras_est[i] + deltapara == old_value) {
            deltapara = deltapara * 10;
        }
        paras_est[i] += deltapara;
        const std::vector<double> yp = func(paras_est);

        for (unsigned ptr = 0; ptr < y.size(); ptr++) {
            yp_minus_y[ptr] = (yp[ptr] - y[ptr])/deltapara;
        }
        
        column(SenMat, i) = yp_minus_y;

        // restore parameter estimates
        paras_est[i] = old_value;
    }


    // // calculate the Fisher information matrix
    // if (true == isIdentityMatrix)
         FIM = prod(trans(SenMat), SenMat);
    // else {
    //     FIM = prod(TDoubleMatrix(prod(trans(SenMat), V)), SenMat);
    // }


    // calculate the inverse of FIM
    TDoubleMatrix inverted(FIM.size1(), FIM.size2());
    if (!MathUtil::invertMatrix(FIM, inverted)) {
        // send message
       XDEMessage::log(XDEMessage::WARNING, "nls_summary: Fisher information matrix is "
                                   "singular Calculation of C.I. failed\n");

        
        for (unsigned i = 0; i < num_paras; i++) {
            SE[i] = std::numeric_limits<double>::quiet_NaN();
            LCB[i] = std::numeric_limits<double>::quiet_NaN();
            UCB[i] = std::numeric_limits<double>::quiet_NaN();
            tstat[i] = std::numeric_limits<double>::quiet_NaN();
            pvals[i] = std::numeric_limits<double>::quiet_NaN();
        }

        return;
    }

    boost::math::students_t tdist(num_data - num_paras);
    double tval = boost::math::quantile(tdist, 1.0 - alpha / 2.0);
    double se;  // standard error
    for (unsigned i = 0; i < num_paras; i++) {
        se = sigma * sqrt(inverted(i, i));
        if ( isnan(se) ||  se <= 0.0) {
            SE[i] = 0.0;
            LCB[i] = paras_est[i];
            UCB[i] = paras_est[i];
            tstat[i] = std::numeric_limits<double>::quiet_NaN();
            pvals[i] = 0.0;

            XDEMessage::log(XDEMessage::INFO, 
                "nls_summary: Standard error is zero No C.I., t-stat and "
                "p-value can be calculated\n");

            continue;
        }

        SE[i] = se;
        LCB[i] = paras_est[i] - tval * se;
        UCB[i] = paras_est[i] + tval * se;
        tstat[i] = paras_est[i] / se;
        pvals[i] = 2.0 * boost::math::cdf(tdist, -fabs(tstat[i]));
    }
}
