package org.woodhill.ded.util

import org.woodhill.ded.models.*
import org.woodhill.ded.properties.KPlotProperties
import org.woodhill.ded.properties.KTabProperties
import org.woodhill.ded.results.EstimatedRow
import org.woodhill.ded.results.FixedRow
import org.woodhill.ded.results.JSONResultMatrix
import org.woodhill.ded.results.EstimationCIResults
import org.woodhill.ded.ui.chartoptions.KChartableProperties
import org.woodhill.ded.ui.chartoptions.ParameterChartOptions
import java.sql.PreparedStatement
import kotlin.reflect.KClass
import kotlin.reflect.full.primaryConstructor

data class DBModel(val modelID: Int, val name: String, val pkg: String, val editable:Boolean)

class DBLoadMethods private constructor(DB_URL: String) {

    val connection = DBConnection()
    fun getConnection() = connection.getConnection()

    // Quick hack for now: read db location from the environment, tell I figure out an easy way to implement
    // injection in here.
    init {
    }

    companion object {
        val instance = DBLoadMethods("XYZ")
    }

    private val loadedModels = mutableMapOf<Int, KModel>()
    private val mappingInfoObjects = mutableMapOf<Int, KMappingInfo>()

    private val dependentVariablesForModel = mutableMapOf<KModel, MutableList<EstimableSymbol>>()
    private val modelParametersForModel = mutableMapOf<KModel, MutableList<EstimableSymbol>>()
    private val macrosForModel = mutableMapOf<KModel, MutableList<KMacro>>()
    private val covariatesForModel = mutableMapOf<KModel, MutableList<KCovariate>>()
    private val parameterSetsForModel = mutableMapOf<KModel, MutableMap<String, KParameterSet>>()
    private val solverResultsForModel = mutableMapOf<KModel, MutableList<KSolverResult>>()

    private val estimationResultsForParamSet = mutableMapOf<KParameterSet, EstimationCIResults>()

    private val tvFunctionMappings = mutableMapOf<KCovariate, TVFunctionMapping>()
    //private val tvFunctionMappingsForModel = mutableMapOf<KModel, List<TVFunctionMapping>>()

    private val tabProperties = mutableMapOf<Pair<KModel, String>, KTabProperties>()

    fun removeModelInfo(model: KModel) {
        dependentVariablesForModel.remove(model)
        modelParametersForModel.remove(model)
        macrosForModel.remove(model)
        covariatesForModel.remove(model)
        parameterSetsForModel.remove(model)
        solverResultsForModel.remove(model)

        loadedModels.remove(model.modelID)

        // TODO Clear tab properties as well

    }


    fun getAllPackages(): List<ModelPackage> {
        val allPackages = mutableListOf<ModelPackage>()

        getConnection().use {
            val pstmt = it.prepareStatement("SELECT id, package, editable, expandable FROM package")
            val rs = pstmt.executeQuery()
            while (rs.next()) {
                allPackages.add(
                    ModelPackage(
                        rs.getInt(1),
                        rs.getString(2),
                        editable = rs.getBoolean(3),
                        expandable = rs.getBoolean(4)
                    )
                )
            }
            return allPackages
        }
    }

    fun getAllModelsWithPackage(): List<DBModel> {
        val allModels = mutableListOf<DBModel>()
        val conn = getConnection()

        conn.use {
            val pstmt =
                conn.prepareStatement("SELECT m.id, m.name, p.package, m.editable FROM model m, package p WHERE m.package = p.id")
            val rs = pstmt.executeQuery()
            while (rs.next()) {
                allModels.add(DBModel(rs.getInt(1), rs.getString(2), rs.getString(3), rs.getBoolean(4)))
            }
        }
        return allModels
    }

    fun loadMacros(model: KModel): List<KMacro> {

        if (macrosForModel.containsKey(model)) {
            return macrosForModel[model]!!
        }

        getConnection().use {
            val ret = mutableListOf<KMacro>()
            val pstmt =
                it.prepareStatement("select id, name, formula, description, occurenceIndex from macro where model = ?")
            pstmt.setInt(1, model.modelID)
            val rs = pstmt.executeQuery()
            while (rs.next()) {
                ret.add(
                    KMacro(
                        model,
                        rs.getInt(1),
                        rs.getString(2),
                        rs.getString(3) ?: "",
                        rs.getString(4)
                    )
                )
            }
            macrosForModel[model] = ret
            return ret
        }
    }


