package org.woodhill.ded.results

import edu.rochester.urmc.cbim.jni.EstimatorResult
import edu.rochester.urmc.cbim.jni.JConfidenceIntervalResult
import javafx.beans.property.SimpleDoubleProperty
import javafx.beans.property.SimpleIntegerProperty
import javafx.beans.property.SimpleStringProperty
import org.woodhill.ded.models.KParameterSet
import org.woodhill.ded.properties.AlgorithmProperty
import org.woodhill.ded.util.DBLoadMethods
import tornadofx.getValue
import tornadofx.observableListOf
import tornadofx.setValue
import java.io.File
import java.io.StringReader
import javax.json.Json
import javax.json.JsonObject
import javax.json.JsonObjectBuilder

/**
 * Class to contains the estimation and CI results, as well as the fixed parameters used for the estimation.
 */


class EstimatedRow(
    name: String,
     estimatedValue: Double,
     standardError: Double,
     ciLowerBound: Double,
     ciHighBound: Double,
     pvalue: Double,
     tstat: Double,
     min: Double,
     max: Double,
     description: String
) {
    val nameProperty = SimpleStringProperty(name)
    val estimatedValueProperty = SimpleDoubleProperty(estimatedValue)
    val standardErrorProperty = SimpleDoubleProperty(standardError)
    val ciLowerBoundProperty = SimpleDoubleProperty(ciLowerBound)
    val ciHighBoundProperty = SimpleDoubleProperty(ciHighBound)
    val pvalueProperty = SimpleDoubleProperty(pvalue)
    val tstatProperty = SimpleDoubleProperty(tstat)
    val minProperty = SimpleDoubleProperty(min)
    val maxProperty = SimpleDoubleProperty(max)
    val descriptionProperty = SimpleStringProperty(description)

    var name by nameProperty
    var estimatedValue by estimatedValueProperty
    var standardError by standardErrorProperty
    var ciLowerBound by ciLowerBoundProperty
    var ciHighBound by ciHighBoundProperty
    var pvalue by pvalueProperty
    var tstat by tstatProperty
    var min by minProperty
    var max by maxProperty
    var description by descriptionProperty


}

class FixedRow(
     name: String,
     value: Double,
     min: Double,
     max: Double,
     description: String
) {
    val nameProperty = SimpleStringProperty(name)
    val fixedValueProperty = SimpleDoubleProperty(value)
    val minProperty = SimpleDoubleProperty(min)
    val maxProperty = SimpleDoubleProperty(max)
    val descriptionProperty = SimpleStringProperty(description)

    var name by nameProperty
    var fixedValue by fixedValueProperty
    var min by minProperty
    var max by maxProperty
    var description by descriptionProperty
}

class EstimationCIResults(val paramSet: KParameterSet) {

    val nbObservationsProperty = SimpleIntegerProperty()
    val nbEstimatesProperty = SimpleIntegerProperty()

    val optimalValueProperty = SimpleDoubleProperty()
    val rssValueProperty = SimpleDoubleProperty()
    val aicValueProperty = SimpleDoubleProperty()
    val aiccValueProperty = SimpleDoubleProperty()
    val bicValueProperty = SimpleDoubleProperty()

    var nbObservations by nbObservationsProperty
    var optimalValue by optimalValueProperty
    var rss by rssValueProperty
    var aic by aicValueProperty
    var aicc by aiccValueProperty
    var bic by bicValueProperty
    var nbEstimates by nbEstimatesProperty

    // Will contain json strings
    var solverStr:String = ""
    var optimizerStr:String = ""
    var ciStr:String = ""
    var objectiveFunctionStr:String = ""
    var intermediateResults: String = ""

    val estimatedRows = observableListOf<EstimatedRow>()
    val fixedRows = observableListOf<FixedRow>()


