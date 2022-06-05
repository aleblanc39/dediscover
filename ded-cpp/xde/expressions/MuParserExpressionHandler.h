#ifndef MUPARSEREXPRESSIONHANDLER_H
#define MUPARSEREXPRESSIONHANDLER_H

#include <base/MemoryBank.h>
#include <expressions/ExpressionHandler.h>
#include <expressions/MuParserExpressionEvaluator.h>
#include <extra_parser_functions/TimeVaryingFunction.h>

#include <set>
#include <string>
#include <vector>

namespace ptc = ParseTreeComponents;
class MuParserExpressionHandler;
typedef std::shared_ptr<MuParserExpressionHandler>
    MuParserExpressionHandlerPtr;
/**
 * \brief Generates expressions from the model that can be executed by MuParser.
 *
 *
 */

class MuParserExpressionHandler : public ExpressionHandler {
   public:
    class TermMuGenerator;
    MuParserExpressionHandler();
    virtual ~MuParserExpressionHandler();

    int addExpression(std::string expression);
    double compute(unsigned equationNO) const;

    static double compute2(int equationNO,  MuParserExpressionHandlerPtr ex) {
        return ex -> compute(equationNO);
    }

    std::vector<std::string> getExpressions();
    std::string getExpression(unsigned equationNO);

    void setVariableValue(std::string symbol, double value);
    double getVariableValue(std::string symbol);

    // Should the next 3 be private?

    ProcessedModelExpressionPtr generateCode(
        const ParseTreeComponents::ExpressionPtr expression, int lineNO = -1);
    static std::string generateCode(
        const ParseTreeComponents::TermPtr expression,
        // ProcessedModelExpressionPtr ptr, 
        TermMuGenerator &g);
    static std::string generateCode(const ptc::ExpressionPtr expression,
                                    ProcessedModelExpressionPtr ptr,
                                    TermMuGenerator &generator);


    static bool isFunctionDefined(std::string fName);
    static bool isWordReserved(const std::string &w);
    static bool isKeyword(const std::string &w);
    static bool isPredefinedVariable(const std::string &w);

    void remapMemory();
    std::map<std::string, unsigned> delayedVariableExpressions;
    DelayedVariablePtr addDelayedVariable(const std::string &varName,
                                          const std::string &expressionCode);
    std::vector<DelayedVariablePtr> getDelayedVariables() {
        return delayedVariables;
    }

    std::map<std::string, unsigned> getDelayedVariableExpressions() {
        return delayedVariableExpressions;
    }

    void insertParsedTVFunction(unsigned u) { parsedTVFunctions.insert(u); }
    void resetParsedTVFunctions();
    std::set<unsigned> parsedTVFunctions;
    std::set<unsigned> getParsedTVFunctions() { return parsedTVFunctions; }

    static bool isDouble(std::string s);
    int getLineNumber() { return lineNumber; }

    // Visitor class to generate code understandable by muParser from the parsed
    // by XDE.
    class TermMuGenerator : public boost::static_visitor<std::string> {
       public:
        TermMuGenerator(ProcessedModelExpressionPtr ptr,
                        MuParserExpressionHandler *e)
            : pme(ptr), expressionHandler(e) {}

        std::string operator()(double &d) const;
        std::string operator()(std::string &p) const;
        std::string operator()(ptc::ParsedDoublePtr &p) const;
        std::string operator()(ptc::DelayedVarPtr &p);
        std::string operator()(ptc::ExpressionPtr &e);
        std::string operator()(ptc::FunctionCallPtr &f);
        std::string operator()(ptc::InitialConditionTermPtr &f);
        std::string operator()(ptc::TimeVaryingFunctionTermPtr &f);
        std::string operator()(ptc::IndicatorFunctionTermPtr &f);

       private:
        ProcessedModelExpressionPtr pme;
        void addSymbolUsed(std::string v);
        TimeVaryingEntry getTimeVaryingEntry(const std::string s);

       public:  // Put here to prevent warnings about order
        MuParserExpressionHandler *expressionHandler;
    };

    std::vector<DelayedVariablePtr> delayedVariables;

   private:
    MemoryBank memoryBank;
    int lineNumber;
    static std::set<std::string> functionNames;
    static std::set<std::string> keywordNames;
    static std::set<std::string> tvFunctionNames;
    static std::set<std::string> predefinedVariables;


    std::vector<MuParserExpressionEvaluator *> evaluators;
};


#endif
