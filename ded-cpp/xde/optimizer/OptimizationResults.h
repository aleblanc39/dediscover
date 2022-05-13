#ifndef OPTIMIZATION_RESULTS_H
#define OPTIMIZATION_RESULTS_H

#include <vector>
#include <memory>


class OptimizationResults {
    public:
        OptimizationResults(
          const std::vector<double> &lowerBounds,
          const std::vector<double> &upperBounds,
          const double &optimalValue,
          const std::vector<double> &optimalParamValues, 
          const int &nobs): 
          _lowerBounds(lowerBounds), _upperBounds(upperBounds),
          _optimalValue(optimalValue), _paramValues(optimalParamValues), nbObservations(nobs) {

        }

        OptimizationResults() {
          nbObservations = 0;
        }

        auto getOptimalValue() const {return _optimalValue;}
        const auto &getParamValues() const  {return _paramValues;}
        auto getNbObservations() const {return nbObservations;}

        const auto &getLowerBounds() {return _lowerBounds;}
        const auto &getUpperBounds() {return _upperBounds;}

    private:
        std::vector<double> _lowerBounds;
        std::vector<double> _upperBounds;
        double _optimalValue;
        std::vector<double>  _paramValues;
        int nbObservations;


};


typedef std::shared_ptr<OptimizationResults> OptimizationResultsPtr;
#endif  //OPTIMIZATION_RESULTS_H

