#include "XDEParser.h"

#include <base/XDEException.h>
#include <expressions/MuParserExpressionHandler.h>
#include <expressions/TimeVaryingFunctionHandler.h>

#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>
#include <boost/math/constants/constants.hpp>
#include <iterator>

// Boost graph includes, used in sorting the macros, initial conditions.
#include <extra_parser_functions/NoFunction.h>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/exception.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/visitors.hpp>

using namespace std;

XDEParser::~XDEParser() {
    XDEMessage::log(XDEMessage::DEBUG4, "Into XDEParser destructor.");
}

/**
 * Parse the model.
 *
 */

void XDEParser::parseModel(string model) {
    if (model == "") return;
    parseTree = std::make_shared<ParseTree>(model);

    if (parseTree->getErrorMessages().size() > 0) {
        ErrorMessageList eml = parseTree->getErrorMessages();
        copy(eml.begin(), eml.end(), back_inserter(errorMessages));
        return;
    }

    if (parseTree->getDerivativeStatements().size() == 0) {
        errorMessages.push_back(ErrorMessageType(
            0, "Model needs at least one differential equation."));
        return;
    }

    extractSymbols();
    if (errorMessages.size() > 0) return;

    processStatements();
    if (errorMessages.size() > 0) return;

    processMacroCycles();
    if (errorMessages.size() > 0) return;

    buildMSV(dependentVariablesVector, dependentVariables);
    buildMSV(macrosVector, macros);
    buildMSV(modelParametersVector, modelParameters);
    buildMSV(covariatesVector, covariates);

    historyIntervals = extractHistory();
    delayedVariableHandler = createDelayedVariableObj();
    if (errorMessages.size() > 0) return;
}

/**
 * Go through the parse tree and extract all the symbols, and put them in the
 * appropriate lists: model parameters, dependent variables, or macros.
 */

void XDEParser::extractSymbols() {
    // First extract the derivatives
    for (const ptc::StatementPtr &st : parseTree->getDerivativeStatements()) {
        string varName = st->getVarName();
        unsigned lineNO = st->getLineNumber();
        if (dependentVariables.find(varName) != dependentVariables.end())
            errorMessages.push_back(ErrorMessageType(
                lineNO, "Variable " + varName +
                            " already defined as a dependent variable"));

        if (!validDependentVariable(varName))
            errorMessages.push_back(ErrorMessageType(
                lineNO, "Can not use " + varName + " as a dependent variable"));

        if (errorMessages.size() == 0) {
            dependentVariables[varName] =
                DependentVariablePtr(new DependentVariable(
                    varName, lineNO, parseTree->getOccurenceIndex(varName)));
            expressionHandler->addDependentVariable(varName);
        }
    }

    // Can validate now that the initial condition statements are valid.
    set<string> initCondsFound;
    for (const ptc::StatementPtr &st :
         parseTree->getInitialConditionStatements()) {
        string varName = st->getVarName();
        unsigned lineNO = st->getLineNumber();
        if (dependentVariables.find(varName) == dependentVariables.end()) {
            errorMessages.push_back(ErrorMessageType(
                lineNO, "Initial condition for  " + varName +
                            " does not refer to a dependent variable"));
        }
        if (initCondsFound.find(varName) != initCondsFound.end()) {
            errorMessages.push_back(ErrorMessageType(
                lineNO,
                "An initial condition has already been defined for variable " +
                    varName));
        }
        initCondsFound.insert(varName);
    }

    // Then extract the macros. Same procedure.
    for (const ptc::StatementPtr &st : parseTree->getMacroStatements()) {
        string varName = st->getVarName();
        unsigned lineNO = st->getLineNumber();
        if (macros.find(varName) != macros.end())
            errorMessages.push_back(ErrorMessageType(
                lineNO, "Variable " + varName + " already defined as a macro"));
        if (!validMacroName(varName))
            errorMessages.push_back(ErrorMessageType(
                lineNO, "Can not use " + varName + " as a macro"));
        macros[varName] = MacroPtr(
            new Macro(varName, lineNO, parseTree->getOccurenceIndex(varName)));
    }

    // Extracts the covariates.

    for (const ptc::StatementPtr &st : parseTree->getCovariateStatements()) {
        string varName = st->getVarName();
        unsigned lineNO = st->getLineNumber();
        if (covariates.find(varName) != covariates.end())
            errorMessages.push_back(ErrorMessageType(
                lineNO,
                "Variable " + varName + " already defined as a covariate"));
        if (!validMacroName(varName))  // Can change name of function later...
            errorMessages.push_back(ErrorMessageType(
                lineNO, "Can not use " + varName + " as a covariate"));

        // TODO AL2021: See if there is a cleaner solution. Can we do without a
        // function, or create a function that only returns 0. Probably not
        // needed in new version, since a function will be assigned from gui on
        // simulation.
        covariates[varName] = CovariatePtr(new Covariate(
            varName, lineNO, parseTree->getOccurenceIndex(varName),
            TimeVaryingFunctionPtr(new NoFunction())));
    }
}

