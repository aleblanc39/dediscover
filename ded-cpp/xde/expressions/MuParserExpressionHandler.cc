#include <base/XDEMessage.h>
#include <base/XDEUtil.h>
#include <expressions/ExpressionSetEvaluator.h>
#include <expressions/MemoryBankSynchronizer.h>
#include <expressions/MuParserExpressionHandler.h>
#include <expressions/TimeVaryingFunctionHandler.h>
#include <extra_parser_functions/TimeVaryingFunctionFactory.h>
#include <parser/ParserException.h>

#include <boost/algorithm/string.hpp>
#include <regex>
#include "MuParserObjectsList.h"

using namespace std;
namespace ptc = ParseTreeComponents;

static TimeVaryingFunctionFactory TVFFactory;


set<string> MuParserExpressionHandler::functionNames =
    {"sin","cos","tan","asin","acos","atan","sinh",
        "cosh","tanh","asinh","acosh","atanh","log2","log10","log",
        "ln","exp","sqrt","sign","rint","abs","min","max","sum","avg"};

set<string> MuParserExpressionHandler::keywordNames =
   {"and","or","xor"};

set<string> MuParserExpressionHandler::predefinedVariables = { "t", "t0", "pi", "NaN", "INF"};

bool MuParserExpressionHandler::isDouble(std::string s) {
    try {
        // If the cast does not throw an exception then it is a double.
        (void)boost::lexical_cast<double>(s);
        return true;
    } catch (...) {
        // Otherwise check for predefined doubles.
        if (s == "pi" || s == "INF" || s == "-INF" || s == "+INF" || s == "NaN")
            return true;
        return false;
    }
}

/**
 * Returned true if w is a function or a keyword used in MuParser. Will expand
 * to includes words defined in the ExpressionEvaluator (PI, NaN, etc.) plus
 * the additional function (gamma_p, ind).
 */

bool MuParserExpressionHandler::isWordReserved(const std::string &w) {
    return (functionNames.find(w) != functionNames.end() ||
            keywordNames.find(w) != keywordNames.end() ||
            MuParserExpressionEvaluator::isFunctionDefined(w) ||
            TimeVaryingFunctionHandler::isFunctionDefined(w));
}

bool MuParserExpressionHandler::isFunctionDefined(string fName) {
    return (functionNames.find(fName) != functionNames.end() ||
            MuParserExpressionEvaluator::isFunctionDefined(fName));
}

bool MuParserExpressionHandler::isKeyword(const std::string &w) {
    return keywordNames.find(w) != keywordNames.end();
}

bool MuParserExpressionHandler::isPredefinedVariable(const std::string &w) {
    return predefinedVariables.find(w) != predefinedVariables.end();
}


MuParserExpressionHandler::MuParserExpressionHandler() {
    memorySynchronizer.reset(new MemoryBankSynchronizer(&memoryBank));
}

MuParserExpressionHandler::~MuParserExpressionHandler() {
    for (unsigned i = 0; i < evaluators.size(); i++) delete evaluators[i];
    // TODO Release TV functions
}

/**
 * Add an expression
 *
 * Will add an expression to the handler. For this class we will parse
 * the equations and create a new MuParserExpressionEvaluator object.
 */
int MuParserExpressionHandler::addExpression(string expression) {
    XDEMessage::log(
        XDEMessage::DEBUG1,
        string("MuParserExpressionHandler::addExpression:: Adding expression: ")
            .append(expression));
    evaluators.push_back(
        new MuParserExpressionEvaluator(expression, memorySynchronizer));
    XDEMessage::log(XDEMessage::DEBUG1, "Expression added ok");
    return evaluators.size() - 1;
}

/**
 * Set the value of a variable. Should we do error checking here?
 */
void MuParserExpressionHandler::setVariableValue(string symbol, double value) {
    memoryBank.setValue(symbol, value);
}