    fun loadCovariates(model: KModel): List<KCovariate> {

        if (covariatesForModel.containsKey(model)) {
            return covariatesForModel[model]!!
        }

        getConnection().use {
            val ret = mutableListOf<KCovariate>()
            val pstmt =
                it.prepareStatement("select id, name, timeexpression, description, occurenceIndex from covariate where model = ?")

            pstmt.setInt(1, model.modelID)
            val rs = pstmt.executeQuery()
            while (rs.next()) {
                ret.add(
                    KCovariate(
                        model,
                        rs.getInt(1),
                        rs.getString(2),
                        rs.getString(3),
                        rs.getString(4)
                    )
                )
            }
            covariatesForModel[model] = ret
            return ret
        }
    }

    /**
     * Load all the mapping info from a model based on a mapping id. Used in case we want to access a mapping
     * before they have been loaded.
     */
    private fun dbLoadMappingInfo(mappingID: Int): KMappingInfo {
        if (!mappingInfoObjects.containsKey(mappingID)) {
            getConnection().use {
                val pstmt = it.prepareStatement("select model from table_column where id = ?")
                pstmt.setInt(1, mappingID)
                val rs = pstmt.executeQuery()
                val model = rs.getInt(1)
                mappingInfoForModel(loadedModels[model]!!)
            }
        }
        return mappingInfoObjects[mappingID]!!
    }

    // TODO Don't cache this.
    // TODO Clean up. This can also be called just to populate mappingInfoObjects

    fun mappingInfoForModel(model: KModel): Map<String, KMappingInfo> {

        val objectsForModel = mutableMapOf<String, KMappingInfo>()
        getConnection().use {
            val pstmt = it.prepareStatement(
                "select tc.id, tc.column_name, tc.column_index, tc.status,  tc.used_for, tc.map_to, tc.formula, tc.description, tc.failure_message," +
                        "tc.data  " +
                        "from table_column tc, model m where tc.model = m.id and m.id=?"
            )
            pstmt.setInt(1, model.modelID)
            val rs = pstmt.executeQuery()
            while (rs.next()) {
                var i = 1
                val mappingInfoObj = KMappingInfo(
                    rs.getInt(i++),
                    rs.getString(i++),
                    rs.getInt(i++),
                    KMappingInfo.MappingStatus.valueOf(rs.getInt(i++))!!,
                    KMappingInfo.MappingType.valueOf(rs.getString(i++)),
                    rs.getString(i++),
                    rs.getString(i++),
                    rs.getString(i++),
                    rs.getString(i++),
                    rs.getString(i)
                )
                objectsForModel[rs.getString(2)] = mappingInfoObj
                mappingInfoObjects[rs.getInt(1)] = mappingInfoObj
            }
        }
        return objectsForModel
    }


    fun getTimepoints(timepoints: String): List<Double> {
        val dblTimepoints = mutableListOf<Double>()
        timepoints.trim().split(" ").forEach {
            if (!it.equals(""))
                dblTimepoints.add(it.toDouble())
        }
        return dblTimepoints
    }


    private fun getParamType(model: KModel, symbol: String): ParameterChartOptions.ParamType {
        if (symbol.endsWith(".DATA"))
            return ParameterChartOptions.ParamType.DATA
        if (symbol.endsWith("_MEAN"))
            return ParameterChartOptions.ParamType.REPLICATE_MEAN
        if (loadCovariates(model).map { it.name }.contains(symbol))
            return ParameterChartOptions.ParamType.COVARIATE
        if (loadMacros(model).map { it.name }.contains(symbol))
            return ParameterChartOptions.ParamType.MACRO
        if (loadDependentVariables(model).map { it.name }.contains(symbol))
            return ParameterChartOptions.ParamType.VARIABLE

        return ParameterChartOptions.ParamType.OTHER

    }


    fun loadChartableProperties(model: KModel, chartableProperties: KChartableProperties) {

        getConnection().use {
            val pstmt =
                it.prepareStatement("select symbol, visible, log, style from chartable_properties where model = ?")
            pstmt.setInt(1, model.modelID)
            val rs = pstmt.executeQuery()
            while (rs.next()) {
                chartableProperties.addChartableProperty(
                    rs.getString(1),
                    ParameterChartOptions(
                        rs.getString(1),
                        rs.getBoolean(2),
                        rs.getBoolean(3),
                        ParameterChartOptions.DisplayStyle.valueOf(rs.getString(4)),
                        //ParameterChartOptions.ParamType.OTHER
                        getParamType(model, rs.getString(1))
                    )
                )
            }
        }
    }



