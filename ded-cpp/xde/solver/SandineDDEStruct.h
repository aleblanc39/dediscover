
// TODO See if we can create some cache here

struct FnDdes {
    ModelPtr thisModel;
    
    //std::vector<double> ret;
    FnDdes(ModelPtr m) { 
        thisModel = m; 
    }

    // The Sambine code sends a vector y of size n (nbDependentVars) containing the values of y at time t and the computed
    // delays in z. z has length n*nbDelays, with the first n values at the first unique delay expression as defined in the
    // model object, the next n values for the second expression, etc.
    std::vector<double> operator()(const double& t,
                                   const std::vector<double>& y,
                                   const std::vector<double>& z) const {
        auto nvar = y.size();
        auto nDelays = z.size() / y.size();
        TDoubleVector yp;
        TDoubleMatrix mat(nvar, nDelays);
        unsigned ptr = 0;
        for (auto j = 0; j < nDelays; j++) {
            for (auto i = 0; i < nvar; i++) {
                mat(i, j) = z[ptr++];
            }
        }

        thisModel->compute(t, XDEUtil::getTVectorDouble(y), mat, yp);
        std::vector<double> ret(yp.size());
        std::copy(yp.begin(), yp.end(), ret.begin());
        return ret;
    }
};

struct FnHistory {
    ModelPtr thisModel;
    double firstPoint;
    std::vector<double> initValues;
    std::vector<double> ret;

    
    FnHistory(ModelPtr m, const TDoubleVector& initialConditionValues,
              const TDoubleVector& tspan)
        : thisModel(m), firstPoint(tspan(0)) {
        initValues.resize(initialConditionValues.size());
        std::copy(initialConditionValues.begin(), initialConditionValues.end(),
                  initValues.begin());
        ret.resize(initialConditionValues.size());
    }

    std::vector<double> operator()(const double& t) const {
        if (t == firstPoint) {
             return initValues;
        } else {
            return  thisModel->getHistoryAtTime(t);
        }
    }
};

struct FnEvent {
    std::vector<double> operator()(const double& t,
                                   const std::vector<double>& /*y*/,
                                   const std::vector<double>& /*dydt*/,
                                   const std::vector<double>& /*z*/) const {
        return {t};
    }
};

struct FnDelay {
    ModelPtr thisModel;

    FnDelay(ModelPtr m) : thisModel(m) {}
    std::vector<double> operator()(const double& t, const int& nbDelays,
                                   const std::vector<double>&) const {
        return  thisModel->getUniqueTimes(t);
    }
};
