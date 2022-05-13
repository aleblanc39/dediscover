#ifndef ESTIMATIONSYMBOLATTRIBUTES_H
#define ESTIMATIONSYMBOLATTRIBUTES_H

#include <parameters/SymbolAttributes.h>

#include <iostream>

/**
 * \brief Class to hold the values required for the estimation.
 *
 * This class will be the template argument for the EstimationParameterSet
 * class.
 *
 */

class EstimationSymbolAttributes : public SymbolAttributes {
   public:
    EstimationSymbolAttributes(std::string name, double value, double minValue,
                               double maxValue, bool estimated)
        : initialValue(value),
          minValue(minValue),
          maxValue(maxValue),
          estimatedFlag(estimated) {
        setName(name);
    }
    EstimationSymbolAttributes();
    virtual ~EstimationSymbolAttributes();

    double getInitialValue() const { return initialValue; }
    double getMinValue() const { return minValue; }
    double getMaxValue() const { return maxValue; }
    bool isEstimated() const { return estimatedFlag; }

    void setEstimated(bool b) { estimatedFlag = b; }
    void setMin(double d) { minValue = d; }
    void setMax(double d) { maxValue = d; }
    void setInitialValue(double d) { initialValue = d; }

   private:
    double initialValue;
    double minValue;
    double maxValue;
    bool estimatedFlag;
};

std::ostream &operator<<(std::ostream &out,
                         const EstimationSymbolAttributes &o);

#endif
