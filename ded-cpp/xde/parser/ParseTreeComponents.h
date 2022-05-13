#ifndef PARSETREECOMPONENTS_H
#define PARSETREECOMPONENTS_H

#include <base/XDEUtil.h>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/math/constants/constants.hpp>

#include <boost/variant.hpp>
#include <iostream>
#include <stack>
#include <string>
#include <vector>

/**
 * This file contains a collection of classes that are used by the
 * Spirit parser when parsing the model and creating the objects
 * based on its grammar.
 */

/**
 * \brief Namespace defining the different components resulting from parsing
 * the model with the SpiritParser.
 */
namespace ParseTreeComponents {

class Term;
class Expression;
class DelayedVar;
class FunctionCall;
class InitialConditionTerm;
class TimeVaryingFunctionTerm;
class IndicatorFunctionTerm;
class ParsedDouble;

typedef std::shared_ptr<Term> TermPtr;
typedef std::shared_ptr<Expression> ExpressionPtr;
typedef std::shared_ptr<DelayedVar> DelayedVarPtr;
typedef std::shared_ptr<FunctionCall> FunctionCallPtr;
typedef std::shared_ptr<InitialConditionTerm> InitialConditionTermPtr;
typedef std::shared_ptr<TimeVaryingFunctionTerm> TimeVaryingFunctionTermPtr;
typedef std::shared_ptr<IndicatorFunctionTerm> IndicatorFunctionTermPtr;
typedef std::shared_ptr<ParsedDouble> ParsedDoublePtr;

typedef std::vector<Expression> VectorOfExpressions;
typedef boost::variant<std::string, ParsedDouble> HistoryBoundVariant;

// Quick hack,
static double getDouble(std::string str) {
    boost::trim(str);

    if (str == "-INF") return -std::numeric_limits<double>::infinity();
    if (str == "+INF" || str == "INF" || str == "t0")
        return std::numeric_limits<double>::infinity();
    if (str == "NaN") return XDEUtil::XDE_NAN;
    if (str == "pi") return boost::math::constants::pi<double>();
    return boost::lexical_cast<double>(str);
}

class ParsedDouble {
   public:
    ParsedDouble() {}
    ParsedDouble(double d) : doubleValue(d) {}
    double getValue() { return doubleValue; }
    std::string toString() { return stringValue; }
    void setStringValue(const std::string &s) { stringValue = s; }

    double doubleValue;
    std::string stringValue;
};

class BoundVisitor : public boost::static_visitor<double> {
   public:
    double operator()(double d) const { return d; }
    double operator()(std::string s) const { return getDouble(s); }
    double operator()(ParsedDouble p) const { return p.getValue(); }
};

class StringBoundVisitor : public boost::static_visitor<std::string> {
   public:
    std::string operator()(double d) const {
        return boost::lexical_cast<std::string>(d);
    }
    std::string operator()(std::string s) const { return s; }
    std::string operator()(ParsedDouble p) const { return p.toString(); }
};

class Statement {
   public:
    typedef enum {
        DERIVATIVE,
        MACRO,
        INITIAL_CONDITION,
        HISTORY,
        COVARIATE
    } StatementType;

    Statement() {}
    Statement(const std::string &a) : varName(a) {}

    void setLowerHistoryBound(HistoryBoundVariant v) { lowerHistoryBound = v; }
    void setUpperHistoryBound(HistoryBoundVariant v) { upperHistoryBound = v; }

    void setLowerBoundSign(std::string s) { lowerBoundSign = s; }
    void setUpperBoundSign(std::string s) { upperBoundSign = s; }

    HistoryBoundVariant getLowerHistoryBound() { return lowerHistoryBound; }
    HistoryBoundVariant getUpperHistoryBound() { return upperHistoryBound; }

    std::string getLowerBoundSign() { return lowerBoundSign; }
    std::string getUpperBoundSign() { return upperBoundSign; }

    void addExpression(const Expression &s);
    void setLineNumber(unsigned *l) {
        lineNumber = *l;
        /**std::cerr << "In Statement: Assigning line number: " << *l <<
           "\n";**/
    }
    unsigned getLineNumber() { return lineNumber; }
    std::string getVarName() { return varName; }
    ExpressionPtr getExpression() const { return expression; }
    void setStatementType(StatementType s) {
        statementType = s;
        /*std::cerr << "In Statement: Setting type to: " << s << "\n";*/
    }
    StatementType getStatementType() { return statementType; }

   protected:
    StatementType statementType;
    std::string varName;
    unsigned lineNumber;
    ExpressionPtr expression;

    // Values related to upper lower history bounds.

    HistoryBoundVariant lowerHistoryBound;
    HistoryBoundVariant upperHistoryBound;

    std::string lowerBoundSign;
    std::string upperBoundSign;
};

typedef std::shared_ptr<Statement> StatementPtr;
typedef std::vector<StatementPtr> StatementList;

// Right now the SpiritParser does not really use the inheritance. Set
// strings in base class, and use them. Can come back to this later, when
// we add delay statements.
// Update: Will add delay bounds to base statement class, for now

class MacroStatement : public Statement {
   public:
    MacroStatement() {}
    MacroStatement(const std::string &s);
};

class DerivativeStatement : public Statement {
   public:
    DerivativeStatement() {}
    DerivativeStatement(const std::string &s);
};

class InitialConditionStatement : public Statement {
   public:
    InitialConditionStatement() {}
    InitialConditionStatement(const std::string &s);
};

typedef std::pair<std::string, TermPtr> OpTermType;

class Expression {
   public:
    Expression() { startsWithNeg = false; }
    Expression(const Term &t);
    Expression(const std::string &s);

