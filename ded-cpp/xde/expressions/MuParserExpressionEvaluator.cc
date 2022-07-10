#include "expressions/MuParserExpressionEvaluator.h"

#include <base/XDEException.h>
#include <base/XDEMessage.h>
#include <base/XDEUtil.h>

#include <boost/cast.hpp>
#include <boost/math/special_functions/gamma.hpp>
#include <cmath>
#include <map>
#include <sstream>

#include "MuParserObjectsList.h"
#include "muParser.h"
#include "muParserDef.h"

using namespace std;
static double PI = acos(-1.0);

typedef enum { EQ = 0, LT, GT, LEQ, GEQ, NEQ } OperatorType;
static const std::map<std::string, OperatorType> operatorMap = {{"=", EQ},{"<", LT}, {"<=", LEQ}, {">", GT}, {">=", GEQ}, {"<>", NEQ}};
static const OperatorType &getOperatorCode(string op) {
       if (!operatorMap.count(op))
        throw XDEException(string("Unrecognized operator: ").append(op));
    return operatorMap.at(op);
}


const string MuParserExpressionEvaluator::TVFunctionName = "___TVFunction";

// const string MuParserExpressionEvaluator::VarargsFunctionName =
// "___VarargsFunction";
const string MuParserExpressionEvaluator::MuParserFunctionName =
    "___MuParserObject";


template <>
double MuParserObjectsList::compute(unsigned indx) {
    return functions[indx]->compute();
}

std::set<std::string> MuParserExpressionEvaluator::additionalFunctions = {"ind", "gamma_p", "piecewise", "if"};

MuParserExpressionEvaluator::~MuParserExpressionEvaluator() {
    XDEMessage::log(XDEMessage::DEBUG2,
                    "Into MuParserExpressionEvaluator destructor");
}

MuParserExpressionEvaluator::MuParserExpressionEvaluator(
    string expr, MemorySynchronizerPtr mem)
    : expression(expr), memorySynchronizer(mem) {
    try {
        parser.SetExpr(expr);
        parser.DefineFun("Indicator3", MuParserExpressionEvaluator::Indicator3,
                         false);
        parser.DefineFun("Indicator2", MuParserExpressionEvaluator::Indicator2,
                         false);
        parser.DefineFun("gamma_p", MuParserExpressionEvaluator::boost_gamma_p,
                         false);
        parser.DefineFun("piecewise", MuParserExpressionEvaluator::Piecewise,
                         false);
        parser.DefineFun("if", MuParserExpressionEvaluator::xde_if, false);

        parser.DefineFun(TVFunctionName,
                         MuParserExpressionEvaluator::TVFunction, false);
        // parser.DefineFun(VarargsFunctionName,
        // MuParserExpressionEvaluator::VarargsFunction, false);
        parser.DefineFun(MuParserFunctionName,
                         MuParserExpressionEvaluator::MuParserObject, false);
        parser.DefineConst("INF", XDEUtil::XDE_INF);
        parser.DefineConst("pi", PI);
        parser.DefineConst("NaN", XDEUtil::XDE_NAN);
        mu::varmap_type varmap = parser.GetUsedVar();
        mu::varmap_type::const_iterator it = varmap.begin();
        while (it != varmap.end()) {
            variableNames.push_back(it->first);
            it++;
        }

        // Assign memory to the variables.
        for (unsigned i = 0; i < variableNames.size(); i++) {
            parser.DefineVar(variableNames[i],
                             memorySynchronizer->getLocation(variableNames[i]));
        }
    } catch (mu::ParserError &e) {
        XDEMessage::log(XDEMessage::XDE_ERROR, e.GetMsg());
        throw XDEException(e.GetMsg());
    }
}

/**
 * Method called when the mapping of the underlying memory has changed.
 */

void MuParserExpressionEvaluator::remapMemory() {
    for (const auto &v : variableNames) {
        parser.DefineVar(v, memorySynchronizer->getLocation(v));
    }
}

double MuParserExpressionEvaluator::evaluate() {
    // mu::varmap_type variables = parser.GetVar();

    // // Get the number of variables
    // mu::varmap_type::const_iterator item = variables.begin();
    return parser.Eval();
}

//========================= Private methods ==================================

