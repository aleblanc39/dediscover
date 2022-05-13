#include "ParseTree.h"


using namespace ParseTreeComponents;

ParseTree::ParseTree(std::string equations) {
    result = SpiritParser::parseEquations(equations);

    errorMessages = result->getErrorMessages();
    if (errorMessages.size() == 0) populateStatementLists(result);
}

void ParseTree::populateStatementLists(ParsingResultPtr result) {
    for(const auto &p : result->getStatements()) {
        if (p->getStatementType() == Statement::DERIVATIVE) {
            derivativeStatements.push_back(p);
        } else if (p->getStatementType() == Statement::MACRO) {
            macroStatements.push_back(p);
        } else if (p->getStatementType() == Statement::INITIAL_CONDITION) {
            initialConditionStatements.push_back(p);
        } else if (p->getStatementType() == Statement::HISTORY) {
            historyStatements.push_back(p);
        } else if (p->getStatementType() == Statement::COVARIATE) {
            covariateStatements.push_back(p);
        } else {
            std::cerr << "ParseTree::populateStatementsList: Can't recognize "
                         "statement!!!\n";
        }
        allStatements.push_back(p);
    }
}
