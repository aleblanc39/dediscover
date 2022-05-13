#include <mapping/MappingFormula.h>
#include <mapping/ParsingResults.h>

using namespace std;

// Maybe just need the formula here.  Values used in working  data
MappingFormula::MappingFormula(ModelPtr model, const string &formula) {
    string errorMsg = MappingFormula::validateFormula(model, formula);
    if (errorMsg != "") {
        throw FormulaException(errorMsg);
    }

    evaluator = MuParserExpressionEvaluatorPtr(new MuParserExpressionEvaluator(
        formula, model->getMemorySynchronizer()));
}

MappingFormula::MappingFormula(const std::vector<std::string> &variables,
                               const std::string &formula,
                               MemorySynchronizerPtr memorySynchronizer) {
    string errorMsg = MappingFormula::validateFormula(variables, formula);
    if (errorMsg != "") {
        throw FormulaException(errorMsg);
    }

    evaluator = MuParserExpressionEvaluatorPtr(
        new MuParserExpressionEvaluator(formula, memorySynchronizer));
}

/**
 * Validate a mapping.
 *
 * Will validate the mapping defined by the expression. If the
 * expression is valid then nothing happens, otherwise a
 * MappingException is thrown.
 */

string MappingFormula::validateFormula(const vector<string> &variableNamesSet,
                                       const string &expression) {
    ParsingResults results(expression);
    vector<string> fNames = results.getFunctionNames();

    vector<string>::iterator it;
    vector<string>::iterator it_end = fNames.end();
    for (it = fNames.begin(); it != it_end; it++) {
        if (!MuParserExpressionHandler::isFunctionDefined(*it))
            return (string("Function is not defined: ").append(*it));
    }
    vector<string> symbols = results.getSymbolNames();
    it_end = symbols.end();
    //vector<string>::iterator varNamesSetEnd = variableNamesSet.end();
    for (it = symbols.begin(); it != it_end; it++) {
        // if (variableNamesSet.find(*it) == varNamesSetEnd)
        if (std::find(variableNamesSet.begin(), variableNamesSet.end(), *it) ==
            variableNamesSet.end())

            return (string("Error in mapping: variable ")
                        .append(*it)
                        .append(" is not defined."));
    }

    // Right now there is no reason to have something of the form X[]
    // in the mapping.
    if (results.getIndexedSymbolNames().size() > 0) {
        return ("There should not be indexed variables in the mapping.");
    }
    return "";
}

string MappingFormula::validateFormula(ModelPtr model,
                                       const string &expression) {
    // set<string> variableNamesSet(model->getAllVariables().begin(),
    //                              model->getAllVariables().end());
    return MappingFormula::validateFormula(model->getAllVariables(), expression);
}
