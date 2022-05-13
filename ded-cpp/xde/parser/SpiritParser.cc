#define BOOST_SPIRIT_USE_PHOENIX_V3 1
#include "SpiritParser.h"

#include <boost/algorithm/string.hpp>
#include <boost/config/warning_disable.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/spirit/home/classic/phoenix/new.hpp>
#include <boost/spirit/home/support/iterators/line_pos_iterator.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_eps.hpp>
#include <boost/spirit/include/qi_repeat.hpp>
#include <boost/spirit/include/support_ascii.hpp>
#include <boost/spirit/repository/include/qi_confix.hpp>
namespace phx = boost::phoenix;

#include <stdio.h>
#include <stdlib.h>

#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <vector>

using std::cin;
using std::cout;
using std::endl;
using std::ifstream;
using std::string;
using std::vector;

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace repo = boost::spirit::repository;

using ascii::alnum;
using ascii::alpha;
using ascii::char_;
using ascii::cntrl;
using ascii::graph;
using ascii::print;
using ascii::space;
using boost::apply_visitor;
using std::shared_ptr;
using boost::phoenix::function;
using boost::spirit::get_column;
using boost::spirit::qi::unused_type;
using qi::_1;
using qi::_val;
using qi::eps;
using qi::grammar;
using qi::lexeme;
using qi::rule;
using repo::confix;

#include <boost/spirit/include/phoenix_object.hpp>
namespace ptc = ParseTreeComponents;
static unsigned lineNumber = 0;
static unsigned currentPos = 0;
typedef vector<ptc::Statement> VectorOfStatements;

static ErrorMessageList errorMessages;

struct CreateParsingResultFunction {
    template <typename A, typename B = qi::unused_type>
    struct result {
        typedef ParsingResult *type;
    };

    ParsingResult *operator()(VectorOfStatements &s) const {
        ParsingResult *result = new ParsingResult();
        for (ptc::Statement &st : s) {
            result->addStatement(st);
        }
        return result;
    }
};

template <typename Iterator>
class XDEErrorHandler {
   public:
    template <typename, typename, typename, typename, typename, typename>
    struct result {
        typedef void type;
    };

    /** @short An error has occured while parsing an attribute's value.
     *
     *   Function invoked when bad value type of the attribute was parsed.
     *
     *   @param start Begin of the input being parsed when the error occures
     *   @param end End of the input being parsed when the error occures
     *   @param errorPos Position where the error occures
     *   @param what Expected tokens
     *   @param attributeName Name of attribute which value is currently being
     * parsed
     *   @param parser Pointer to main parser for purposes of storing generated
     * error
     *   @see ParseError
     */
    void operator()(Iterator start, Iterator /*end*/, Iterator errorPos,
                    const boost::spirit::info & /*what*/, unsigned /*lineNO*/,
                    string msg) const {
        std::cerr << "Reporting an error on line " << lineNumber << ": " << msg
                  << "\n";
        int pos = get_column(start, errorPos);
        // errorMessages.push_back(ErrorMessageType(lineNumber,
        // boost::lexical_cast<string>(what)));
        errorMessages.push_back(ErrorMessageType(
            lineNumber,
            msg + " at position: " + boost::lexical_cast<string>(pos)));
        // const Db::Identifier &attributeName, ParserImpl<Iterator> *parser)
        // const;
    }
};

void addLine() {
    lineNumber++;
    currentPos = 0;
}

static std::map<std::string, int> occurenceMap;
static unsigned occurenceIndex = 0;

// Keeping track of occurence. May have issues if XXX and dXXX are both defined,
// since dXXX/dt may cause dXXX to come here to early. Ignore for now.
static void updateOccurence(const std::string &symbol) {
    // std::cerr << "*** Just encountered " << symbol << " ***" << std::endl;
    if (occurenceMap.find(symbol) == occurenceMap.end())
        occurenceMap[symbol] = occurenceIndex++;
}
// void updatePos(int length) {
//     //std::cerr << "Updating match of length" << length << std::endl;
//     currentPos += length;
// }

unsigned getLineNo() { return lineNumber; }

/**
 * Define a skipper. Just using ascii::space will skip over eol's, which is
 * bad when parsing multiline input.
 * Skipper must skip spaces and comments, respect eol's as separators
 */

