#include <mapping/MappingFormula.h>
#include <mapping/ParsingResults.h>

// TODO Too specific here. Only used for isFunctionDefined function. 
// Should be passed as parameter
#include <expressions/MuParserExpressionHandler.h>


using namespace std;


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
    for (it = symbols.begin(); it != it_end; it++) {
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

