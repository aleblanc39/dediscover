package org.woodhill.ded.models

import edu.rochester.urmc.cbim.jni.JTimeVaryingFunction
import javafx.beans.property.SimpleBooleanProperty
import org.woodhill.ded.util.DBLoadMethods
import org.woodhill.ded.util.stringPairsToJson
import org.woodhill.ded.util.jsonControlParamsToMap
import tornadofx.getValue
import tornadofx.setValue

/**
 * The TVFunctionMapping keeps the information about the function name and the control params, but not the function object
 * itself. Name and control params may change. Created from json string, but then all operations based on map.
 */
class TVFunctionMapping(var id: Int, val covariate:KCovariate, var tvFunctionName: String, var controlParamsJson: String, val tableColumns:MutableList<KMappingInfo> = mutableListOf(), used: Boolean = false) {


    val usedProperty = SimpleBooleanProperty(used)
    var used by usedProperty

    var tvFunction = JTimeVaryingFunction(tvFunctionName)
    private val controlParamsMap = jsonControlParamsToMap(controlParamsJson).toMutableMap()

    fun clearControlParams() = controlParamsMap.clear()
    fun addControlParams(newParams: List<Pair<String, String>>) {
        newParams.forEach{
            controlParamsMap[it.first] = it.second
        }
    }
    fun getControlParamsMap() = controlParamsMap.toMap()

    // TODO Consolidate will all the objects that have control params.
    fun getControlParamsAsJson() = stringPairsToJson(controlParamsMap.toList())
    constructor(covariate:KCovariate):this(nextId(), covariate, "", "", mutableListOf(), false)



    override fun toString(): String {
        if (tvFunctionName.isEmpty())
            return "(None Defined)"
        return "$tvFunctionName(" + getControlParamsMap()
            .map {
                it.key + ":" + it.value
            }.toString() + ")"
    }

    fun clone(cov:KCovariate):TVFunctionMapping {
        return TVFunctionMapping(nextId(), cov, tvFunctionName, controlParamsJson, tableColumns, used)

    }


    companion object {
        private var id:Int = -1
        private fun nextId():Int = id--

        private val tvFunctionMappings = mutableMapOf<KCovariate, TVFunctionMapping>()




        fun getMappingForCovariate(covariate: KCovariate): TVFunctionMapping =
             tvFunctionMappings.getOrPut(covariate) {
                if (covariate.id < 0) TVFunctionMapping(covariate) else DBLoadMethods.instance.dbLoadTVFunctionMapping(covariate)
            }
    }
}