    fun loadPlotPropertiesFromDB(modelID: Int, chartVar: String): KPlotProperties {
        val query = "select key, value from plot_properties where model=? and chartVar=?"
        val ret = KPlotProperties()
        getConnection().use {
            val pstmt = it.prepareStatement(query)
            pstmt.setInt(1, modelID)
            pstmt.setString(2, chartVar)
            val rs = pstmt.executeQuery()
            while (rs.next()) {
                ret.put(rs.getString(1), rs.getString(2))
            }
        }
        return ret
    }

    fun <T : KTabProperties> loadTabProperties(model: KModel, tabclass: KClass<T>): T {
        val tab = tabclass.primaryConstructor!!.call(model)
        if (tabProperties.containsKey(Pair(model, tab.tabName())))
            return tabProperties[Pair(model, tab.tabName())] as T
        if (model.modelID >= 0) {
            val query = "select key, value from tab_properties where model=? and tab=?"
            getConnection().use {
                val pstmt = it.prepareStatement(query)
                pstmt.setInt(1, model.modelID)
                pstmt.setString(2, tab.tabName())
                val rs = pstmt.executeQuery()
                while (rs.next()) {
                    tab.put(rs.getString(1), rs.getString(2))
                }
            }
            tab.initializeProperties()
        }
        tabProperties[Pair(model, tab.tabName())] = tab
        return tab
    }

////============================================================

    // The database will use the same table for Dependent Variables and Model Parameters. There is a "type" field,
    // contains either MP or DV
    private val modelSymbolValuesQuery = "select ms.name, p.id,  p.value, p.min, p.max, p.sliders, p.estimated " +
            "from estimable_symbol ms, estimable_symbol_value p where  p.parameterset = ? and p.modelsymbol = ms.id and ms.type = ?"

    fun loadInitialConditionsValues(paramSet: KParameterSet): List<EstimableParameterValue> {
        val initialConditions = loadDependentVariables(paramSet.model)
        getConnection().use {
            val pstmt = it.prepareStatement(modelSymbolValuesQuery)
            pstmt.setInt(1, paramSet.id)
            pstmt.setString(2, "DV")
            return readModelSybolsValues(pstmt, initialConditions, paramSet)
        }
    }


    private fun readModelSybolsValues(
        pstmt: PreparedStatement,
        modelSymbols: List<EstimableSymbol>,
        paramSet: KParameterSet
    ): List<EstimableParameterValue> {
        val rs = pstmt.executeQuery()
        val ret = mutableListOf<EstimableParameterValue>()
        while (rs.next()) {
            ret.add(
                EstimableParameterValue(
                    rs.getInt(2),
                    paramSet,
                    modelSymbols.find {
                        it.name.equals(rs.getString(1))
                    }!!,
                    rs.getDouble(3),
                    rs.getDouble(4),
                    rs.getDouble(5),
                    rs.getBoolean(6),
                    rs.getBoolean(7)
                )
            )
        }
        return ret
    }

///===========================================

    private val modelSymbolQuery =
        "select occurenceIndex, name, description, id from estimable_symbol where model = ? and type = ?"

    private fun loadModelSymbols(model: KModel, pstmt: PreparedStatement): MutableList<EstimableSymbol> {
        val rs = pstmt.executeQuery()
        val ret = mutableListOf<EstimableSymbol>()
        while (rs.next()) {
            ret.add(
                EstimableSymbol(
                    rs.getInt(4),
                    model,
                    rs.getString(2),
                    rs.getInt(1),
                    rs.getString(3)
                )
            )
        }
        return ret
    }

    fun dbLoadTVFunctionMapping(covariate: KCovariate): TVFunctionMapping {
        if (!tvFunctionMappings.keys.contains(covariate)) {
            // TODO Next 4 lines or so are repeated many times. May want to factor out.
            getConnection().use {
                val sql =
                    "select id, tvfunction, controlparams_json, used from tvfunction_mapping where covariate = ?"
                val pstmt = it.prepareStatement(sql)
                pstmt.setInt(1, covariate.id)
                val rs = pstmt.executeQuery()

                // There may not be a mapping associated with the covariate.
                if (rs.next()) {
                    val functionMappingID = rs.getInt(1)
                    val functionName = rs.getString(2)
                    val controlParams = rs.getString(3)
                    val used = rs.getBoolean(4)

                    val colMappingSql = "select table_column from tvf_table_mapping where tvfunction_mapping = ?"
                    val pstmtMapping = it.prepareStatement(colMappingSql)
                    pstmtMapping.setInt(1, functionMappingID)
                    val mappedColumns = mutableListOf<KMappingInfo>()
                    val rsMapping = pstmtMapping.executeQuery()
                    while (rsMapping.next()) {
                        mappedColumns.add(dbLoadMappingInfo(rsMapping.getInt(1)))
                    }
                    tvFunctionMappings[covariate] =
                        TVFunctionMapping(
                            functionMappingID,
                            covariate,
                            functionName,
                            controlParams,
                            mappedColumns,
                            used
                        )
                } else {
                    tvFunctionMappings[covariate] = TVFunctionMapping(covariate)
                }
            }
        }

        return tvFunctionMappings[covariate]!!
    }


