#ifndef PARSINGRESULTS_H
#define PARSINGRESULTS_H

#include <vector>
#include <string>
#include <set>



/**
 * \brief Holds the results of the ExpressionParser.
 *
 * Will divide the results into 3 types: function names, 
 * symbol names and indexed symbol names.
 */

class ParsingResults {


 public:
    ParsingResults(const std::string &expression);

    void addSymbol(const std::string &symbol);
    void addIndexedSymbol(const std::string &symbol);
    void addFunction(const std::string &function);

    std::vector<std::string> getFunctionNames();
    std::vector<std::string> getSymbolNames();
    std::vector<std::string> getIndexedSymbolNames();

 private:
    std::set<std::string> functionNames;
    std::set<std::string> symbolNames;
    std::set<std::string> indexedSymbolNames;

    void parseExpression(const std::string &expression);

};


#endif // PARSINGRESULTS