/**
 * Go through all the statements, and create the muParser required statements.
 * Some side effects:
 * - Will extract the model parameters, and create the objects.
 */

void XDEParser::processStatements() {
    // Create expression for derivatives
    for (const ptc::StatementPtr &st : parseTree->getDerivativeStatements()) {
        DependentVariablePtr depVar = dependentVariables[st->getVarName()];
        depVar->assignDerivative(createProcessedModelExpression(st));
        addModelParameters(depVar->getDerivativeExpression(),
                           st->getLineNumber());
    }

    for (const ptc::StatementPtr &st :
         parseTree->getInitialConditionStatements()) {
        DependentVariablePtr depVar = dependentVariables[st->getVarName()];
        depVar->assignInitialCondition(createProcessedModelExpression(st));
        addModelParameters(depVar->getInitialConditionExpression(),
                           st->getLineNumber());
    }

    for (const ptc::StatementPtr &st : parseTree->getMacroStatements()) {
        string varName = st->getVarName();
        MacroPtr mac = macros[varName];
        mac->assignExpression(createProcessedModelExpression(st));
        addModelParameters(mac->getExpressionObject(), st->getLineNumber());
    }

    for (const ptc::StatementPtr &st : parseTree->getCovariateStatements()) {
        string varName = st->getVarName();
        CovariatePtr cov = covariates[varName];
        cov->assignExpression(createProcessedModelExpression(st));
        addModelParameters(cov->getExpressionObject(), st->getLineNumber());
        // Stuff related to original time-varying function.
        // Will be adding the default, which is locf.
        cov->setDisplayExpression(
            MuParserExpressionEvaluator::getTVFunctionString(
                cov->getTimeVaryingIndex(),
                cov->getExpressionObject()->getDisplayExpression()));
    }
}

ProcessedModelExpressionPtr XDEParser::createProcessedModelExpression(
    const ptc::StatementPtr &st) {
    return expressionHandler->generateCode(st->getExpression(),
                                           st->getLineNumber());
}

/**
 * Add the models parameters. At this point it is expected that all the
 * dependent variables and macros have been identified. Everything else, that
 * is not a constant (such as pi) is a model parameter.
 * Constants such as pi should not be returned bu getSymbolsUsed().
 */
void XDEParser::addModelParameters(ProcessedModelExpressionPtr exp,
                                   unsigned /*lineNO*/) {
    for (const string &s : exp->getSymbolsUsed()) {
        if (!isCovariate(s) && !isMacro(s) && !isDependentVariable(s) &&
           !MuParserExpressionHandler::isPredefinedVariable(s)) {
            // Check that it has not already be added
            if (!isModelParameter(s)) {
                modelParameters[s] = ModelParameterPtr(
                    new ModelParameter(s, parseTree->getOccurenceIndex(s)));
            }
        }
    }
}

