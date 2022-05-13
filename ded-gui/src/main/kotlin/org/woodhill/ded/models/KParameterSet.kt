package org.woodhill.ded.models

import edu.rochester.urmc.cbim.jni.JNIParameterSet
import edu.rochester.urmc.cbim.jni.ParameterValue
import javafx.collections.ObservableList
import org.woodhill.ded.util.DBLoadMethods.Companion.instance
import tornadofx.observableListOf


/**
 * A negative valID indicates that this parameter set is not in the store, a negative modelID
 * indicates that this parameter set is part of a new session
 *
 */
class KParameterSet(var id: Int, val name: String, val model: KModel) {

    private val _initialConditionsValues = observableListOf<EstimableParameterValue>()
    val initialConditionsValues: ObservableList<EstimableParameterValue> get() = _initialConditionsValues

    private val _modelParametersValues = observableListOf<EstimableParameterValue>()
    val modelParameterValues: ObservableList<EstimableParameterValue> get() = _modelParametersValues

    fun getParamWithName(name:String):EstimableParameterValue = _initialConditionsValues.union(_modelParametersValues).first { it.nameProperty.value == name }
    fun allParameterValues() =  _initialConditionsValues.union(_modelParametersValues)

    // Only used when duplicating model
    constructor(
        name: String,
        model: KModel,
        modelParams: List<EstimableSymbol>,
        initialConditions: List<EstimableSymbol>
    ) : this(
        nextID(),
        name,
        model
    ) {
        updateSymbols(modelParams, initialConditions)
    }

    init {
        if (id >= 0) {
            _initialConditionsValues.addAll(instance.loadInitialConditionsValues(this))
            _modelParametersValues.addAll(instance.loadModelParametersValues(this))
        }
    }

    /**
     * Looks a lot like SessionUtils.synchronizeModelsymbols. Should consolidate.
     */
    private fun updateEstimatedParameterValues(currentList:MutableList<EstimableParameterValue>, newList:List<EstimableSymbol>, fromStorages: List<EstimableParameterValue>) {
        currentList.removeIf { curr -> !newList.contains(curr.modelSymbol) }
        val symbolsInCurrent = currentList.map { it.modelSymbol }.toSet()

        newList.filter{!symbolsInCurrent.contains(it)}.forEach { modelSymbol->
            val recInStorage = fromStorages.findLast { it.modelSymbol == modelSymbol }
            if (recInStorage != null) {
                currentList.add(recInStorage)
            } else {
                currentList.add(EstimableParameterValue(this, modelSymbol))
            }
        }
    }

    fun updateSymbols(modelParams: List<EstimableSymbol>, initialConditions: List<EstimableSymbol>) {
        updateEstimatedParameterValues(_modelParametersValues, modelParams, instance.loadModelParametersValues(this))
        updateEstimatedParameterValues(_initialConditionsValues, initialConditions, instance.loadInitialConditionsValues(this))
    }

    // TODO Rename? Currently only used when duplicating a parameter set. So there would be no need for clearing. Also
    // updateModelSymbols may fill the same purpose.
    fun replaceValues(newModelParameterValues:List<EstimableParameterValue>, newInitialConditionValues:List<EstimableParameterValue>) {
        _initialConditionsValues.clear()
        _initialConditionsValues.addAll(newInitialConditionValues)
        _modelParametersValues.clear()
        _modelParametersValues.addAll(newModelParameterValues)
    }

    fun JNIParamValue(p: EstimableParameterValue):ParameterValue = ParameterValue(p.modelSymbol.name, p.minValue, p.maxValue, p.defaultValue, p.estimated)
    fun jniParameterSet():JNIParameterSet {
        val ret = JNIParameterSet()
        _modelParametersValues.forEach{ret.setParameterValues(JNIParamValue(it))}
        _initialConditionsValues.forEach{ret.setParameterValues(JNIParamValue(it))}
        return ret
    }

    companion object {
        fun nextID(): Int {
            return -1
        }

        fun getOrCreateParameterSet(model: KModel, paramSetName: String): KParameterSet {
            val sets = instance.loadParameterSets(model)
            if (!sets.keys.contains(paramSetName)) {
                sets[paramSetName] = createParameterSet(model, paramSetName)
            }
            return sets[paramSetName]!!
        }

        fun getParameterSet(model: KModel, paramSetName: String): KParameterSet? {
            return parameterSetsForModel(model)[paramSetName]
        }

        fun parameterSetsForModel(model: KModel) = instance.loadParameterSets(model)

        var newParamSetID: Int = -1

        @JvmStatic
        private fun createParameterSet(model: KModel, paramSetName: String): KParameterSet {
            val paramSet = KParameterSet(newParamSetID, paramSetName, model)
            newParamSetID--
            return paramSet
        }
    }
}