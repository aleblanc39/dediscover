#include "ParseTreeComponents.h"

#include <cassert>
#include <iostream>

using namespace std;

namespace ParseTreeComponents {
MacroStatement::MacroStatement(const std::string &s) : Statement(s) {
    //    cerr << "Will create macro statement from string: " << s << endl;
}
DerivativeStatement::DerivativeStatement(const std::string &s) : Statement(s) {
    // cerr << "Will create derivative statement from string: " << s << endl;
}

InitialConditionStatement::InitialConditionStatement(const std::string &s)
    : Statement(s) {
    // cerr << "Will create initial condition statement from string: " << s <<
    // endl;
}

Term::Term(const Expression &e) {
    // cerr << "Creating term from an expression\n";
    termContents = ExpressionPtr(new Expression(e));
}
Term::Term(const double &d) {
    // cerr << "Creating term from double: " << d <<endl;
    termContents = d;
}

Term::Term(const std::string &s) {
    // cerr << "Creating term from string " << s << endl;
    termContents = s;
    ;
}

Term::Term(const DelayedVar &d) {
    // cerr << "Creating term from delayedVar\n";
    termContents = DelayedVarPtr(new DelayedVar(d));
}

Term::Term(const ParsedDouble &pd) {
    termContents = ParsedDoublePtr(new ParsedDouble(pd));
}

Term::Term(const InitialConditionTerm &d) {
    // cerr << "Creating term from delayedVar\n";
    termContents = InitialConditionTermPtr(new InitialConditionTerm(d));
}

Term::Term(const FunctionCall &d) {
    //	cerr << "Creating term from function call\n";
    termContents = FunctionCallPtr(new FunctionCall(d));
}

Term::Term(const TimeVaryingFunctionTerm &d) {
    // cerr << "Creating term from function call\n";
    termContents = TimeVaryingFunctionTermPtr(new TimeVaryingFunctionTerm(d));
}

Term::Term(const IndicatorFunctionTerm &t) {
    // cerr << "Creating term from Indicator function\n";
    termContents = IndicatorFunctionTermPtr(new IndicatorFunctionTerm(t));
}

string Term::toString() const {
    if (originalString != "") return originalString;
    return boost::apply_visitor(TermVisitor(), termContents);
}

void Expression::addOperator(const std::string &s) {
    // cerr << "Adding operator to expression: " << s << endl;
    opTerms.push_back(OpTermType(s, TermPtr()));
}

void Expression::addTerm(const Term &t) {
    // cerr << "Adding term: " << t.toString() << " to expression\n";
    if (firstTerm == NULL)
        firstTerm.reset(new Term(t));
    else
        opTerms.back().second.reset(new Term(t));
}

Expression::Expression(const Term &t) {
    // cerr << "Will build expression from a term: " << t.toString() << endl;
    // cerr << "Creating expression directly from a term\n";
    startsWithNeg = false;
    firstTerm.reset(new Term(t));
}

Expression::Expression(const std::string &s) {
    if (s == "-") {
        startsWithNeg = true;
        // firstTerm.reset(new Term("-"));
    } else {
        startsWithNeg = false;
    }
}

string Expression::toString() const {
    string ret(firstTerm->toString());
    for (const auto &op : opTerms) {
        ret.append(" ").append(op.first).append(" ").append(
            op.second->toString());
    }
    // cerr << "Expression::toString, startsWithNeg is: " << startsWithNeg <<
    // endl;
    if (startsWithNeg) ret = "-" + ret;
    // cerr << "Returning " << ret << endl;
    return ret;
}

void Statement::addExpression(const Expression &s) {
    // cerr << "Will add expression to statement\n";
    expression.reset(new Expression(s));
}

string DelayedVar::toString() {
    string ret(varName);
    ret.append("[").append(expression->toString()).append("]");
    return ret;
}

string InitialConditionTerm::toString() {
    string ret(varName);
    ret.append("[t0]");
    return ret;
}

string IndicatorFunctionTerm::toString() {
    // TODO Implement for real...
    return "some indicator function\n";
}

void FunctionCall::addExpressionList(const VectorOfExpressions &e) {
    for (const auto &ex : e) {
        argList.push_back(ExpressionPtr(new Expression(ex)));
    }
}

string FunctionCall::toString() {
    string ret = varName + "(";
    if (argList.size() > 0) ret += argList[0]->toString();
    for (unsigned i = 1; i < argList.size(); i++)
        ret += (", " + argList[i]->toString());
    ret += ")";
    return ret;
}

string TimeVaryingFunctionTerm::toString() {
    string ret = varName + "(";
    if (singleArguments.size() > 0)
        ret += boost::apply_visitor(StringBoundVisitor(), singleArguments[0]);
    for (unsigned i = 1; i < singleArguments.size(); i++)
        ret += (", " +
                boost::apply_visitor(StringBoundVisitor(), singleArguments[i]));

    ret += "[" + boost::apply_visitor(StringBoundVisitor(), leftVector[0]);
    for (unsigned i = 1; i < leftVector.size(); i++)
        ret +=
            (", " + boost::apply_visitor(StringBoundVisitor(), leftVector[i]));
    ret += "],[" + boost::apply_visitor(StringBoundVisitor(), rightVector[0]);
    for (unsigned i = 1; i < rightVector.size(); i++)
        ret +=
            (", " + boost::apply_visitor(StringBoundVisitor(), rightVector[i]));
    ret += "])";
    return ret;
}
}  // namespace ParseTreeComponents