template <typename Iterator>
struct XDESkipper : public qi::grammar<Iterator> {
    XDESkipper() : XDESkipper::base_type(skip, "PL/0") {
        skip = ascii::space - qi::eol | qi::lit("#") >> *(qi::char_ - qi::eol) |
               qi::lit("//") >> *(qi::char_ - qi::eol);
    }
    qi::rule<Iterator> skip;
};

BOOST_FUSION_ADAPT_STRUCT(ptc::ParsedDouble,
                          (double, doubleValue)(std::string, stringValue))

template <typename Iterator, typename Skipper = XDESkipper<Iterator> >
struct XDE_parser : qi::grammar<Iterator, ParsingResult *(), Skipper> {
    XDE_parser(Iterator it_begin) : XDE_parser::base_type(start) {
        using ascii::char_;
        using boost::phoenix::construct;
        using boost::phoenix::val;
        using qi::_1;
        using qi::_4;

        using phx::at_c;
        using phx::begin;
        using phx::end;

        start = listOfStatements[_val = CreateParsingResult(_1)] > qi::eoi;

        listOfStatements %= *qi::eol[&addLine] > statement >>
                            +qi::eol[&addLine] >
                            *(statement >> *qi::eol[&addLine]);

        statement %= covariateStatement[bind(&ptc::Statement::setStatementType,
                                             _val, ptc::Statement::COVARIATE)] |
                     macroStatement[boost::phoenix::bind(
                         &ptc::Statement::setStatementType, _val,
                         ptc::Statement::MACRO)] |
                     derivativeStatement[boost::phoenix::bind(
                         &ptc::Statement::setStatementType, _val,
                         ptc::Statement::DERIVATIVE)] |
                     historyStatement[boost::phoenix::bind(
                         &ptc::Statement::setStatementType, _val,
                         ptc::Statement::HISTORY)] |
                     initialConditionStatement[boost::phoenix::bind(
                         &ptc::Statement::setStatementType, _val,
                         ptc::Statement::INITIAL_CONDITION)];

        macroStatement %=
            symbolName >> qi::lit("=") >
            expression[boost::phoenix::bind(&ptc::MacroStatement::addExpression,
                                            _val, _1)] >>
            qi::eps[boost::phoenix::bind(&ptc::Statement::setLineNumber, _val,
                                         &lineNumber)];

        derivativeStatement %=
            qi::lit("d") > symbolName > qi::lit("/dt") > qi::lit("=") >
            expression[boost::phoenix::bind(
                &ptc::DerivativeStatement::addExpression, _val, _1)] >>
            qi::eps[boost::phoenix::bind(&ptc::Statement::setLineNumber, _val,
                                         &lineNumber)];

        initialConditionStatement %=
            symbolName >> qi::lit("[t0]") > qi::lit("=") >
            expression[boost::phoenix::bind(
                &ptc::InitialConditionStatement::addExpression, _val, _1)] >>
            qi::eps[boost::phoenix::bind(&ptc::Statement::setLineNumber, _val,
                                         &lineNumber)];

        covariateStatement %=
            qi::lit("Covariate") > qi::lit("{") > symbolName > qi::lit(",") >
            expression[boost::phoenix::bind(
                &ptc::InitialConditionStatement::addExpression, _val, _1)] >
            qi::lit("}") >>
            qi::eps[boost::phoenix::bind(&ptc::Statement::setLineNumber, _val,
                                         &lineNumber)];

        // Has the form x[t] = 3*MAC0 - alpha, -INF <= t < -3
        // Right now forces upper and lower bounds.
        historyStatement %=
            symbolName >> qi::lit("[t]") > qi::lit("=") >
            expression[boost::phoenix::bind(&ptc::Statement::addExpression,
                                            _val, _1)] > qi::lit(",") >
            (parsedDouble | qi::string("-INF"))[boost::phoenix::bind(
                &ptc::Statement::setLowerHistoryBound, _val, _1)] >
            (qi::string("<=") | qi::string("<"))[boost::phoenix::bind(
                &ptc::Statement::setLowerBoundSign, _val, _1)] > qi::lit("t") >
            (qi::string("<=") | qi::string("<"))[boost::phoenix::bind(
                &ptc::Statement::setUpperBoundSign, _val, _1)] >
            (parsedDouble | qi::string("+INF") |
             qi::string("t0"))[boost::phoenix::bind(
                &ptc::Statement::setUpperHistoryBound, _val, _1)] >>
            qi::eps[boost::phoenix::bind(&ptc::Statement::setLineNumber, _val,
                                         &lineNumber)];

        expression %=
            -expression_prefix >>
            term[boost::phoenix::bind(&ptc::Expression::addTerm, _val, _1)] >>
            *((arithmetic_op | all_boolean_op)[boost::phoenix::bind(
                  &ptc::Expression::addOperator, _val, _1)] >
              term[boost::phoenix::bind(&ptc::Expression::addTerm, _val, _1)]);

        expressionWOBool %=
            -expression_prefix >>
            term[boost::phoenix::bind(&ptc::Expression::addTerm, _val, _1)] >>
            *((arithmetic_op)[boost::phoenix::bind(
                  &ptc::Expression::addOperator, _val, _1)] >
              term[boost::phoenix::bind(&ptc::Expression::addTerm, _val, _1)]);

        arithmetic_op %= qi::string("+") | qi::string("-") | qi::string("/") |
                         qi::string("^") | qi::string("*");

        expression_prefix %= qi::string("-");

        lg_boolean_op %= qi::string("<=") | qi::string(">=") | qi::string("<") |
                         qi::string(">");

        all_boolean_op %= lg_boolean_op | qi::string("==") | qi::string("||") |
                          qi::string("&&") | qi::string("!=");

        term %= indicatorFunction | timeVaryingFunction | initialCondition |
                delayedVar | qi::lit("(") > expression > qi::lit(")") |
                functionCall | symbolName | parsedDouble;
        //| qi::double_ [&ptc::Term::setOriginalString, _val, _1]);
        // Can't have constant, since it will cause problems with variables of
        // the form pi_X. So will be parsed as symbol name,
        //	    | constant
        //| symbolName);

        parsedDouble =
            qi::raw[qi::double_[at_c<0>(_val) = _1]]
                   [at_c<1>(_val) = construct<std::string>(begin(_1), end(_1))];

        indicatorFunction %=
            indicator >> qi::lit("(") >>
            expressionWOBool[boost::phoenix::bind(
                &ptc::IndicatorFunctionTerm::addExpression, _val, _1)] >>
            all_boolean_op[boost::phoenix::bind(
                &ptc::IndicatorFunctionTerm::addBoolean, _val, _1)] >>
            expressionWOBool[boost::phoenix::bind(
                &ptc::IndicatorFunctionTerm::addExpression, _val, _1)] >>
            -(all_boolean_op[boost::phoenix::bind(
                  &ptc::IndicatorFunctionTerm::addBoolean, _val, _1)] >>
              expressionWOBool[boost::phoenix::bind(
                  &ptc::IndicatorFunctionTerm::addExpression, _val, _1)]) >>
            qi::lit(")");

        delayedVar %= symbolName >> qi::lit("[") >>
                      expression[boost::phoenix::bind(
                          &ptc::DelayedVar::addExpression, _val, _1)] >>
                      qi::lit("]");

        initialCondition %= symbolName >> qi::lit("[t0]");

        functionCall %= symbolName >> qi::lit("(") >
                        -expressionList[boost::phoenix::bind(
                            &ptc::FunctionCall::addExpressionList, _val, _1)] >
                        qi::lit(")");

        timeVaryingFunction %=
            symbolName >> qi::lit("(") >>
            symbolName[boost::phoenix::bind(
                &ptc::TimeVaryingFunctionTerm::addSingleArgument, _val, _1)] >>
            *(qi::lit(",") >>
              (parsedDouble | symbolName)[boost::phoenix::bind(
                  &ptc::TimeVaryingFunctionTerm::addSingleArgument, _val, _1)])

            >> qi::lit(",") >> qi::lit("[") >>
            (parsedDouble | symbolName)[boost::phoenix::bind(
                &ptc::TimeVaryingFunctionTerm::addToLeftVector, _val, _1)] >>
            *(qi::lit(",") >>
              (parsedDouble | symbolName)[boost::phoenix::bind(
                  &ptc::TimeVaryingFunctionTerm::addToLeftVector, _val, _1)]) >>
            qi::lit("]") >> qi::lit(",") >> qi::lit("[") >>
            (parsedDouble | symbolName)[boost::phoenix::bind(
                &ptc::TimeVaryingFunctionTerm::addToRightVector, _val, _1)] >>
            *(qi::lit(",") >>
              (parsedDouble | symbolName)[boost::phoenix::bind(
                  &ptc::TimeVaryingFunctionTerm::addToRightVector, _val,
                  _1)]) >>
            qi::lit("]") >>
            -(qi::lit(",") >>
              (parsedDouble | symbolName)[boost::phoenix::bind(
                  &ptc::TimeVaryingFunctionTerm::setDefaultValue, _val, _1)]) >>
            qi::lit(")");

        expressionList = expression >> *(',' >> expression);

        symbolName2 %= (qi::char_("a-zA-Z_") >> *qi::char_("a-zA-Z_0-9"));

        symbolName %= symbolName2[&updateOccurence];

        //[&updateOccurence];

        constant %= qi::string("+INF") | qi::string("-INF") | qi::string("pi") |
                    qi::string("t0");

        indicator %= qi::string("ind");

        // qi::on_error<qi::accept>
        //     (
        //      initialConditionStatement,
        //      std::cerr << val("Error on line: ")
        //      << boost::phoenix::ref(lineNumber) << val(": Expecting ")
        //      << _4                               // what failed?
        //      << val(" here: \"")
        //      << construct<std::string>(qi::_3, qi::_2)   // iterators to
        //      error-pos, end
        //      << val("\"")
        //      << std::endl
        //      );

        boost::phoenix::function<XDEErrorHandler<Iterator> > myErrorHandler =
            XDEErrorHandler<Iterator>();

        // Really need a reason for all the names?
        start.name("start");
        expression.name("expression");
        initialConditionStatement.name("initialConditionStatement");
        derivativeStatement.name("derivativeStatement");
        macroStatement.name("macroStatement");
        historyStatement.name("historyStatement");
        covariateStatement.name("covariateStatement");
        term.name("term");
        expression.name("expression");
        functionCall.name("functionCall");
        //	qi::debug(start);

        // All error situations. Looks like val(lineNumber) always returns 0, so
        // bypassed in ErrorHandler class.

        qi::on_error<qi::fail>(
            start, myErrorHandler(it_begin, qi::_2, qi::_3, qi::_4,
                                  val(lineNumber), "Will not process further"));

        qi::on_error<qi::fail>(
            initialConditionStatement,
            myErrorHandler(it_begin, qi::_2, qi::_3, qi::_4, val(lineNumber),
                           "Badly defined initial condition statement"));

        qi::on_error<qi::fail>(
            macroStatement,
            myErrorHandler(it_begin, qi::_2, qi::_3, qi::_4, val(lineNumber),
                           "Badly defined macro statement"));
        qi::on_error<qi::fail>(
            derivativeStatement,
            myErrorHandler(it_begin, qi::_2, qi::_3, qi::_4, val(lineNumber),
                           "Badly defined derivative statement"));
        qi::on_error<qi::fail>(
            historyStatement,
            myErrorHandler(it_begin, qi::_2, qi::_3, qi::_4, val(lineNumber),
                           "Badly defined history statement"));
        qi::on_error<qi::fail>(
            covariateStatement,
            myErrorHandler(it_begin, qi::_2, qi::_3, qi::_4, val(lineNumber),
                           "Badly defined covariate statement"));

        qi::on_error<qi::fail>(
            term,
            myErrorHandler(it_begin, qi::_2, qi::_3, qi::_4, val(lineNumber),
                           "Badly defined expression: parenthesis missing?"));

        qi::on_error<qi::fail>(
            expression, myErrorHandler(it_begin, qi::_2, qi::_3, qi::_4,
                                       val(lineNumber), "Error in expression"));

        qi::on_error<qi::fail>(
            functionCall,
            myErrorHandler(it_begin, qi::_2, qi::_3, qi::_4, val(lineNumber),
                           "Error in function call"));
    }

