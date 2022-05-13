#ifndef MAPPINGFORMULA
#define MAPPINGFORMULA

/**
 */

#include <mapping/DataTableMapping.h>
#include <model/XDEModel.h>

class FormulaException {
   public:
    std::string msg;

    const auto  &getMsg() { return msg; }
    FormulaException(const std::string &s) { msg = s; }
};

class MappingFormula {
   public:
    static std::string validateFormula(ModelPtr model,
                                       const std::string &formula);
    static std::string validateFormula(const std::vector<std::string> &variableNamesSet, const std::string &expression);

    MappingFormula(ModelPtr model, const std::string &formula);
    MappingFormula(const std::vector<std::string> &variables, const std::string &formula, MemorySynchronizerPtr memorySynchronizer);


    double compute() const {return evaluator->evaluate();}

   private:
    MuParserExpressionEvaluatorPtr evaluator;
};

#endif  // MAPPINGFORMULA