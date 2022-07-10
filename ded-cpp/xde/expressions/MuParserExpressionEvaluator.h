#ifndef MUPARSEREXPRESSIONEVALUATOR_H_
#define MUPARSEREXPRESSIONEVALUATOR_H_

#include <expressions/MemorySynchronizer.h>
#include <extra_parser_functions/TimeVaryingFunctionList.h>
#include <muParser.h>

#include <string>

class MuParserExpressionEvaluator;

typedef std::shared_ptr<MuParserExpressionEvaluator>
    MuParserExpressionEvaluatorPtr;

/**
 * \brief Class to hold a muParser expression (which can span many lines) and
 * evaluate it.
 *
 * This class will contain a muParser expression and a method to
 * evaluate the expression. It includes facilities to add function to
 * the MuParser default set of predefined equations.
 *
 * TODO: This class should probably extend a virtual class ExpressionEvaluator.
 */

class MuParserExpressionEvaluator {
   public:
    /**
     * Define type for operators so we can perform integer comparison
     * in the code instead of string comparisons.
     *
     */
    static double Indicator2(double lhs, double op, double rhs);
    static double Indicator3(double lhs, double op1, double middle, double op2,
                             double rhs);
    static double TVFunction(double index, double t);
    static double MuParserObject(double index);

    static double boost_gamma_p(double a, double z);
    static double Piecewise(const double *args, int nbArgs);
    static double xde_if(double arg1, double arg2, double arg3);

    static std::string getIndicatorString(std::string lhs, std::string op,
                                          std::string rhs);
    static std::string getIndicatorString(std::string lhs, std::string op1,
                                          std::string middle, std::string op2,
                                          std::string rhs);
    static std::string getTVFunctionString(unsigned index,
                                           std::string variable);
    static std::string getVarargsFunctionString(unsigned index);
    static std::string getMuParserFunctionString(unsigned index);

    ~MuParserExpressionEvaluator();
    MuParserExpressionEvaluator(std::string expression,
                                MemorySynchronizerPtr memorySynchronizer);

    double evaluate();

    const auto &getExpression() const { return expression; }
    const auto  &getVariableNames() const {return variableNames;};

    static bool isFunctionDefined(std::string s) {
        return additionalFunctions.find(s) != additionalFunctions.end();
    }

    void remapMemory();

   private:
    // Disallow copy constructors for this class
    MuParserExpressionEvaluator(const MuParserExpressionEvaluator &);
    mu::Parser parser;  // This is the muParser.
    
    std::vector<std::string> variableNames;
    std::string expression;
    MemorySynchronizerPtr memorySynchronizer;

    double *varLocations;

    int getLocation(const std::string &var);
    static const std::string TVFunctionName;
    static const std::string VarargsFunctionName;
    static const std::string MuParserFunctionName;
    // Indicates the functions that are availables to the user. ind(), gamma_p,
    // etc which are not provided by a subclass or varargs.
    static std::set<std::string> additionalFunctions;
};

#endif /*MUPARSEREXPRESSIONEVALUATOR_H_*/
