#include "ParsingResults.h"

#include <base/XDEMessage.h>
#include <base/XDEUtil.h>

#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <boost/tokenizer.hpp>

using namespace boost;
using namespace std;

static void addToSet(const string &s, set<string> &aSet);

// TODO ALL2021: Move somewhere else? parser directory?

ParsingResults::ParsingResults(const string &expression) {
    parseExpression(expression);
}

/**
 * Add a symbol.
 *
 * Will add a symbol. If it is a duplicate then nothing is
 * changed.
 */

void ParsingResults::addSymbol(const string &symbol) { addToSet(symbol, symbolNames); }

/**
 * Add a function name..
 *
 * Will add a symbol. If it is a duplicate then nothing is
 * changed.
 */

void ParsingResults::addFunction(const string &functionName) {
    addToSet(functionName, functionNames);
}

void ParsingResults::addIndexedSymbol(const string &indexedSymbol) {
    addToSet(indexedSymbol, indexedSymbolNames);
}

vector<string> ParsingResults::getFunctionNames() {
    vector<string> ret(functionNames.begin(), functionNames.end());
    return ret;
}

vector<string> ParsingResults::getSymbolNames() {
    vector<string> ret(symbolNames.begin(), symbolNames.end());
    return ret;
}

vector<string> ParsingResults::getIndexedSymbolNames() {
    vector<string> ret(indexedSymbolNames.begin(), indexedSymbolNames.end());
    return ret;
}

/**
 * Parse an expression to extract the symbols.
 *
 * Parse an expression to extract the function names and model
 * symbols. Does not validate the symbol as such.
 * A function name is any symbol name followed by an open
 * parenthesis. Anything else is considered a symbol.
 */

void ParsingResults::parseExpression(const string &expression) {
    XDEMessage::log(
        XDEMessage::DEBUG2,
        string("ExpressionParser: parsing expression: ").append(expression));

    static regex re("([^[:alpha:]_])?([[:alpha:]|_][[:word:]]*)(.*)");

    cmatch what;
    string match;
    string remainder(expression);
    bool moreTokens = true;
    while (moreTokens) {
        bool ret = regex_search(remainder.c_str(), what, re);
        if (!ret) {
            moreTokens = false;
        } else {
            string leftChar(what[1].first, what[1].second);
            string rightChar(what[3].first, what[3].second);
            string token(what[2].first, what[2].second);
            XDEMessage::log(XDEMessage::DEBUG5,
                            string("Looking at: ").append(token));
            trim(leftChar);
            trim(rightChar);

            // If the character to the left is a digit then this need
            // to be e or E followed by +/- then numbers.
            // line must match regex

            if ((toupper(token[0]) == 'E') && leftChar.length() > 0 &&
                isdigit(leftChar[leftChar.length() - 1])) {
                // We just matched [digit][e|E] and this can only be a
                // floating point number. Nothing to do.
            }

            // Now check if this is a function call.
            else if (rightChar.length() > 0 && rightChar[0] == '(') {
                addFunction(token);
            }
            // Check if it is an indexed variable such as X[...].
            else if (rightChar.length() > 0 && rightChar[0] == '[') {
                addIndexedSymbol(token);
            }
            // Simply a token. Could be any reserved word, but this
            // is not a concern here.
            else {
                addSymbol(token);
            }
            remainder.assign(rightChar);
        }
    }
}

// Local utility functions.

void addToSet(const string &s, set<string> &aSet) {
    string s1(s);
    trim(s1);
    XDE_ASSERT(s1.size() > 0);
    aSet.insert(s1);
}