    // Only called from dbLoadMethods.
    constructor(paramSet: KParameterSet, solver:String, ci:String, optimizer:String, objectiveFunction:String, nbObservations:Int, nbEstimates:Int, optimalValue: Double,
    rss:Double, aic:Double, aicc:Double, bic:Double, intermediateResults:String, estimatedRows:List<EstimatedRow>, fixedRows:List<FixedRow>):this(paramSet) {
        solverStr = solver
        optimizerStr = optimizer
        ciStr = ci
        objectiveFunctionStr = objectiveFunction
        this.nbObservations = nbObservations
        this.nbEstimates = nbEstimates
        this.optimalValue = optimalValue
        this.rss = rss
        this.aic = aic
        this.aicc = aicc
        this.bic = bic
        this.intermediateResults = intermediateResults
        this.estimatedRows.addAll(estimatedRows)
        this.fixedRows.addAll(fixedRows)
    }

    fun updateResults(newResults: Pair<EstimatorResult, JConfidenceIntervalResult>, solver:AlgorithmProperty, ci:AlgorithmProperty, optimizer: AlgorithmProperty, objectiveFunction: AlgorithmProperty) {
        solverStr = solver.toJsonString();
        optimizerStr = optimizer.toJsonString()
        ciStr = ci.toJsonString()
        objectiveFunctionStr = objectiveFunction.toJsonString()
        estimatedRows.clear()

        val ciResults =  newResults.second.resultObjects
        for (i in (0 until newResults.first.bestResult.size)) {
            val param = paramSet.getParamWithName(newResults.first.estimatedParameterNames[i])
            estimatedRows.add(
                EstimatedRow(
                    param.nameProperty.value,
                    newResults.first.bestResult[i],
                    ciResults[i].standardError,
                    ciResults[i].lowerBound,
                    ciResults[i].upperBound,
                    ciResults[i].pvalue,
                    ciResults[i].tstat,
                    param.minValue,
                    param.maxValue,
                    param.descProperty.value
                )
            )
        }

        fixedRows.clear()
        val fixed = paramSet.initialConditionsValues.filter { !it.estimated }.toMutableList()
        fixed.addAll(paramSet.modelParameterValues.filter { !it.estimated })
        fixed.forEach {
            fixedRows.add(
                FixedRow(
                    it.nameProperty.value,
                    it.defaultValue,
                    it.minValue,
                    it.maxValue,
                    it.descProperty.value
                )
            )
        }

        println("Updating estimator result, with # observations: ${newResults.first.nObs}")
        aic = newResults.first.aic
        bic = newResults.first.bic
        aicc = newResults.first.aiCc
        nbObservations = newResults.first.nObs
        rss = newResults.first.rss
        optimalValue = newResults.first.optimalValue
        nbEstimates = newResults.first.estimatedParameterNames.size
    }


    private fun exportCSV(filename: String) {
        File(filename).bufferedWriter().use {out ->
            out.write("Name,Estimated Value,Standard Error,CI Low Bound,CI High Bound,p-value,t-statistic,Min,Max,Description")
            out.newLine()
            estimatedRows.forEach {
                out.write(listOf(it.name, it.estimatedValue, it.standardError, it.ciLowerBound, it.ciHighBound,
                it.pvalue, it.tstat, it.description).joinToString(separator = ","))
                out.newLine()
            }
        }
    }

    /**
     * TODO Control params are still one big string. Can fix later.
     */
    private fun createJSONObj(value:String): JsonObject {
        val reader = Json.createReader(StringReader(value))
        return reader.readObject()
    }

    private fun exportJSON(filename: String) {

        val jsobObj = Json.createObjectBuilder()
        jsobObj.add("solver", createJSONObj(solverStr))
        jsobObj.add("optimizer", createJSONObj(optimizerStr))
        jsobObj.add("objectiveFcn", createJSONObj(objectiveFunctionStr))
        jsobObj.add("CI", createJSONObj(ciStr))


        File(filename).bufferedWriter().use {out ->
            out.write(jsobObj.build().toString())
            out.newLine()
            // TODO Add actual estimated values.
        }
    }


    /**
     * Export estimation result. Saving as a JSON file will include the algorithms and parameters used
     * in the estimation.
     */
    fun exportEstimationResults(filename:String, asCSV: Boolean = true)  = if (asCSV) exportCSV(filename) else exportJSON(filename)

    companion object {
        fun loadEstimationResult (paramSet:KParameterSet): EstimationCIResults {
            return if (paramSet.model.modelID < 0) {
                EstimationCIResults(paramSet)
            } else {
                DBLoadMethods.instance.loadEstimationResult(paramSet)
            }
        }
    }
}