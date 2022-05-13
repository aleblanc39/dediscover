#include "ExpressionSetEvaluator.h"

#include <base/XDEUtil.h>
#include <expressions/MemoryBankSynchronizer.h>

#include <boost/algorithm/string.hpp>

using namespace std;

ExpressionSetEvaluator::ExpressionSetEvaluator() { equationsChanged = true; }

/**
 * Constructor with a memory synchronizer.
 * TODO: Check that the type is indeed a MemoryBankSynchronizer
 */

ExpressionSetEvaluator::ExpressionSetEvaluator(
    MemorySynchronizerPtr memSynchronizer)
    : memorySynchronizer(memSynchronizer) {
    equationsChanged = true;
}

ExpressionSetEvaluator::~ExpressionSetEvaluator() {}

// The order is important for the addExpression(s) routines, as
// the expression will be evaluated in the given order.
void ExpressionSetEvaluator::addExpression(const std::string &expr) {
    equationsChanged = true;
    expressions.push_back(expr);
}

void ExpressionSetEvaluator::addExpressions(
    const std::vector<std::string> &newExprs) {
    equationsChanged = true;
    int currentSize = expressions.size();
    expressions.resize(expressions.size() + newExprs.size());
    copy(newExprs.begin(), newExprs.end(), expressions.begin() + currentSize);
}

void ExpressionSetEvaluator::setExpression(const std::string &expr) {
    equationsChanged = true;
    expressions.clear();
    expressions.push_back(expr);
}

void ExpressionSetEvaluator::setExpressions(
    const std::vector<std::string> &newExprs) {
    equationsChanged = true;
    expressions.assign(newExprs.begin(), newExprs.end());
}

double ExpressionSetEvaluator::compute() {
    if (expressions.size() == 0) return 0;

    if (equationsChanged) updateEquations();
    return  expressionEvaluator->evaluate();
}

// Private methods
void ExpressionSetEvaluator::updateEquations() {
    XDE_ASSERT(memorySynchronizer != NULL);
    equationsChanged = false;
    string equations = prepareEquations();
    //std::cerr << "New equations: " << equations << std::endl;
    expressionEvaluator.reset(
        new MuParserExpressionEvaluator(equations, memorySynchronizer));
}

string ExpressionSetEvaluator::prepareEquations() {
    string ret("");
    for (auto &x : expressions) {
        // Assume that there is only an EOL.
        if (ret.size() > 0) ret.append(",");
        boost::erase_all(x, "\r");
        boost::erase_all(x, "\n");
        ret.append(x);
    }
    return ret;
}
