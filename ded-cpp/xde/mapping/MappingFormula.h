#ifndef MAPPINGFORMULA
#define MAPPINGFORMULA
#include <string>
#include <expressions/MuParserExpressionEvaluator.h>
#include <expressions/MemorySynchronizer.h>

//#include <mapping/DataTableMapping.h>

class FormulaException {
   public:
    std::string msg;

    const auto  &getMsg() { return msg; }
    FormulaException(const std::string &s) { msg = s; }
};

class MappingFormula {
   public:
    static std::string validateFormula(const std::vector<std::string> &variableNamesSet, const std::string &expression);

    MappingFormula(const std::vector<std::string> &variables, const std::string &formula, MemorySynchronizerPtr memorySynchronizer);


    double compute() const {return evaluator->evaluate();}

   private:
    MuParserExpressionEvaluatorPtr evaluator;
};

#endif  // MAPPINGFORMULA