double MuParserExpressionHandler::getVariableValue(std::string symbol) {
    return memoryBank.getValue(symbol);
}

double MuParserExpressionHandler::compute(unsigned equationNO) const {
    XDE_ASSERT(equationNO < evaluators.size());
    return evaluators[equationNO]->evaluate();
}

string MuParserExpressionHandler::getExpression(unsigned equationNO) {
    XDE_ASSERT(equationNO < (unsigned)(evaluators.size()));
    return evaluators[equationNO]->getExpression();
}

vector<string> MuParserExpressionHandler::getExpressions() {
    vector<string> ret;
    for (unsigned i = 0; i < evaluators.size(); i++)
        ret.push_back(evaluators[i]->getExpression());
    return ret;
}

void MuParserExpressionHandler::remapMemory() {
    for (MuParserExpressionEvaluator *e : evaluators) {
        e->remapMemory();
    }
}

void MuParserExpressionHandler::resetParsedTVFunctions() {
    for (const auto &u : parsedTVFunctions) {
        TimeVaryingFunctionList::getInstance()->removeFunction(u);
    }
    parsedTVFunctions.clear();
}

//====================================================================
// Methods to generate code that can be used by muParser

/**
 * Will generate a processedModelExpression with the original expression.
 * At the end ptr will contain an (or several)  expression that can be read by
 * muParser, and a list of the identifiers used (that will help determine the
 * model parameters) as well as a list of the delayed variable used. Caller
 * will be responsible to identify errors, such as delay on wrong variables.
 *
 * Will create an expression in the Expression handler.
 *
 * This class will have information about valid functions.
 */

ProcessedModelExpressionPtr MuParserExpressionHandler::generateCode(
    const ptc::ExpressionPtr expression, int lineNO) {
    lineNumber = lineNO;

    ProcessedModelExpressionPtr ptr(
        new ProcessedModelExpression(expression->toString()));
    TermMuGenerator generator(ptr, this);

    string newExpression =
        generateCode(expression->getFirstTerm(), generator);
    if (expression->startsWithMinus()) newExpression = "-" + newExpression;
    if (isWordReserved(newExpression))
        throw ParserException(
            lineNumber,
            newExpression +
                " is a reserved word and can not be used in this context.");

    for (const ptc::OpTermType &op : expression->getOpTerms()) {
        newExpression.append(" " + op.first + " ");
        std::string ex = generateCode(op.second, generator);
        if (isWordReserved(ex))
            throw ParserException(
                lineNumber,
                ex +
                    " is a reserved word and can not be used in this context.");

        newExpression.append(ex);
    }

    ptr->setTransformedExpression(newExpression);
    ptr->setEquationID(addExpression(newExpression));

    // Collect here information collected by TermMuGenerator, regarding delay
    // expressions.

    return ptr;
}

/**
 * Generate code for the expression. In case it is a identifier, must
 * check that it is not a function name.
 * Can we merge this code with the above?
 */
string MuParserExpressionHandler::generateCode(
    const ptc::ExpressionPtr expression, ProcessedModelExpressionPtr ptr,
    TermMuGenerator &generator) {
    const std::string variable = "[[:alpha:]|_][[:alnum:]]*";
    static std::regex re(variable);

    string newExpression =
        generateCode(expression->getFirstTerm(),  generator);
    if (expression->startsWithMinus()) newExpression = "-" + newExpression;

    if (isWordReserved(newExpression))
        throw ParserException(
            generator.expressionHandler->getLineNumber(),
            newExpression +
                " is a reserved word and can not be used in this context.");

    for (const auto &op : expression->getOpTerms()) {
        newExpression.append(" " + op.first + " ");
        std::string ex = generateCode(op.second,  generator);

        //boost::cmatch what;
        if (std::regex_match(ex.c_str(), re)) {
            if (isWordReserved(ex)) {
                throw ParserException(
                    generator.expressionHandler->getLineNumber(),
                    ex + " is a reserved word and can not be used in this "
                         "context.");
            }
        }

        newExpression.append(ex);
    }
    return newExpression;
}