    fun loadSolverResults(model: KModel): MutableList<KSolverResult> {
        if (!solverResultsForModel.keys.contains(model)) {
            val results = mutableListOf<KSolverResult>()
            val sql =
                "select sr.id, sr.solver, sr.dirty, ps.name,  sr.timepoints, sr.controlparams_json, sr.jsonresult   from solverresult sr, parameterset ps " +
                        "where sr.parameterset = ps.id and ps.model = ?"
            getConnection().use {
                val pstmt = it.prepareStatement(sql)
                pstmt.setInt(1, model.modelID)
                val rs = pstmt.executeQuery()
                while (rs.next()) {
                    val resultMatrix = JSONResultMatrix(getTimepoints(rs.getString(5)), rs.getString(7))
                    loadParameterSets(model)[rs.getString(4)]?.let { it1 ->
                        KSolverResult(
                            rs.getInt(1),
                            rs.getString(2),
                            resultMatrix.matrixData.first.toMutableList(),
                            rs.getBoolean(3),
                            it1,
                            getTimepoints(rs.getString(5)),
                            parseControlParameters(rs.getString(6)),
                            //parsedResult.second
                            resultMatrix.matrixData.second.toMutableList()
                        )
                    }?.let { it2 -> results.add(it2) }
                }
            }

            solverResultsForModel[model] = results
        }
        return solverResultsForModel[model]!!

    }


    fun updateModelParameters(model: KModel, params: List<EstimableSymbol>) {
        val l = modelParametersForModel[model]!!
        l.clear()
        l.addAll(params)

    }

    fun updateDependentVariables(model: KModel, params: List<EstimableSymbol>) {
        val l = dependentVariablesForModel[model]!!
        l.clear()
        l.addAll(params)
    }

    fun updateCovariates(model: KModel, params: List<KCovariate>) {
        val l = covariatesForModel[model]!!
        l.clear()
        l.addAll(params)
    }

    fun updateMacros(model: KModel, params: List<KMacro>) {
        val l = macrosForModel[model]!!
        l.clear()
        l.addAll(params)
    }

    fun loadModelParameters(model: KModel): MutableList<EstimableSymbol> {
        if (!modelParametersForModel.containsKey(model)) {
            getConnection().use {
                val pstmt = it.prepareStatement(modelSymbolQuery)
                pstmt.setInt(1, model.modelID)
                pstmt.setString(2, "MP")
                val ret = loadModelSymbols(model, pstmt).toMutableList()
                modelParametersForModel[model] = ret
            }
        }
        return modelParametersForModel[model]!!
    }

    fun loadModelFromDB(modelID: Int): KModel {
        if (!loadedModels.containsKey(modelID)) {
            getConnection().use {
                val pstmt =
                    it.prepareStatement("select m.name, m.title, p.package, m.equations, m.editable from model m, package p  where m.id = ? and p.id = m.package")
                pstmt.setInt(1, modelID)
                val rs = pstmt.executeQuery()
                if (!rs.next())
                    throw Exception("No model with id: $modelID")
                loadedModels[modelID] = KModel(
                    modelID,
                    rs.getString(2),
                    rs.getString(1),
                    rs.getString(3),
                    rs.getString(4),
                    rs.getBoolean(5)
                )
            }
        }
        return loadedModels[modelID]!!
    }

    fun loadModelParametersValues(paramSet: KParameterSet): List<EstimableParameterValue> {
        val modelParameters = loadModelParameters(paramSet.model)
        getConnection().use {
            val pstmt = it.prepareStatement(modelSymbolValuesQuery)
            pstmt.setInt(1, paramSet.id)
            pstmt.setString(2, "MP")
            return readModelSybolsValues(pstmt, modelParameters, paramSet)
        }
    }

    fun loadDependentVariables(model: KModel): MutableList<EstimableSymbol> {
        if (!dependentVariablesForModel.keys.contains(model)) {
            getConnection().use {
                val pstmt = it.prepareStatement(modelSymbolQuery)
                pstmt.setInt(1, model.modelID)
                pstmt.setString(2, "DV")
                val ret = loadModelSymbols(model, pstmt)
                dependentVariablesForModel[model] = ret
            }
        }
        return dependentVariablesForModel[model]!!
    }

