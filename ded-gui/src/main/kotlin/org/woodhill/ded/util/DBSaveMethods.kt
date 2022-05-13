package org.woodhill.ded.util

import org.woodhill.ded.models.*
import org.woodhill.ded.properties.KTabProperties
import org.woodhill.ded.properties.PlotProperty
import org.woodhill.ded.results.EstimationCIResults
import org.woodhill.ded.ui.chartoptions.ParameterChartOptions
import java.sql.Date
import java.sql.SQLException


class DBSaveMethods private constructor( ) {


    companion object {
        val instance = DBSaveMethods()
    }

    private val connection = DBConnection()
    private fun getConnection() = connection.getConnection()


    private fun executeCreateOrUpdate(id: Int, table: String, values: List<Pair<String, Any>>): Int {
        val stmt = getStatement(id, table, values)
        getConnection().use {
            val pstmt = it.prepareStatement(stmt)
            for (i in 0..values.size - 1) {
                pstmt.setObject(i + 1, values[i].second)
            }
            pstmt.executeUpdate()

            if (id < 0) {
                pstmt.generatedKeys.use { generatedKeys ->
                    if (generatedKeys.next()) {
                        val newKey = generatedKeys.getInt(1)
                        return newKey
                    } else {
                        throw SQLException("Creating user failed, no ID obtained.")
                    }
                }
            }
        }
        return id
    }

    fun updatePackage(packageName:String, expandable: Boolean, editable: Boolean) {
        val packageID = getPackageID(packageName)
        getConnection().use { conn ->
            val pstmt = conn.prepareStatement("UPDATE package set expandable=?, editable=? where id = ?")
            pstmt.setBoolean(1, expandable)
            pstmt.setBoolean(2, editable)
            pstmt.setInt(3, packageID)
            pstmt.executeUpdate()
        }
    }

    /**
     * Get id of existing package. Create if it doesn't exist.
     *
     * TODO Rename: also used directly to insert new package
     */
    fun getPackageID(packageName: String, expandable: Boolean = true, editable: Boolean = true): Int {

        val querySql = "select id from package where package = ?"
        val insertSql = "insert into package (package, expandable, editable) values (?,?,?)"

        getConnection().use {
            val pstmt = it.prepareStatement(querySql)
            pstmt.setString(1, packageName)
            val rs = pstmt.executeQuery()
            if (rs.next()) {
                return rs.getInt(1)
            } else {
                val insertStmt = it.prepareStatement(insertSql)
                insertStmt.setString(1, packageName)
                insertStmt.setBoolean(2, expandable)
                insertStmt.setBoolean(3, editable)
                insertStmt.executeUpdate()

                insertStmt.generatedKeys.use { generatedKeys ->
                    if (generatedKeys.next()) {
                        return generatedKeys.getInt(1)
                    } else {
                        throw Exception("Exception occurred inserting package.")
                    }
                }
            }
        }
    }

    fun saveModelObject(kmodel: KModel) {
        val newID = executeCreateOrUpdate(
            kmodel.modelID, "model", listOf(
                Pair("name", kmodel.sessionName),
                Pair("title", kmodel.title),
                Pair("package", getPackageID(kmodel.packageName)),
                Pair("equations", kmodel.equations),
                Pair("editable", kmodel.editable)
            )
        )
        kmodel.modelID = newID
        DBLoadMethods.instance.loadModelFromDB(kmodel.modelID)
    }

    fun updateSessionName(modelID:Int, packageName: String, sessionName:String, sessionTitle:String) {
        val pkgID = getPackageID(packageName);
        getConnection().use {conn ->
            val pstmt = conn.prepareStatement("update model set name = ?, package = ?, title = ? where id=?")
            pstmt.setString(1, sessionName)
            pstmt.setInt(2, pkgID)
            pstmt.setString(3, sessionTitle)
            pstmt.setInt(4, modelID)
            pstmt.executeUpdate()
        }
    }


    fun saveModelParameters(model: KModel, params: List<EstimableSymbol>) {
        val symbolsToDelete = DBLoadMethods.instance.loadModelParameters(model).filter { !params.contains(it) }
        params.forEach {
            saveModelParameter(it)
        }
        deleteModelParameters(symbolsToDelete)
        DBLoadMethods.instance.updateModelParameters(model, params)
    }