bool XDEParser::isDependentVariable(const string &varName) {
    if (dependentVariables.find(varName) != dependentVariables.end())
        return true;
    return false;
}

bool XDEParser::isMacro(const string &varName) {
    if (macros.find(varName) != macros.end()) return true;
    return false;
}

bool XDEParser::isCovariate(const string &varName) {
    if (covariates.find(varName) != covariates.end()) return true;
    return false;
}

bool XDEParser::isModelParameter(const string &varName) {
    if (modelParameters.find(varName) != modelParameters.end()) return true;
    return false;
}

// Methods used for error checking by the parser.

/**
 * Validate the dependent variable
 *
 * Will validate a dependent variable. It can be anything that is not
 * a reserved word or already used as a macro.
 */

bool XDEParser::validDependentVariable(const string &varName) {
    if (  
        (!MuParserExpressionHandler::isWordReserved(varName)) &&
        !MuParserExpressionHandler::isPredefinedVariable(varName) &&
        macros.find(varName) == macros.end())
        return true;

    return false;
}

/**
 * Validate the macro name.
 *
 * Will validate a macro. It can be anything that is not
 * a reserved word or already used as a dependent variable.
 *
 * // TODO
 * Will return false in the case the macro already exists: either
 * change the behavior or the name.
 */

bool XDEParser::validMacroName(const string &varName) {
    if ((!MuParserExpressionHandler::isWordReserved(varName)) &&
        !MuParserExpressionHandler::isPredefinedVariable(varName) &&
        macros.find(varName) == macros.end() &&
        dependentVariables.find(varName) == dependentVariables.end() &&
        covariates.find(varName) == covariates.end())
        return true;
    return false;
}


set<string> getSymbolsUsed(ModelSymbolVariant v) {
    return boost::apply_visitor(UsedSymbolsVisitor(), v);
}

template <typename VisitedMap>
class my_bfs_visitor : public boost::default_bfs_visitor {
   public:
    my_bfs_visitor(VisitedMap v) : visited(v) {}

    template <typename Vertex, typename Graph>
    void discover_vertex(Vertex u, const Graph &) const {
        put(visited, u, true);
    }

   private:
    VisitedMap visited;
};

/**
 * Look for dependencies in the macros, initial conditions and covariates and
 * sort them accordingly.
 *
 * Will look at cycles in the macros and sort them in topological
 * order to ensure the the computation is valid. Will be performed by
 * building a directed graph where A -> B means  that A uses
 * B in order to complete its computation. After a topological sort
 * the macros need to be computed in order. Since we use a back
 * inserter, the resulting sort will list the vertices in the order
 * required to perform the computation.
 *
 */