    fun loadParameterSets(model: KModel): MutableMap<String, KParameterSet> {
        if (!parameterSetsForModel.keys.contains(model)) {
            val paramSets = mutableMapOf<String, KParameterSet>()
            val sql = "select id, name from parameterset where model = ?"
            getConnection().use {
                val pstmt = it.prepareStatement(sql)
                pstmt.setInt(1, model.modelID)
                val rs = pstmt.executeQuery()
                while (rs.next()) {
                    paramSets.put(
                        rs.getString(2),
                        KParameterSet(rs.getInt(1), rs.getString(2), model)
                    )
                }
            }
            parameterSetsForModel.put(model, paramSets)
        }

        return parameterSetsForModel[model]!!

    }


    private fun getEstimatedRows(paramSet: KParameterSet): List<EstimatedRow> {
        val rows = mutableListOf<EstimatedRow>()
        val sql = "select er.name," +
                " er.estimatedValue," +
                " er.standardError," +
                " er.ciLowerBound," +
                " er.ciHighBound," +
                " er.pvalue," +
                " er.tstat," +
                " er.min," +
                " er.max," +
                " er.description" +
                " from estimation_result_estimated_row er, estimation_result res where er.estimation_result = res.id and res.parameterset = ?"
        getConnection().use {
            val pstmt = it.prepareStatement(sql)
            pstmt.setInt(1, paramSet.id)
            val rs = pstmt.executeQuery()

            while (rs.next()) {
                var i = 1
                rows.add(
                    EstimatedRow(
                        name = rs.getString(i++),
                        estimatedValue = rs.getDouble(i++),
                        standardError = rs.getDouble(i++),
                        ciLowerBound = rs.getDouble(i++),
                        ciHighBound = rs.getDouble(i++),
                        pvalue = rs.getDouble(i++),
                        tstat = rs.getDouble(i++),
                        min = rs.getDouble(i++),
                        max = rs.getDouble(i++),
                        description = rs.getString(i)
                    )
                )
            }
        }
        return rows
    }


    private fun getFixedRows(paramSet: KParameterSet): List<FixedRow> {
        val rows = mutableListOf<FixedRow>()
        val sql = "select er.name," +
                " er.value," +
                " er.min," +
                " er.max," +
                " er.description" +
                " from estimation_result_fixed_row er, estimation_result res where er.estimation_result = res.id and res.parameterset = ?"
        getConnection().use {
            val pstmt = it.prepareStatement(sql)
            pstmt.setInt(1, paramSet.id)
            val rs = pstmt.executeQuery()
            while (rs.next()) {
                var i = 1
                rows.add(
                    FixedRow(
                        name = rs.getString(i++),
                        value = rs.getDouble(i++),
                        min = rs.getDouble(i++),
                        max = rs.getDouble(i++),
                        description = rs.getString(i)
                    )
                )
            }
        }
        return rows
    }

    fun loadEstimationResult(paramSet: KParameterSet): EstimationCIResults {

        val stmt =
            "select solver, objective_function, confidence_interval, optimizer, nb_observations, nb_estimates, best_result," +
                    "rss, aic, aicc, bic, json_intermediate_results from estimation_result where parameterset = ?"

        if (!estimationResultsForParamSet.containsKey(paramSet)) {

            val estimatedRows = getEstimatedRows(paramSet)
            val fixedRows = getFixedRows(paramSet)

            getConnection().use {

                val pstmt = it.prepareStatement(stmt)
                pstmt.setInt(1, paramSet.id)
                val rs = pstmt.executeQuery()
                if (rs.next()) {
                    var i = 1
                    val estResult = EstimationCIResults(
                        paramSet = paramSet,
                        solver = rs.getString(i++),
                        objectiveFunction = rs.getString(i++),
                        ci = rs.getString(i++),
                        optimizer = rs.getString(i++),
                        nbObservations = rs.getInt(i++),
                        nbEstimates = rs.getInt(i++),
                        optimalValue = rs.getDouble(i++),
                        rss = rs.getDouble(i++),
                        aic = rs.getDouble(i++),
                        aicc = rs.getDouble(i++),
                        bic = rs.getDouble(i++),
                        intermediateResults = rs.getString(i),
                        estimatedRows = estimatedRows,
                        fixedRows = fixedRows
                    )
                    estimationResultsForParamSet[paramSet] = estResult
                } else {
                    // Handles the cases of imported sessions from old versions.
                    val estResult = EstimationCIResults(paramSet)
                    estimationResultsForParamSet[paramSet] = estResult
                }
            }
        }
        return estimationResultsForParamSet[paramSet]!!
    }
}