    fun saveDependentVariables(model: KModel, params: List<EstimableSymbol>) {
        val symbolsToDelete = DBLoadMethods.instance.loadDependentVariables(model).filter { !params.contains(it) }
        params.forEach {
            saveDependentVariable(it)
        }
        deleteDependentVariables(symbolsToDelete)
        DBLoadMethods.instance.updateDependentVariables(model, params)
    }


    private fun saveModelParameter(mp: EstimableSymbol) {
        saveModelSymbol(mp, "MP")
    }

    private fun saveDependentVariable(dv: EstimableSymbol) {
        saveModelSymbol(dv, "DV")
    }

    private fun saveModelSymbol(ms: EstimableSymbol, symbolType: String) {
        val newID = executeCreateOrUpdate(
            ms.id, "estimable_symbol", listOf(
                Pair("type", symbolType),
                Pair("occurenceIndex", ms.occurenceIndex),
                Pair("name", ms.name),
                Pair("model", ms.model.modelID),
                Pair("description", ms.description)
            )
        )
        ms.id = newID
    }

    fun saveCovariate(cov: KCovariate) {
        val newID = executeCreateOrUpdate(
            cov.id, "covariate", listOf(
                Pair("occurenceIndex", cov.occurenceIndex),
                Pair("name", cov.name),
                Pair("model", cov.model.modelID),
                Pair("description", cov.description),
                Pair("timeexpression", cov.timeExpression)
            )
        )
        cov.id = newID
    }

    fun saveMacro(macro: KMacro) {
        val newID = executeCreateOrUpdate(
            macro.id, "macro", listOf(
                Pair("occurenceIndex", macro.occurenceIndex),
                Pair("name", macro.name),
                Pair("model", macro.model.modelID),
                Pair("description", macro.description),
                Pair("formula", macro.formula)
            )
        )
        macro.id = newID
    }


    private fun saveParameterSetValue(value: EstimableParameterValue) {
        val newID = executeCreateOrUpdate(
            value.id, "estimable_symbol_value", listOf(
                Pair("parameterset", value.parameterSet.id),
                Pair("modelsymbol", value.modelSymbol.id),
                Pair("value", value.defaultValue),
                Pair("min", value.minValue),
                Pair("max", value.maxValue),
                Pair("sliders", value.slider),
                Pair("estimated", value.estimated)
            )
        )
        value.id = newID
    }

    private fun deleteOldParamSetValues(paramSet: KParameterSet) {
        getConnection().use {
            println("Deleting old param set values")
            val pstmt = it.prepareStatement("delete from estimable_symbol_value where parameterset= ?")
            pstmt.setInt(1, paramSet.id)
            pstmt.executeUpdate()
        }
    }



    fun deleteParameterSet(parameterSet: KParameterSet) {
        getConnection().use {
            val pstmt = it.prepareStatement("delete from parameterSet where id = ?")
            pstmt.setInt(1, parameterSet.id)
            pstmt.executeUpdate()
        }
    }

    fun saveParameterSet(parameterSet: KParameterSet,  deleteOld:Boolean = false) {
        if (deleteOld)
            deleteOldParamSetValues(parameterSet)

        val newID = executeCreateOrUpdate(
            parameterSet.id, "parameterSet", listOf(
                Pair("name", parameterSet.name),
                Pair("model", parameterSet.model.modelID)
            )
        )
        parameterSet.id = newID
        parameterSet.modelParameterValues.forEach {
            saveParameterSetValue(it)
        }
        parameterSet.initialConditionsValues.forEach {
            saveParameterSetValue(it)
        }
    }


    fun saveChartableProperties(parameterChartOptions: List<ParameterChartOptions>, model: KModel) {
        val addProperty = "insert into chartable_properties (model, symbol, log, visible, style) values (?,?,?,?,?)"

        getConnection().use {
            val pstmt = it.prepareStatement("delete from chartable_properties where model = ?")
            pstmt.setInt(1, model.modelID)
            pstmt.executeUpdate()

            val addStatement = it.prepareStatement(addProperty)
            parameterChartOptions.forEach {
                addStatement.setInt(1, model.modelID)
                addStatement.setString(2, it.name)
                addStatement.setBoolean(3, it.useLog)
                addStatement.setBoolean(4, it.isVisible)
                addStatement.setString(5, it.displayStyle.name)
                addStatement.executeUpdate()
            }
        }
    }


