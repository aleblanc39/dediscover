package org.woodhill.ded.sbml

import edu.rochester.urmc.cbim.jni.JDDEModel
import org.woodhill.ded.models.EstimableParameterValue
import org.woodhill.ded.models.EstimableSymbol
import org.woodhill.ded.models.KModel
import org.woodhill.ded.models.KParameterSet
import org.woodhill.ded.util.DBSaveMethods


fun assignParamValue(
    estimableSymbol: EstimableSymbol,
    paramSet: KParameterSet,
    value: Double
): EstimableParameterValue {
    val usedValue = if (value == Double.NaN) 0.0 else value
    val minValue = if (usedValue > 0.0) 0.0 else 2 * usedValue
    val maxValue = if (usedValue <= 0.0) 1.0 else 2 * usedValue

    return EstimableParameterValue(
        parameterSet = paramSet, modelSymbol = estimableSymbol, default = usedValue,
        minValue = minValue, maxValue = maxValue, slider = true, estimated = false
    )
}


class XDESessionBuilder(sbmlParser: SBMLFileParser, sessionName: String, sessionTitle: String, packageName: String) {

    val kmodel = KModel(
        sessionName = sessionName,
        title = sessionTitle,
        packageName = packageName,
        modelID = -1,
        equations = sbmlParser.createEquations()
    )

    val paramSet = KParameterSet.getOrCreateParameterSet(kmodel, "Default")
    private val modelParamValuesMap = sbmlParser.getModelParamValues()
    private val initialConditionsValuesMap = sbmlParser.getInitialConditionValues()

    val modelObj = JDDEModel(kmodel.equations)
    private val modelParameters =
        modelObj.jModelParameters.map { EstimableSymbol(-1, kmodel, it.name, it.firstOccurenceLine, "") }
    private val initialConditions =
        modelObj.jInitialConditions.map { EstimableSymbol(-1, kmodel, it.name, it.firstOccurenceLine, "") }


    val modelParameterValues = modelParameters.map {
        println("Assigning model param: ${it.name}")
        assignParamValue(it, paramSet, modelParamValuesMap[it.name]!!)
    }

    private val initialConditionValues = initialConditions.map {
        assignParamValue(it, paramSet, initialConditionsValuesMap[it.name]!!)
    }

    fun saveModel() {
        kmodel.save()
        DBSaveMethods.instance.saveModelParameters(kmodel, modelParameters)
        DBSaveMethods.instance.saveDependentVariables(kmodel, initialConditions)

        paramSet.replaceValues(modelParameterValues, initialConditionValues)
        DBSaveMethods.instance.saveParameterSet(paramSet)
    }

}