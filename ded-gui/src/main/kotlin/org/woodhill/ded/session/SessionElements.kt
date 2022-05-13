package org.woodhill.ded.session

import edu.rochester.urmc.cbim.jni.*
import org.jfree.chart.ui.Drawable
import org.woodhill.ded.models.*
import org.woodhill.ded.properties.*
import org.woodhill.ded.results.EstimationCIResults
import org.woodhill.ded.ui.chartoptions.ChartableControlsTB
import org.woodhill.ded.ui.chartoptions.KChartableProperties
import org.woodhill.ded.util.DBSaveMethods
import org.woodhill.ded.util.DEDUtils.Companion.synchronizeNewModelSymbols

// Get rid of tornadofx dependencies. Should be in SessionViewModel.
import tornadofx.asObservable
import tornadofx.observableListOf

class SessionElements(val model: KModel) {
    val defaulParameterSetName = "Default"
    val estimationParameterSetName = "Estimation"

    val solverResults = KSolverResult.getSolverResults(model)
    val mappingInfoSet = KMappingInfoSet.getMappingInfoSet(model)
    val datatable = KDatatable(mappingInfoSet)

    val simTabProperties = KTabProperties.loadTabProperties(model, KSimTabProperties::class)
    val solverTabProperties = KTabProperties.loadTabProperties(model, KSolverTabProperties::class)
    val estimationTabProperties = KTabProperties.loadTabProperties(model, EstimationTabProperties::class)
    val globalProperties = KTabProperties.loadTabProperties(model, GlobalProperties::class)
    val estimationResultsProperties = KTabProperties.loadTabProperties(model, EstimationResultsProperties::class)

    val modelObj: JDDEModel = JDDEModel(if (globalProperties.validModel) model.equations else "")

    private val chartingParameters = KChartableProperties.getProperties(model)
    val chartableControls = ChartableControlsTB(model, modelObj, chartingParameters)

    val plots = observableListOf<Drawable>()

    val defaultParameterSet: KParameterSet =
        KParameterSet.getOrCreateParameterSet(model, simTabProperties.selectedParamSet(defaulParameterSetName))

    val estimationParameterSet: KParameterSet =
        KParameterSet.getOrCreateParameterSet(model, estimationParameterSetName)

    val macros = KMacro.getMacros(model).asObservable()
    val covariates = KCovariate.getCovariates(model).asObservable()

    // For now there will only be 1 estimation result, associated with estimation parameter set.
    // Will "lock" param set after an estimation is completed.
    val estimationResults = EstimationCIResults.loadEstimationResult(estimationParameterSet)

    var modelParams = EstimableSymbol.loadModelParameters(model).toMutableList()
    var initialConditions = EstimableSymbol.loadInitialConditions(model).toMutableList()

    private fun createTVFunctionMappings(covs: List<KCovariate>): MutableMap<KCovariate, TVFunctionMapping> =
        covs.associateWith { TVFunctionMapping.getMappingForCovariate(it) }.toMutableMap()

    val tvFunctionMappings: MutableMap<KCovariate, TVFunctionMapping> = createTVFunctionMappings(covariates)

    private val deletedParameterSets = mutableSetOf<KParameterSet>()

    fun deleteParameterSet(ps: KParameterSet) {
        deletedParameterSets.add(ps)
        parameterSets.removeIf {
            it == ps
        }
        solverResults.removeIf {
            it.parameterSet == ps
        }
    }

    // =================================================================================================
    // Methods involved in updating session