void XDEParser::processMacroCycles() {
    unsigned nbDependentVariables = dependentVariables.size();

    vector<ModelSymbolVariant> symbolsInGraph;
    vector<string> allNames;

    for (const auto &p1 : dependentVariables) {
        allNames.push_back(p1.first);
        symbolsInGraph.push_back(p1.second);
    }

    for (const auto  &p2 : macros) {
        allNames.push_back(p2.first);
        symbolsInGraph.push_back(p2.second);
    }

    for (const auto &p2 : covariates) {
        allNames.push_back(p2.first);
        symbolsInGraph.push_back(p2.second);
    }

    typedef boost::adjacency_list<boost::mapS, boost::vecS, boost::directedS>
        Graph;

    // Create a graph.
    unsigned graphSize = symbolsInGraph.size();
    Graph graph(graphSize);
    for (unsigned i = 0; i < graphSize; i++) {
        set<string> symbolsUsed = getSymbolsUsed(symbolsInGraph[i]);
        for (const auto &s: symbolsUsed) {
            if (isMacro(s) || isDependentVariable(s) || isCovariate(s)) {
                vector<string>::iterator it =
                    find(allNames.begin(), allNames.end(), s);
                XDE_ASSERT(it != allNames.end());
                boost::add_edge(i, it - allNames.begin(), graph);
            }
        }
    }

    // Do nothing if there is no macros using another macro. Otherwise
    // apply topological sort.

    vector<unsigned> symbolsComputationOrder;
    if (boost::num_edges(graph) > 0) {
        try {
            boost::topological_sort(
                graph, std::back_inserter(symbolsComputationOrder));
        } catch (boost::not_a_dag) {
            // All this code goes to identify and report the cycle.
            XDEMessage::log(XDEMessage::XDE_ERROR,
                            "Cycle found among macros in the model.");
            // Only reason for the exception is a cycle in the graph. Now use
            // DFS to locate where the cycle is so the user gets a better
            // message.

            // Array to store predecessor (parent) of each vertex.
            // Code mostly adapted from the boost documentation
            typedef Graph::vertex_descriptor Vertex;
            std::vector<Vertex> p(boost::num_vertices(graph), -1);
            std::vector<Vertex> b(boost::num_vertices(graph), -1);

            boost::depth_first_search(
                graph,
                boost::visitor(boost::make_dfs_visitor(std::make_pair(
                    boost::record_predecessors(&p[0], boost::on_tree_edge()),
                    boost::record_predecessors(&b[0],
                                               boost::on_back_edge())))));

            // Identify the cycle. First find a back edge.
            std::vector<Vertex>::iterator it =
                std::find_if(b.begin(), b.end(), bind2nd(greater<int>(), -1));

            vector<string> cycle;
            unsigned source_vertex = it - b.begin();
            // cycle.push_back(macros[source_vertex]->getName());
            cycle.push_back(allNames[source_vertex]);
            unsigned next_vertex = *it;
            while (next_vertex != source_vertex) {
                // cycle.push_back(macros[next_vertex]->getName());
                cycle.push_back(allNames[next_vertex]);
                next_vertex = p[next_vertex];
            }

            string returnedCycle;
            for (const auto &s : cycle) {
                returnedCycle.append(s + " <- ");
            }
            returnedCycle.append(cycle[0]);

            errorMessages.push_back(ErrorMessageType(
                0,
                string("Cycle found among macros in the model. Cycle is: (A <- "
                       "B means A is required to compute B)\n")
                    .append(returnedCycle)));
        }
    } else {
        // Empty graph
        unsigned s = allNames.size();
        symbolsComputationOrder.resize(s);
        for (unsigned i = 0; i < s; i++) {
            symbolsComputationOrder[i] = i;
        }
    }

    // Determine which macros are time-dependent, and check if any
    // initial condition uses (directly or indirectly) time-dependent
    // variables.

    for (const unsigned &i : symbolsComputationOrder) {
        if (isTimeDependent(i, symbolsInGraph)) {
            if (isMacro(allNames[i])) {
                macros[allNames[i]]->setTimeDependent(true);
            } else if (isDependentVariable(
                           allNames[i])) {  // Initial condition. Throw
                // error if time dependent
                errorMessages.push_back(ErrorMessageType(
                    0, "Error: initial condition for variable " + allNames[i] +
                           " contains time dependencies."));
            }
        }
    }

    // Determine which macros are used, directly or indirectly, by
    // initial conditions. Computed by performing a BFS on all the
    // initial condition, and flagging all the macros that are
    // encountered.

    // Dependent variables are stored first in the graph.

    // TODO This does not look right. Will probably work.
    //    typedef boost::graph_traits < Graph >::vertex_descriptor Vertex;
    typedef boost::graph_traits<Graph>::vertices_size_type Size;

    // TODO
    // Will work, but not sure about the use of Size here. Really want a bool,
    // that will be set to true when a node is visited. Adapted from code at:
    // http://www.boost.org/doc/libs/1_44_0/libs/graph/example/bfs-example.cpp

    vector<Size> visited(graphSize, false);
    my_bfs_visitor<Size *> vis(&visited[0]);

    for (unsigned i = 0; i < nbDependentVariables; i++) {
        boost::breadth_first_search(graph, boost::vertex(i, graph),
                                    visitor(vis));
    }

    for (unsigned i = nbDependentVariables; i < visited.size(); i++) {
        if (visited[i]) {
            macros[allNames[i]]->setUsedInInitialCondition(true);
        }
    }

    // Fill the array containing the symbols (initConds, macros) in order in
    // which they must be computed
    orderOfComputation.clear();
    orderOfComputation.reserve(graphSize);
    for (const unsigned &u : symbolsComputationOrder) {
        orderOfComputation.push_back(allNames[u]);
    }
}