    // Some general rules
    qi::rule<Iterator, ParsingResult *(), Skipper> start;

    qi::rule<Iterator, VectorOfStatements(), Skipper> listOfStatements;

    qi::rule<Iterator, ptc::Statement(), Skipper> statement;

    qi::rule<Iterator, ptc::Statement(), Skipper> macroStatement;
    qi::rule<Iterator, ptc::Statement(), Skipper> derivativeStatement;
    qi::rule<Iterator, ptc::Statement(), Skipper> initialConditionStatement;
    qi::rule<Iterator, ptc::Statement(), Skipper> historyStatement;
    qi::rule<Iterator, ptc::Statement(), Skipper> covariateStatement;

    qi::rule<Iterator, ptc::Expression(), Skipper> expression;
    qi::rule<Iterator, ptc::Expression(), Skipper> expressionWOBool;
    qi::rule<Iterator, ptc::DelayedVar(), Skipper> delayedVar;
    qi::rule<Iterator, ptc::InitialConditionTerm(), Skipper> initialCondition;
    qi::rule<Iterator, ptc::FunctionCall(), Skipper> functionCall;
    qi::rule<Iterator, ptc::TimeVaryingFunctionTerm(), Skipper>
        timeVaryingFunction;
    qi::rule<Iterator, ptc::IndicatorFunctionTerm(), Skipper> indicatorFunction;
    qi::rule<Iterator, ptc::Term(), Skipper> term;