    fun savePlotProperties(plotProperties: List<PlotProperty>) {
        getConnection().use {conn ->
            val pstmt = conn.prepareStatement("insert into plot_properties (model,chartVar, key, value) values (?,?,?,?)")
            plotProperties.forEach {prop ->
                var i=1
                pstmt.setInt(i++, prop.model.modelID)
                pstmt.setString(i++, prop.chartVar)
                pstmt.setString(i++, prop.key)
                pstmt.setString(i, prop.value)
                pstmt.executeUpdate()
            }
        }
    }


    fun saveTabProperties(model: KModel, properties: KTabProperties) {
        val delCommand = "delete from tab_properties where model=? and tab=?"
        val insertCommand = "insert into tab_properties (model, tab, key, value) values (?,?,?,?)"

        getConnection().use { conn ->
            val delSql = conn.prepareStatement(delCommand)
            delSql.setInt(1, model.modelID)
            delSql.setString(2, properties.tabName())
            delSql.executeUpdate()

            val insertSql = conn.prepareStatement(insertCommand)
            properties.keySet().forEach { key ->
                insertSql.setInt(1, model.modelID)
                insertSql.setString(2, properties.tabName())
                insertSql.setString(3, key)
                insertSql.setString(4, properties.getAsString(key))
                insertSql.execute()
            }
        }
    }


    /**
     * Save the solver result. We save only one solver result per parameter set.
     */
    fun saveSolverResult(solverResult: KSolverResult) {
        if (solverResult.id >= 0) {
            throw Exception("SolverResults are immutable. Shouldn't save with id > 0")
        }


        val stmt =
            "INSERT INTO solverresult (solver, parameterset, dirty, date, jsonresult, timepoints, controlparams_json) values(?, ?, ?, ?, ?, ?, ?)"
        getConnection().use {
            val delStmt = it.prepareStatement("delete from solverresult where parameterset = ?")
            delStmt.setInt(1, solverResult.parameterSet.id)
            delStmt.executeUpdate()

            val pstmt = it.prepareStatement(stmt)
            pstmt.setString(1, solverResult.solver)
            pstmt.setInt(2, solverResult.parameterSet.id)
            pstmt.setBoolean(3, solverResult.dirty)
            pstmt.setDate(4, Date(System.currentTimeMillis()))
            pstmt.setString(5, solverResult.dataAsJson())
            pstmt.setString(6, solverResult.timepoints.joinToString(" "))
            pstmt.setString(7, solverResult.controlParamsAsJson())

            pstmt.executeUpdate()
            pstmt.generatedKeys.use { generatedKeys ->
                if (generatedKeys.next()) {
                    solverResult.id = generatedKeys.getInt(1)
                } else {
                    throw SQLException("Creating user failed, no ID obtained.")
                }
            }
        }
    }