bool XDEParser::isTimeDependent(unsigned i,
                                const vector<ModelSymbolVariant> &v) {
    set<string> symbolsUsed = getSymbolsUsed(v[i]);
    for (string s : symbolsUsed) {
        if (s == "t" || isDependentVariable(s)) return true;
        if (isMacro(s) && macros[s]->isTimeDependent()) {
            return true;
        }
    }
    return false;
}

HistoryIntervalsPtr<std::string, int> XDEParser::extractHistory() {
    std::multimap<std::string, HistoryElement> historyIntervalsMap;
    for (const ptc::StatementPtr &st : parseTree->getHistoryStatements()) {
        string varName = st->getVarName();
        unsigned lineNO = st->getLineNumber();

        if (dependentVariables.find(varName) == dependentVariables.end()) {
            errorMessages.push_back(ErrorMessageType(
                lineNO, "History function for  " + varName +
                            " does not refer to a dependent variable"));
        }
        ProcessedModelExpressionPtr pme = createProcessedModelExpression(st);
        addModelParameters(pme, st->getLineNumber());

        // The upper bound could be t0. Since no other history
        // item will define a time > t0 we can set t0 to +INF
        // without affecting the computation.
        // Same applies for tend
        ParseTreeComponents::HistoryBoundVariant ub =
            st->getUpperHistoryBound();
        ParseTreeComponents::HistoryBoundVariant lb =
            st->getLowerHistoryBound();

        double lower =
            boost::apply_visitor(ParseTreeComponents::BoundVisitor(), lb);
        double upper =
            boost::apply_visitor(ParseTreeComponents::BoundVisitor(), ub);

        auto eqID =
            expressionHandler->addExpression(pme->getTransformedExpression());

        historyIntervalsMap.insert(pair<string, HistoryElement>(
            varName,
            pair(HistoryInterval(lower, upper,
                                 st->getLowerBoundSign() == "<" ? false : true,
                                 st->getUpperBoundSign() == "<" ? false : true),
                 eqID)));
    }

    std::vector<string> depVarNames(dependentVariablesVector.size());
    std::transform(
        dependentVariablesVector.begin(), dependentVariablesVector.end(),
        depVarNames.begin(),
        [](DependentVariablePtr p) -> std::string { return p->getName(); });

    return std::make_shared<HistoryIntervals<string, int>>(
        depVarNames, historyIntervalsMap);
}

DelayedVariableHandlerPtr<unsigned> XDEParser::createDelayedVariableObj() {

    const auto &delayExpressions = expressionHandler->getDelayedVariableExpressions();
    vector<unsigned> ids(delayExpressions.size());
    std::transform(delayExpressions.begin(), delayExpressions.end(), ids.begin(), [](pair<string, unsigned> p)->unsigned{return p.second;});
    
    const auto &delayedVariables = expressionHandler -> getDelayedVariables();
    vector<pair<string, unsigned>> delayVarsMapping(delayedVariables.size());
    std::transform(delayedVariables.begin(), delayedVariables.end(), 
        delayVarsMapping.begin(), [](DelayedVariablePtr p)->pair<string, unsigned>{return std::pair(p->getVarName(), p->getExpressionID());});

    return std::make_shared<DelayedVariableHandler<unsigned>>  (getAllNames(dependentVariablesVector), delayVarsMapping, ids);

}