// Return the location of a var. A map would be more elegant, but since
// an expression is likely to contain only a few variables this is
// probably faster to both execute and write.
int MuParserExpressionEvaluator::getLocation(const std::string &var) {
    for (unsigned i = 0; i < variableNames.size(); i++)
        if (variableNames[i] == var) return i;

    throw runtime_error("Undefined variable");
}

// Static methods used for indicator function.
// Must return double because they will be called from muparser.

double MuParserExpressionEvaluator::Indicator3(double lhs, double op1,
                                               double middle, double op2,
                                               double rhs) {
    if ((int)(Indicator2(lhs, op1, middle) + Indicator2(middle, op2, rhs)) == 2)
        return 1;
    return 0;
}

double MuParserExpressionEvaluator::Indicator2(double lhs, double op,
                                               double rhs) {
    OperatorType iop = (OperatorType)((int)op);

    if (lhs > rhs && (iop == GEQ || iop == GT || iop == NEQ)) return 1;
    if (lhs == rhs && (iop == LEQ || iop == GEQ || iop == EQ)) return 1;
    if (lhs < rhs && (iop == LEQ || iop == LT || iop == NEQ)) return 1;
    return 0;
}

double MuParserExpressionEvaluator::TVFunction(double index, double t) {
    unsigned unsignedIndex = boost::numeric_cast<unsigned>(index);
    XDE_ASSERT(fabs(index - unsignedIndex) < 0.5);
    return TimeVaryingFunctionList::getInstance()->compute(unsignedIndex, t);
}

double MuParserExpressionEvaluator::MuParserObject(double index) {
    unsigned unsignedIndex = boost::numeric_cast<unsigned>(index);
    XDE_ASSERT(fabs(index - unsignedIndex) < 0.5);
    return MuParserObjectsList::getInstance()->compute(unsignedIndex);
}

/**
 * Create a string that will substitute what the parser sees as an
 * indicator for what the expression evaluator will use. Here it will
 * be
 *	Indicator2(lhs, op, rhs)
 */

string MuParserExpressionEvaluator::getIndicatorString(string lhs, string op,
                                                       string rhs) {
    stringstream s;
    s << "Indicator2(" << lhs << ", " << getOperatorCode(op) << ", " << rhs
      << ")";
    return s.str();
}

string MuParserExpressionEvaluator::getIndicatorString(string lhs, string op1,
                                                       string middle,
                                                       string op2, string rhs) {
    stringstream s;
    if (op2 == "" && rhs == "") return getIndicatorString(lhs, op1, middle);
    if (op2 == "" || rhs == "") throw XDEException("Badly formed expression");

    s << "Indicator3(" << lhs << ", " << getOperatorCode(op1) << ", " << middle
      << ", " << getOperatorCode(op2) << ", " << rhs << ")";
    return s.str();
}

string MuParserExpressionEvaluator::getTVFunctionString(unsigned index,
                                                        std::string variable) {
    string ret(TVFunctionName);
    ret.append("(")
        .append(boost::lexical_cast<string>(index))
        .append(", ")
        .append(variable)
        .append(")");
    return ret;
}

std::string MuParserExpressionEvaluator::getMuParserFunctionString(
    unsigned index) {
    string ret(MuParserFunctionName);
    ret.append("(").append(boost::lexical_cast<string>(index)).append(")");
    return ret;
}


// TODO: Move these functions to a separate class, as their number is very
// likely to increase.

double MuParserExpressionEvaluator::boost_gamma_p(double a, double z) {
    if (a <= 0 || z < 0) return XDEUtil::XDE_NAN;
    return boost::math::gamma_p(a, z);
}

double MuParserExpressionEvaluator::Piecewise(const double *args, int n) {
    if (n == 0) return 0;
    for (int i = 0; i < n; i += 2) {
        // TODO Better comparison to 0. There's some macro somewhere...

        if (i == n - 1 || args[i + 1] != 0) {
            return args[i];
        }
    }
    return 0;
}

double MuParserExpressionEvaluator::xde_if(double arg1, double arg2,
                                           double arg3) {
    // Handles round-offs.
    if (arg1 < 0.1 && arg1 > (-0.1)) return arg3;
    return arg2;
}