    private fun updateModelSymbols() {

        val modelParamsBefore = modelParams.map { it.name }.toSet()
        val modelParamsAfter = modelObj.jModelParameters.map { it.name }.toSet()

        val depVarsBefore = initialConditions.map { it.name }.toSet()
        val depVarsAfter = modelObj.jInitialConditions.map { it.name }.toSet()

        synchronizeNewModelSymbols(
            modelParams,
            modelObj.jModelParameters,
            EstimableSymbol.loadModelParameters(model),
            { x: ModelSymbol -> x.name },
            { x: JModelParameter -> x.name },
            { x: ModelSymbol, y: JModelParameter -> x.occurenceIndex = y.firstOccurenceLine },
            { x: JModelParameter -> EstimableSymbol(-1, model, x.name, x.firstOccurenceLine, "") })


        synchronizeNewModelSymbols(
            initialConditions,
            modelObj.jInitialConditions,
            EstimableSymbol.loadInitialConditions(model),
            { x: ModelSymbol -> x.name },
            { x: JInitialCondition -> x.name },
            { x: ModelSymbol, y: JInitialCondition -> x.occurenceIndex = y.firstOccurenceLine },
            { x: JInitialCondition -> EstimableSymbol(-1, model, x.name, x.firstOccurenceLine, "") })

        // TODO Should we do for all parameter sets, and discard solver results if there was any change
        // BUG here: updateSymbols clears the param set values. (Fixed?)

        // TODO Do we need to take care of deleted parameter sets
        if (!(depVarsBefore == depVarsAfter && modelParamsBefore == modelParamsAfter)) {
            parameterSets.filter {
                !(it.name == defaulParameterSetName || it.name == estimationParameterSetName)
            }.map {
                deleteParameterSet(it)
            }
        }
//
//        KParameterSet.getParameterSet(model, defaulParameterSetName)?.updateSymbols(modelParams, initialConditions)
//        KParameterSet.getParameterSet(model, estimationParameterSetName)?.updateSymbols(modelParams, initialConditions)

        parameterSets.forEach{it.updateSymbols(modelParams, initialConditions)}
        synchronizeNewModelSymbols(
            covariates,
            modelObj.jCovariates,
            KCovariate.getCovariates(model),
            { x: ModelSymbol -> x.name },
            { x: JCovariate -> x.name },
            { x: KCovariate, y: JCovariate -> x.occurenceIndex = -1; x.timeExpression = y.displayExpression.substring(
                y.displayExpression.indexOf(',') + 1,
                y.displayExpression.lastIndexOf(')')) },
            { x: JCovariate ->
                KCovariate(
                    model, x.name,
                    x.displayExpression.substring(
                        x.displayExpression.indexOf(',') + 1,
                        x.displayExpression.lastIndexOf(')')
                    ),
                    "",
                    -1
                )
            })

        synchronizeNewModelSymbols(
            macros,
            modelObj.jMacros,
            KMacro.getMacros(model),
            { x: ModelSymbol -> x.name },
            { x: JMacro -> x.name },
            { x: KMacro, y: JMacro -> x.formula = y.formula },
            { x: JMacro -> KMacro(model, x.name, x.formula, -1) })
    }

    fun updateChartableProperties() {
        chartableControls.refresh()
    }

    /**
     * Update the mappings. Nothing to change in cases of plot_only, ignore, time.
     *
     * In case of covariate must check that the covariate still exists. If not set to ignore.
     *
     * In case of estimation, check that formula is still valid. If not set to ignore.
     */
    private fun reassignDataMappings() {

        val colsUsedInCovariates = tvFunctionMappings.values.map { it.tableColumns }.flatten()
        mappingInfoSet.mappings.values.forEach { kmi ->
            if (kmi.usedFor == KMappingInfo.MappingType.COVARIATE) {
                if (!colsUsedInCovariates.contains(kmi)) {
                    kmi.usedFor = KMappingInfo.MappingType.IGNORE
                }
            }

            if (kmi.usedFor == KMappingInfo.MappingType.ESTIMATION) {
                kmi.status =
                    if (modelObj.validateFormula(kmi.formula)
                            .isEmpty()
                    ) KMappingInfo.MappingStatus.VALID else KMappingInfo.MappingStatus.FAILED
            }
        }
    }

    private fun reassignCovariteMappings() {
        val newMappings = mutableMapOf<KCovariate, TVFunctionMapping>()
        val oldCovNames = tvFunctionMappings.keys.map { it.name }.toString()
        covariates.forEach { cov ->
            if (oldCovNames.contains(cov.name)) {
                val oldMapping = tvFunctionMappings.filter { it.key.name == cov.name }.values.toList()
                if (oldMapping.size != 1) {
                    println("Error: problem with updating mapping")
                    newMappings[cov] = TVFunctionMapping(cov)
                } else {
                    newMappings[cov] = oldMapping[0].clone(cov)
                }

            } else {
                newMappings[cov] = TVFunctionMapping(cov)
            }
        }

        tvFunctionMappings.clear()
        tvFunctionMappings.putAll(newMappings)
    }

    fun parseNewEquations(newEquations: String): List<Pair<Int, String>> {

        modelObj.updateJEquations(newEquations)


        return if (modelObj.errorMessages.size > 0) {
            modelObj.errorMessages.map { Pair(it.first, it.second) }
        } else {
            updateModelSymbols()
            updateChartableProperties()
            reassignCovariteMappings()
            reassignDataMappings()
            globalProperties.validModel = true // Still used?
            listOf()
        }
    }


    // Exporting
    fun exportJsonSimResults(parameterSet: String, filename: String) {
        val paramSet = parameterSetByName(parameterSet)
        val solverResult = solverResults.find { it.parameterSet == paramSet }
        if (solverResult != null) {
            solverResult.exportAsJson(filename)
        } else {
            // TODO Implement logging, and log here
            println("Trying to export from non-existing solver result for param set $paramSet in session ${model.sessionName}")
        }
    }

    fun exportCSVSimResults(parameterSet: String, filename: String) {
        val paramSet = parameterSetByName(parameterSet)

        val solverResult =
            solverResults.find { it.parameterSet == paramSet }
        if (solverResult != null) {
            solverResult.exportAsCSV(filename)
        } else {
            // TODO Implement logging, and log here
            println("Trying to export from non-existing solver result for param set $paramSet in session ${model.sessionName}")
        }
    }

    fun exportEstimationResult(filename: String, asCSV: Boolean = true) =
        estimationResults.exportEstimationResults(filename, asCSV)