string MuParserExpressionHandler::generateCode(const ptc::TermPtr term,
                                               // ProcessedModelExpressionPtr ptr,
                                               TermMuGenerator &g) {
    ptc::TermVariant tv = term->getContents();
    return boost::apply_visitor(g, tv);
}

// ==============================================================
// Methods related to the visitor

std::string MuParserExpressionHandler::TermMuGenerator::operator()(
    double &d) const {
    return boost::lexical_cast<std::string>(d);
}

std::string MuParserExpressionHandler::TermMuGenerator::operator()(
    std::string &p) const {
    pme->addSymbolUsed(p);
    return p;
}

std::string MuParserExpressionHandler::TermMuGenerator::operator()(
    ptc::ParsedDoublePtr &d) const {
    return d->toString();
}

/**
 * Create the string for a delayed variable.
 * Will also add a DelayedVariables record.
 * Does not validate here (yet) if it defines a dependent variable. Caller will
 * do validation. Can add this later.
 */

std::string MuParserExpressionHandler::TermMuGenerator::operator()(
    ptc::DelayedVarPtr &p) {
    std::string varName = p->getVarName();
    if (!(expressionHandler->isDependentVariable(varName)))
        throw ParserException(
            this->expressionHandler->getLineNumber(),
            varName +
                " is not a dependent variables and can not used with a delay.");

    pme->addDelayedVariableUsed(varName);
    ptc::ExpressionPtr eptr = p->getExpression();
    string expressionCode = generateCode(eptr, pme, *this);
    DelayedVariablePtr dv =
        expressionHandler->addDelayedVariable(varName, expressionCode);
    return dv->getInternalName();
}

std::string MuParserExpressionHandler::TermMuGenerator::operator()(
    ptc::InitialConditionTermPtr &p) {
    std::string varName = p->getVarName();
    if (!(expressionHandler->isDependentVariable(varName)))
        throw ParserException(this->expressionHandler->getLineNumber(),
                              varName +
                                  " is not a dependent variable and can not be "
                                  "used with an initial condition.");

    return getInitialConditionString(varName);
}

std::string MuParserExpressionHandler::TermMuGenerator::operator()(
    ptc::IndicatorFunctionTermPtr &p) {
    vector<string> booleans = p->getBooleans();
    vector<ptc::ExpressionPtr> expressions = p->getExpressions();

    XDE_ASSERT(booleans.size() == 1 || booleans.size() == 2);
    if (booleans.size() == 1) {
        return MuParserExpressionEvaluator::getIndicatorString(
            generateCode(expressions[0], pme, *this), booleans[0],
            generateCode(expressions[1], pme, *this));
    } else {
        return MuParserExpressionEvaluator::getIndicatorString(
            generateCode(expressions[0], pme, *this), booleans[0],
            generateCode(expressions[1], pme, *this), booleans[1],
            generateCode(expressions[2], pme, *this));
    }
}

std::string MuParserExpressionHandler::TermMuGenerator::operator()(
    ptc::ExpressionPtr &e) {
    return "(" + generateCode(e, pme, *this) + ")";
}

/**
 * Generate code for function call. Will check that it is a valid function call.
 *
 */

std::string MuParserExpressionHandler::TermMuGenerator::operator()(
    ptc::FunctionCallPtr &f) {
    std::string varName = f->getVarName();

    if (!isFunctionDefined(varName))
        throw ParserException(this->expressionHandler->getLineNumber(),
                              varName + " is not a valid function name.");

    pme->addFunctionUsed(varName);

    std::string ret = varName + "(";
    std::string comma = "";
    for (const auto &p: f->getArguments()) {
        ret += comma;
        ret += generateCode(p, pme, *this);
        comma = ",";
    }
    ret += ")";
    return ret;
}