    qi::rule<Iterator, ptc::VectorOfExpressions(), Skipper> expressionList;

    qi::rule<Iterator, string()> symbolName;
    qi::rule<Iterator, string()> symbolName2;
    qi::rule<Iterator, string()> constant;
    qi::rule<Iterator, string(), Skipper> arithmetic_op;
    qi::rule<Iterator, string(), Skipper> lg_boolean_op;
    qi::rule<Iterator, string(), Skipper> all_boolean_op;
    qi::rule<Iterator, string(), Skipper> indicator;
    qi::rule<Iterator, string(), Skipper> expression_prefix;

    qi::rule<Iterator, ptc::ParsedDouble(), Skipper> parsedDouble;

    function<CreateParsingResultFunction> CreateParsingResult;
};


ParsingResultPtr SpiritParser::parseEquations(const std::string &eqns1) {
    std::string eqns = eqns1 + "\n";
    boost::replace_all(eqns, "\\\n", " ");

    // TODO This is a ugly hack. See how to integrate to above class.
    lineNumber = 1;
    errorMessages.clear();
    occurenceIndex = 0;
    occurenceMap.clear();

    typedef boost::spirit::line_pos_iterator<std::string::const_iterator>
        iterator_type;
    // string::const_iterator iter = eqns.begin();
    // string::const_iterator end = eqns.end();
    iterator_type iter(eqns.begin());
    iterator_type end(eqns.end());

    typedef XDE_parser<iterator_type> XXXXDE_parser;
    typedef XDESkipper<iterator_type> XXXXDESkipper;

    XXXXDE_parser parser(iter);
    XXXXDESkipper skipper;

    ParsingResult *ret = NULL;  // = new ParsingResult();
    (void)phrase_parse(iter, end, parser, skipper, ret);
    // std::cerr << "Returned value: " << r << std::endl;
    // BOOST_FOREACH(ptc::StatementPtr s, ret -> getStatements()) {
    // 	std::cerr << "Returned statement with var: " << s -> getVarName()
    // 		  << " and expression: " << s -> getExpression()->toString()
    // 		  << " at line: " << s->getLineNumber()
    // 		  << std::endl;
    // }

    // Some quick hack here. If the error occurs in the first line
    // the ParsinResult is not created.
    if (ret == NULL) ret = new ParsingResult();
    ParsingResultPtr result(ret);
    result->setErrorMessages(errorMessages);
    result->setOccurenceMap(occurenceMap);
    return result;
}

//================================================================================
// Methods related to the ParsingResult class

void ParsingResult::addStatement(const ptc::Statement &s) {
    statements.push_back(ptc::StatementPtr(new ptc::Statement(s)));
}

ParsingResult::ParsingResult() {}