    /**
     * Save the estimation result. Right now assumes a single estimator result per parameter set. Will delete existing one,
     * remove old one. No need for an id here.
     */
    fun saveEstimationResult(estimationResult: EstimationCIResults) {

        deleteAll("estimation_result", "parameterset", estimationResult.paramSet.id)

        val stmt =
            "INSERT INTO estimation_result (parameterset, solver, objective_function, confidence_interval, optimizer, nb_observations, nb_estimates, best_result," +
                    "rss, aic, aicc, bic, json_intermediate_results) values(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"


        val estimatedRowSql = "INSERT INTO estimation_result_estimated_row (estimation_result," +
                "  name," +
                "  estimatedValue," +
                "  standardError," +
                "  ciLowerBound," +
                "  ciHighBound," +
                "  pvalue," +
                "  tstat," +
                "  min," +
                "  max," +
                "  description) VALUES (?,?,?,?,?,?,?,?,?,?,?) "

        val fixedRowSql = "INSERT INTO estimation_result_fixed_row (estimation_result," +
                "  name," +
                "  value," +
                "  min," +
                "  max," +
                "  description) VALUES (?,?,?,?,?,?) "

        getConnection().use {
            val pstmt = it.prepareStatement(stmt)
            run {
                var i = 1
                pstmt.setInt(i++, estimationResult.paramSet.id)
                pstmt.setString(i++, estimationResult.solverStr)
                pstmt.setString(i++, estimationResult.objectiveFunctionStr)
                pstmt.setString(i++, estimationResult.ciStr)
                pstmt.setString(i++, estimationResult.optimizerStr)
                pstmt.setInt(i++, estimationResult.nbObservations)
                pstmt.setInt(i++, estimationResult.nbEstimates)
                pstmt.setDouble(i++, estimationResult.optimalValue)
                pstmt.setDouble(i++, estimationResult.rss)
                pstmt.setDouble(i++, estimationResult.aic)
                pstmt.setDouble(i++, estimationResult.aicc)
                pstmt.setDouble(i++, estimationResult.bic)
                pstmt.setString(i, estimationResult.intermediateResults)
                pstmt.executeUpdate()
            }
            val newKey = pstmt.generatedKeys.getInt(1)
            run {
                val rowStmt = it.prepareStatement(estimatedRowSql)
                estimationResult.estimatedRows.forEach {
                    var i = 1
                    rowStmt.setInt(i++, newKey)
                    rowStmt.setString(i++, it.name)
                    rowStmt.setDouble(i++, it.estimatedValue)
                    rowStmt.setDouble(i++, it.standardError)
                    rowStmt.setDouble(i++, it.ciLowerBound)
                    rowStmt.setDouble(i++, it.ciHighBound)
                    rowStmt.setDouble(i++, it.pvalue)
                    rowStmt.setDouble(i++, it.tstat)
                    rowStmt.setDouble(i++, it.min)
                    rowStmt.setDouble(i++, it.max)
                    rowStmt.setString(i, it.description)
                    rowStmt.executeUpdate()
                }
            }

            run {
                val rowStmt = it.prepareStatement(fixedRowSql)
                estimationResult.fixedRows.forEach {
                    var i = 1
                    rowStmt.setInt(i++, newKey)
                    rowStmt.setString(i++, it.name)
                    rowStmt.setDouble(i++, it.fixedValue)
                    rowStmt.setDouble(i++, it.min)
                    rowStmt.setDouble(i++, it.max)
                    rowStmt.setString(i++, it.description)
                    rowStmt.executeUpdate()
                }
            }
        }
    }


    private fun deleteAll(table: String, field: String, key: Int) {
        getConnection().use {
            val delStmt = it.prepareStatement("delete from $table where $field = ?")
            delStmt.setInt(1, key)
            delStmt.executeUpdate()
        }
    }


    fun deleteSession(modelID: Int) {
        val stmt = "delete from model where id = ?"
        getConnection().use {
            val pstmt = it.prepareStatement(stmt)
            pstmt.setInt(1, modelID)
            pstmt.executeUpdate()
        }
    }

    /**
     * Delete a package. Not validating here.
     */
    fun deletePackage(packageName: String) {
        val stmt = "delete from package where package = ?"
        getConnection().use {
            val pstmt = it.prepareStatement(stmt)
            pstmt.setString(1, packageName)
            pstmt.executeUpdate()
        }
    }


////////////////////////////////////////////////////////////////////////////////////////////////////////////


    private fun getStatement(id: Int, table: String, values: List<Pair<String, Any>>): String {
        return if (id < 0) {
            "insert into $table (" + values.map { it.first }
                .joinToString(",") + ") values (" + List<String>(values.size) { index ->
                "?"
            }.joinToString(",") + ")"
        } else {
            "update $table set " + values.map { x -> "${x.first} = ?" }.joinToString(",") + " where id = $id"
        }
    }


