#ifndef SPIRITPARSER_H
#define SPIRITPARSER_H


#include "ParseTreeComponents.h"
#include <map>
namespace ptc = ParseTreeComponents;

typedef std::pair<unsigned, std::string> ErrorMessageType;
typedef std::vector<ErrorMessageType> ErrorMessageList;

/**
 * \brief Class to hold the results of the parsing of the model
 * by the Spirit parser.
 *
 */

class ParsingResult {
 public:
    typedef std::map<std::string, int> OccurenceMapType;
    ParsingResult();
    void addStatement(const ptc::Statement &s);
    ptc::StatementList getStatements(){return statements;}
    unsigned getNbErrors() {return x_errorMessages.size();}
    void setErrorMessages(const ErrorMessageList &m){x_errorMessages = m;}
    ErrorMessageList getErrorMessages(){return x_errorMessages;}

    void setOccurenceMap(const OccurenceMapType &m) {occurenceMap = m;}
    int getOccurenceIndex(std::string name) {
	if (occurenceMap.find(name) == occurenceMap.end()) {
	    return -1;
	}
	return occurenceMap[name];
    }

 private:
    ptc::StatementList statements;
    ErrorMessageList x_errorMessages;
    OccurenceMapType occurenceMap;
};

typedef std::shared_ptr<ParsingResult> ParsingResultPtr;


/**
 * \brief Class to handle the parsing of the equations using the boost::spirit
 * parser.
 *
 * Will detect syntax but not semantic errors.  Interface is one
 * static function that returns a parse tree, which may contain the
 * errors that were detected.
 */


class SpiritParser {
 public:
    static ParsingResultPtr parseEquations(const std::string &eqns);

};

#endif // SPIRITPARSER_H