    fun exportDatable(filename: String) {
        datatable.exportData(filename)
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Save session. All methods to save objects are here. Need to go in the correct order to satisfy the
    // dependencies.
    fun save() {
        saveModelValues()
        saveFirstLevel()
        saveSecondLevel()
    }

    private fun saveModelValues() {
        model.save()
    }


    private fun saveFirstLevel() {
        // TODO Find which ones are deleted. Need to keep them somewhere first.
        // Shouldn't use DBSaveMethods here. Too specific

//        // Create methods in EstimableSymbolClass to update[ModelParameter|InitialCondion]
//        // that will know to delete the ones no longer there, save/update new ones.
//        modelParams.forEach { DBSaveMethods.saveModelParameter(it) }
//        initialConditions.forEach { DBSaveMethods.saveDependentVariable(it) }
//
        EstimableSymbol.updateDependentVariables(model, initialConditions)
        EstimableSymbol.updateModelParameters(model, modelParams)

        macros.forEach { DBSaveMethods.instance.saveMacro(it) }
        covariates.forEach { DBSaveMethods.instance.saveCovariate(it) }

        // TODO Need to figure out which parameter sets have been deleted

        deletedParameterSets.forEach {
            DBSaveMethods.instance.deleteParameterSet(it)
        }

        KParameterSet.parameterSetsForModel(model).values.forEach {
            DBSaveMethods.instance.saveParameterSet(it) // Will also save param set values
        }
        DBSaveMethods.instance.saveChartableProperties(chartableControls.chartableProperties, model)
        DBSaveMethods.instance.saveDatatable(datatable)

    }

    private fun saveSecondLevel() {

        solverResults.filter { it.id < 0 }.forEach { DBSaveMethods.instance.saveSolverResult(it) }
        DBSaveMethods.instance.saveCovariateMapping(tvFunctionMappings.values.toList())

        // TODO EstimationResult object should also contain the datatable, mapping info
        DBSaveMethods.instance.saveEstimationResult(estimationResults)
        // TODO This should be automated... Create a set will app the property objects.
        simTabProperties.saveProperties()
        solverTabProperties.saveProperties()
        estimationTabProperties.saveProperties()
        globalProperties.saveProperties()
        estimationResultsProperties.saveProperties()
    }

    // Parameter Set Functionality

    val parameterSets = observableListOf<KParameterSet>()

    /**
     * Copy a parameter set to targetParameterSetName. If it does not exist it is created.
     */
    fun copyParameterSet(
        targetParameterSetName: String,
        copiedFrom: KParameterSet = parameterSetByName(simTabProperties.currentParameterSetProperty.value)
    ): KParameterSet {
        val targetParamSetObject = KParameterSet.getOrCreateParameterSet(model, targetParameterSetName)

        val newModelParameters = copiedFrom.modelParameterValues.map { it.duplicate(targetParamSetObject) }
        val newInitialConditions = copiedFrom.initialConditionsValues.map { it.duplicate(targetParamSetObject) }

        targetParamSetObject.replaceValues(newModelParameters, newInitialConditions)
        if (!parameterSets.contains(targetParamSetObject)) {
            parameterSets.add(targetParamSetObject)
        }
        return targetParamSetObject
    }

    fun parameterSetByName(name: String): KParameterSet {
        val paramSet = parameterSets.find { it.name.equals(name) }
            ?: throw Exception("There should be a parameter set with name: $name")
        return paramSet
    }

    /**
     * Update the covariate mappings. May add checking in case nothing has changed to prevent duplicate work.
     * Really belong here?
     * Must use the control params defined on the cov mapping pane from datatable.
     *
     */
    fun updateCovariateMappings() {
        modelObj.jCovariates.forEach { jcov ->
            val kobj = KCovariate.getCovariates(model).findLast {
                it.name == jcov.name
            }

            val covMapping = tvFunctionMappings[kobj]!!

            if (covMapping.used && covMapping.tableColumns.size > 0 && covMapping.tvFunctionName != "") {
                val tvf = JTimeVaryingFunction(covMapping.tvFunctionName)
                jcov.setTVFMapped(covMapping.used)
                jcov.timeVaryingFunction = tvf
                tvf.controlParameters.forEach { param ->
                    param.value = covMapping.getControlParamsMap()[param.name]
                }

                val timepoints = mappingInfoSet.timepoints()
                covMapping.tableColumns.forEach {
                    val controlPoints = mutableListOf<org.apache.commons.math3.util.Pair<Double, Double>>()
                    timepoints.zip(it.dataList).forEach { time_val ->
                        if (time_val.first != Double.NaN) {
                            controlPoints.add(
                                org.apache.commons.math3.util.Pair(
                                    time_val.first,
                                    time_val.second.toDoubleOrNull() ?: Double.NaN
                                )
                            )
                        }
                    }
                    tvf.addControlPoints(controlPoints)
                }
            } else {
                jcov.setTVFMapped(false)
            }
        }
    }

    init {
        //  parameterChartOptions.addAll(chartingParameters.parameterChartOptions)
        parameterSets.addAll(KParameterSet.parameterSetsForModel(model).values)
    }

}