    fun saveDatatable(datatable: KDatatable) {
        // TODO Add dirty flag so we don't save if not needed. Now assume dirty.

        // Assume datatable always main
        if (!datatable.hasNewData()) {
            return
        }

        getConnection().use { conn ->
            val deletePstmt = conn.prepareStatement("delete from table_column where model = ?")
            deletePstmt.setInt(1, datatable.mappingInfoSet.model.modelID)
            deletePstmt.executeUpdate()
            var colIndexToSave = 0
            datatable.mappingInfoSet.mappings.values.sortedBy { it.columnIndex }.forEach {
                val insertPstmt = conn.prepareStatement(
                    "insert into table_column (model, column_name, column_index, status, data, used_for, map_to, formula, description, failure_message)" +
                            " values(?,?,?,?,?,?,?,?,?, ?)"
                )
                var i: Int = 1
                insertPstmt.setInt(i++, datatable.mappingInfoSet.model.modelID)
                insertPstmt.setString(i++, it.column_name)
                insertPstmt.setInt(i++, colIndexToSave)
                insertPstmt.setInt(i++, it.status.status)
                insertPstmt.setString(i++, it.dataSublist(0, it.dataList.size - datatable.defaultEmptyRows))
                insertPstmt.setString(i++, it.usedFor.name)
                insertPstmt.setString(i++, it.mapTo)
                insertPstmt.setString(i++, it.formula)
                insertPstmt.setString(i++, it.description)
                insertPstmt.setString(i++, it.failureMessage)

                insertPstmt.executeUpdate()
                insertPstmt.generatedKeys.use { generatedKeys ->
                    if (generatedKeys.next()) {
                        //solverResult.id = generatedKeys.getInt(1)
                        it.id = generatedKeys.getInt(1)
                    } else {
                        throw SQLException("Creating user failed, no ID obtained.")
                    }
                }

                colIndexToSave += 1
            }
        }
    }

    fun saveCovariateMapping(covariateTVFMappings: List<TVFunctionMapping>) {
        println("Saving covariate mappings")
        covariateTVFMappings.forEach { tvfMapping ->
            val newID = executeCreateOrUpdate(
                tvfMapping.id, " tvfunction_mapping",
                listOf(
                    Pair("tvfunction", tvfMapping.tvFunctionName),
                    Pair("controlparams_json", tvfMapping.getControlParamsAsJson()),
                    Pair("covariate", tvfMapping.covariate.id),
                    Pair("used", tvfMapping.used)
                )
            )
            tvfMapping.id = newID

            // First delete all the existing mappings.
            getConnection().use { conn ->
                val pstmt = conn.prepareStatement(
                    "delete from tvf_table_mapping where tvfunction_mapping =?"
                )
                pstmt.setInt(1, tvfMapping.id)
                pstmt.executeUpdate()

                // Now recreate new ones. Should be low cost, at least not higher than checking existing ones,
                // or keeping track of the state.
                val addStmt = "insert into tvf_table_mapping (tvfunction_mapping, table_column) values(?,?)"
                tvfMapping.tableColumns.forEach { tc ->
                    val ipstmt = conn.prepareStatement(addStmt)
                    println("Saving tvf_table_mapping for ${tvfMapping.id} and ${tc.id}")
                    ipstmt.setInt(1, tvfMapping.id)
                    ipstmt.setInt(2, tc.id)
                    ipstmt.executeUpdate()
                }
            }
        }
    }


    // TODO Consolidate.
    private fun deleteMacros(symbols: List<KMacro>) {
        if (symbols.isEmpty())
            return
        val sqlString = "delete from macro where id in (${symbols.map { it.id }.joinToString()})"
        println(sqlString)
        getConnection().use { conn ->
            val pstmt = conn.prepareStatement(sqlString)
            pstmt.executeUpdate()
        }
    }

    private fun deleteCovariates(symbols: List<KCovariate>) {
        if (symbols.isEmpty())
            return
        val sqlString = "delete from covariate where id in (${symbols.map { it.id }.joinToString()})"
        println(sqlString)
        getConnection().use { conn ->
            val pstmt = conn.prepareStatement(sqlString)
            pstmt.executeUpdate()
        }
    }

    private fun deleteModelSymbols(symbols: List<EstimableSymbol>) {
        if (symbols.isEmpty())
            return
        val sqlString = "delete from estimable_symbol where id in (${symbols.map { it.id }.joinToString()})"
        println(sqlString)
        getConnection().use { conn ->
            val pstmt = conn.prepareStatement(sqlString)
            pstmt.executeUpdate()
        }
    }

    private fun deleteModelParameters(modelSymbols: List<EstimableSymbol>) {
        deleteModelSymbols(modelSymbols)
    }

    private fun deleteDependentVariables(modelSymbols: List<EstimableSymbol>) {
        deleteModelSymbols(modelSymbols)
    }
}