std::string MuParserExpressionHandler::TermMuGenerator::operator()(
    ptc::TimeVaryingFunctionTermPtr &f) {
    std::string varName = f->getVarName();
    pme->addFunctionUsed(varName);

    TimeVaryingFunctionPtr tvf;
    try {
        tvf = TimeVaryingFunctionHandler::createTVFunction(
            varName, expressionHandler->getMemorySynchronizer(),
            "Original function...>");
    } catch (XDEException &e) {
        throw ParserException(this->expressionHandler->getLineNumber(),
                              e.getMsg());
    }
    unsigned indx = TimeVaryingFunctionList::getInstance()->addFunction(tvf);
    expressionHandler->insertParsedTVFunction(indx);

    std::vector<ptc::HistoryBoundVariant> leftVector = f->getLeftVector();
    std::vector<ptc::HistoryBoundVariant> rightVector = f->getRightVector();
    if (leftVector.size() != rightVector.size())
        throw ParserException(
            this->expressionHandler->getLineNumber(),
            "Mismatch in vector size for time-varying function: " + varName);

    unsigned n = leftVector.size();

    std::string vleft;
    std::string vright;

    for (unsigned i = 0; i < n; i++) {
        vleft = boost::apply_visitor(ptc::StringBoundVisitor(), leftVector[i]);
        vright =
            boost::apply_visitor(ptc::StringBoundVisitor(), rightVector[i]);

        addSymbolUsed(vleft);
        addSymbolUsed(vright);

        TimeVaryingEntry tvel = getTimeVaryingEntry(vleft);
        TimeVaryingEntry tver = getTimeVaryingEntry(vright);

        tvf->addControlPoint(tvel, tver);
    }

    std::string timeVar = boost::apply_visitor(ptc::StringBoundVisitor(),
                                               f->getSingleArguments()[0]);
    addSymbolUsed(timeVar);

    return MuParserExpressionEvaluator::getTVFunctionString(indx, timeVar);
}

static double getDouble(std::string str) {
    boost::trim(str);

    if (str == "-INF") return -numeric_limits<double>::infinity();
    if (str == "+INF" || str == "INF")
        return numeric_limits<double>::infinity();
    if (str == "NaN") return XDEUtil::XDE_NAN;
    if (str == "pi") return boost::math::constants::pi<double>();
    return boost::lexical_cast<double>(str);
}

TimeVaryingEntry
MuParserExpressionHandler::TermMuGenerator::getTimeVaryingEntry(
    const std::string s) {
    try {
        return getDouble(s);
    } catch (...) {
        return s;
    }
}

void MuParserExpressionHandler::TermMuGenerator::addSymbolUsed(std::string v) {
    if (!(isDouble(v) || isKeyword(v)))  // define isConstant?
        pme->addSymbolUsed(v);
}

/**
 * \brief Add a delayed variable for var name, and expression code.
 *
 * If the expression code alread exists, use it again.
 *
 * The DelayedVariable object contains an ID, corresponding to the equation ID
 * in MuParserExpressionHandler.
 *
 */
DelayedVariablePtr MuParserExpressionHandler::addDelayedVariable(
    const std::string &varName, const std::string &expressionCode) {
    DelayedVariablePtr dv;
    map<string, unsigned>::iterator it =
        delayedVariableExpressions.find(expressionCode);
    if (it == delayedVariableExpressions.end()) {
        unsigned newID = addExpression(expressionCode);
        XDEMessage::log(XDEMessage::DEBUG1,
                        "Adding a delayed variable expression.");
        delayedVariableExpressions.insert(
            pair<string, unsigned>(expressionCode, newID));
        dv.reset(new DelayedVariable(varName, newID));
    } else {
        dv.reset(new DelayedVariable(varName, it->second));
    }
    delayedVariables.push_back(dv);
    return dv;
}
