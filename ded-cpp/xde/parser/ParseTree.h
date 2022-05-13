#ifndef PARSETREE_H
#define PARSETREE_H

#include <string>
#include <vector>

#include "SpiritParser.h"
#include "ParseTreeComponents.h"

namespace ptc = ParseTreeComponents;

/**
 * \brief Class to create a parse tree from the XDE equations
 *
 * Will perform syntactic analysis but no semantics. So the results
 * from getMacros() and getDependentVariable() may overlap, and no
 * check is made on the functions to see that they are defined.
 */

class ParseTree
{

public:
    ParseTree(std::string s);
    ErrorMessageList getErrorMessages() {
        return errorMessages;
    }

    ptc::StatementList getDerivativeStatements() { return derivativeStatements; }
    ptc::StatementList getMacroStatements() { return macroStatements; }
    ptc::StatementList getInitialConditionStatements() { return initialConditionStatements; }
    ptc::StatementList getHistoryStatements() { return historyStatements; }
    ptc::StatementList getCovariateStatements() { return covariateStatements; }
    ptc::StatementList getAllStatements() { return allStatements; }

    int getOccurenceIndex(std::string var)  {
        return result->getOccurenceIndex(var);
    }

private:
    ErrorMessageList errorMessages;
    void populateStatementLists(ParsingResultPtr result);
    ParsingResultPtr result;

    ptc::StatementList derivativeStatements;
    ptc::StatementList macroStatements;
    ptc::StatementList initialConditionStatements;
    ptc::StatementList historyStatements;
    ptc::StatementList covariateStatements;
    ptc::StatementList allStatements;
};

typedef std::shared_ptr<ParseTree> ParseTreePtr;

#endif
