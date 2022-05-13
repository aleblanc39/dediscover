#ifndef DEPENDENTVARIABLE_H_
#define DEPENDENTVARIABLE_H_

#include "ModelSymbol.h"

#include "ProcessedModelExpression.h"

#include <base/XDEUtil.h>



class DependentVariable;

typedef std::shared_ptr<DependentVariable> DependentVariablePtr;

/**
 * \brief Handle dependent variables from the model.
 */

class DependentVariable: public ModelSymbol {
public:
    DependentVariable(std::string name);
    DependentVariable(std::string name, unsigned lineNO, int OccurenceIndex);
    virtual ~DependentVariable();


    ProcessedModelExpressionPtr getDerivativeExpression() {
	return derivativeExpression;
    }

    ProcessedModelExpressionPtr getInitialConditionExpression() {
	return initialConditionExpression;
    }

    void assignDerivative(ProcessedModelExpressionPtr s);
    void assignInitialCondition(ProcessedModelExpressionPtr s);


 private:
    ProcessedModelExpressionPtr derivativeExpression;
    ProcessedModelExpressionPtr initialConditionExpression;
};

#endif /* DEPENDENTVARIABLE_H_ */
