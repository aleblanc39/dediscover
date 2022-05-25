package org.woodhill.ded.properties

import edu.rochester.urmc.cbim.jni.Algorithm
import javafx.beans.property.SimpleStringProperty
import org.woodhill.ded.models.ControlParameterProperties
import java.io.StringReader
import javax.json.Json
import kotlin.reflect.KClass
import tornadofx.getValue
import tornadofx.observableListOf
import tornadofx.setValue

class  AlgorithmProperty(algm: String, val clazz: KClass<out Algorithm>, controlParamsAsJson: String) {

    enum class JSON_FIELDS(val key:String) {
        NAME("name"),
        CONTROL_PARAMS("control_params")
    }

    val nameProperty = SimpleStringProperty(algm)
    var name by nameProperty

    var algmInstance = Algorithm.createMethod(clazz.java, algm)
    val controlParamsProperties =  ControlParameterProperties(algmInstance.controlParameters.asList(), controlParamsAsJson)


    val algmLongDescProperty = SimpleStringProperty("")
    var algmLongDesc by algmLongDescProperty

    val algmShortDescProperty = SimpleStringProperty("")
    var algmShortDesc by algmShortDescProperty

    private val selectedAlgmProperty = SimpleStringProperty("")

    val availableMethods = observableListOf<String>()


    fun updateAvailableMethods(filter:(m:String)->Boolean = {true}   ) {
        availableMethods.clear()
        availableMethods.addAll(Algorithm.getAvailableMethods(clazz.java).filter{filter(it)})
    }

    fun setDescriptions() {
        algmShortDesc = algmInstance.shortMethodDescription
        algmLongDesc = algmInstance.longMethodDescription
    }


    fun toJsonString():String {
        val jsonObj = Json.createObjectBuilder()
        jsonObj.add(JSON_FIELDS.NAME.key, name)
        jsonObj.add(JSON_FIELDS.CONTROL_PARAMS.key, controlParamsProperties.paramsAsJson())
        return jsonObj.build().toString()
    }

    /**
     * Needs to be calles to update the algorithm and parameters. Don't do based on property change
     * because another process needs to wait for this one to be completed.
     */
    fun updateAlgm() {
        if (name == null)
            return
        algmInstance = Algorithm.createMethod(clazz.java, name)
        controlParamsProperties.updateControlParams(algmInstance.controlParameters.asList())
        setDescriptions()
    }

    fun updateAlgm(newName: String) {
        nameProperty.value = newName
        updateAlgm()
    }

    fun resetControlParameters() {
        controlParamsProperties.assignDefaults()
    }

    init {
        setDescriptions()
    }

    companion object {
        fun createAlgmProperties(algmPropsAsJSon: String, clazz:KClass<out Algorithm>): AlgorithmProperty {
            val jsonReader = Json.createReader(StringReader(algmPropsAsJSon))
            val jsonObj = jsonReader.readObject()

            val algObject = AlgorithmProperty(jsonObj.getString(JSON_FIELDS.NAME.key), clazz, jsonObj.getString(JSON_FIELDS.CONTROL_PARAMS.key))
            return algObject
        }
    }
}