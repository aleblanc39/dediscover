#ifndef EXPRESSIONSETEVALUATOR_H
#define EXPRESSIONSETEVALUATOR_H



#include <vector>
#include <string>
#include <map>
#include <memory>


#include "MuParserExpressionEvaluator.h"
#include "MemorySynchronizer.h"


/**
 * \brief Perform computation on a set of equations.
 *
 * This class will handle a set of expression. It will create a single
 * MuParser (through a MuParserExpressionEvaluator) object from all
 * the equations and execute them at once when required.  The order in
 * which the equations are assigned is important.
 */

class ExpressionSetEvaluator {

    typedef std::map<std::string, std::string> map_t;
    typedef std::pair<double *, double *> dblptr_t;

 public:
    ExpressionSetEvaluator();

    ExpressionSetEvaluator(MemorySynchronizerPtr memSynchronizer);
    ~ExpressionSetEvaluator();


    /** Add an expression. The order in which they are added is important. */
    void addExpression(const std::string &expr);
    void addExpressions(const std::vector<std::string> &expressions);

    void setExpression(const std::string &expr);
    void setExpressions(const std::vector<std::string> &newExpr);

    double compute();

 private:
    MuParserExpressionEvaluatorPtr expressionEvaluator;
    MemorySynchronizerPtr memorySynchronizer;
    bool equationsChanged;

    std::vector<std::string> expressions;

    void updateEquations();

    std::string prepareEquations();


};

typedef std::shared_ptr<ExpressionSetEvaluator> ExpressionSetEvaluatorPtr;
#endif