    void addOperator(const std::string &s);
    void addTerm(const Term &t);

    std::string toString() const;

    TermPtr getFirstTerm() { return firstTerm; }
    std::vector<OpTermType> getOpTerms() { return opTerms; }
    bool startsWithMinus() { return startsWithNeg; }

   private:
    bool startsWithNeg;
    TermPtr firstTerm;
    std::vector<OpTermType> opTerms;
};

typedef boost::variant<std::string, double, ExpressionPtr, DelayedVarPtr,
                       InitialConditionTermPtr, FunctionCallPtr,
                       TimeVaryingFunctionTermPtr, IndicatorFunctionTermPtr,
                       ParsedDoublePtr>
    TermVariant;

class Term {
   public:
    Term() {}
    Term(const Expression &e);
    Term(const double &d);
    Term(const std::string &s);
    Term(const DelayedVar &s);
    Term(const FunctionCall &s);
    Term(const TimeVaryingFunctionTerm &s);
    Term(const InitialConditionTerm &s);
    Term(const IndicatorFunctionTerm &s);
    Term(const ParsedDouble &s);

    std::string toString() const;
    TermVariant getContents() { return termContents; }

    void setOriginalString(std::string s) {
        std::cerr << "Setting original string in term to: " << s << std::endl;
        originalString = s;
    }

   private:
    std::string originalString;
    TermVariant termContents;
};

class DelayedVar {
   public:
    DelayedVar(){};
    DelayedVar(const std::string &s) : varName(s) {}
    void addExpression(const Expression &s) {
        expression.reset(new Expression(s));
    }
    std::string getVarName() { return varName; }
    ExpressionPtr getExpression() { return expression; }
    std::string toString();

   private:
    std::string varName;
    ExpressionPtr expression;
};

class InitialConditionTerm {
   public:
    InitialConditionTerm() {}
    InitialConditionTerm(const std::string &s) : varName(s) {}
    std::string getVarName() { return varName; }
    std::string toString();

   private:
    std::string varName;
};

class FunctionCall {
   public:
    FunctionCall() {}
    FunctionCall(const std::string &s) : varName(s) {}

    void addExpressionList(const VectorOfExpressions &e);
    std::vector<ExpressionPtr> getArguments() { return argList; }
    std::string toString();
    std::string getVarName() { return varName; }

   private:
    std::string varName;
    std::vector<ExpressionPtr> argList;
};

class TimeVaryingFunctionTerm {
   public:
    TimeVaryingFunctionTerm() {}
    TimeVaryingFunctionTerm(const std::string &s) : varName(s) {}

    std::string toString();
    std::string getVarName() { return varName; }

    std::vector<HistoryBoundVariant> getLeftVector() { return leftVector; }
    std::vector<HistoryBoundVariant> getRightVector() { return rightVector; }
    std::vector<HistoryBoundVariant> getSingleArguments() {
        return singleArguments;
    }

    void setSingleArguments(const std::vector<HistoryBoundVariant> &v) {
        singleArguments = v;
    }
    void setLeftVector(const std::vector<HistoryBoundVariant> &v) {
        leftVector = v;
    }
    void setRightVector(const std::vector<HistoryBoundVariant> &v) {
        rightVector = v;
    }

    void addSingleArgument(HistoryBoundVariant v) {
        singleArguments.push_back(v);
    }
    void addToLeftVector(HistoryBoundVariant v) { leftVector.push_back(v); }
    void addToRightVector(HistoryBoundVariant v) { rightVector.push_back(v); }

    void setDefaultValue(HistoryBoundVariant v) { defaultValue = v; }

   private:
    HistoryBoundVariant defaultValue;
    std::string varName;
    std::vector<HistoryBoundVariant> singleArguments;
    std::vector<HistoryBoundVariant> leftVector;
    std::vector<HistoryBoundVariant> rightVector;
};

class IndicatorFunctionTerm {
   public:
    IndicatorFunctionTerm() {}
    IndicatorFunctionTerm(const std::string &s) : varName(s) {}

    void addBoolean(const std::string &s) { booleans.push_back(s); }
    void addExpression(const Expression &s) {
        expressions.push_back(ExpressionPtr(new Expression(s)));
    }

    std::vector<std::string> getBooleans() { return booleans; }
    std::vector<ExpressionPtr> getExpressions() { return expressions; }

    std::string toString();

   private:
    std::string
        varName;  // This is really a dymmy here. Will always be set to I
    std::vector<std::string> booleans;
    std::vector<ExpressionPtr> expressions;
};

// Define a visitor to return a string on the TermVariant

class TermVisitor : public boost::static_visitor<std::string> {
   public:
    std::string operator()(double d) const {
        return boost::lexical_cast<std::string>(d);
    }
    std::string operator()(std::string p) const { return p; }
    std::string operator()(DelayedVarPtr p) const { return p->toString(); }
    std::string operator()(ExpressionPtr e) const {
        return "(" + e->toString() + ")";
    }
    std::string operator()(FunctionCallPtr f) const { return f->toString(); }

    std::string operator()(TimeVaryingFunctionTermPtr f) const {
        return f->toString();
    }

    std::string operator()(InitialConditionTermPtr f) const {
        return f->toString();
    }

    std::string operator()(IndicatorFunctionTermPtr f) const {
        return f->toString();
    }

    std::string operator()(ParsedDoublePtr f) const { return f->toString(); }
};
}  // namespace ParseTreeComponents

#endif  // PARSETREECOMPONENTS_H
