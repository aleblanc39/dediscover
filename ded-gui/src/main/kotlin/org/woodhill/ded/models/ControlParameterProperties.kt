package org.woodhill.ded.models

import edu.rochester.urmc.cbim.jni.Algorithm
import edu.rochester.urmc.cbim.jni.JGeneralParameter
import javafx.beans.property.*
import javafx.event.EventTarget
import javafx.scene.control.Alert
import javafx.scene.control.Control
import javafx.scene.layout.Region
import org.woodhill.ded.ui.view.util.doubletextfield
import org.woodhill.ded.ui.view.util.integertextfield
import org.woodhill.ded.util.parseControlParameters
import org.woodhill.ded.util.stringPairsToJson
import tornadofx.*
import kotlin.reflect.KClass

/**
 * Class to handle control parameters present in many circumstances.
 */

//class ControlParameterProperties(controlParams: List<JGeneralParameter> = mutableListOf(), valuesAsJson: String = "") {
class ControlParameterProperties(clazz: KClass<out Algorithm>, algmName: String, valuesAsJson: String = "") {

    private val paramProperties = mutableMapOf<String, Any>()
    private val _controlParams = Algorithm.getControlParameters(clazz.java, algmName)

    fun paramValues(): List<Pair<String, Any>> = paramProperties.toList()


    /**
     * Update the list of control parameters when the algorithm selection has changed.
     * TODO Maybe remove this functionality, create a new object every time.
     */
    fun updateControlParams(controlParams: List<JGeneralParameter>) {
        _controlParams.clear()
        _controlParams.addAll(controlParams)
        paramProperties.clear()

        _controlParams.forEach {
            when (it.value.javaClass.kotlin) {
                Boolean::class -> paramProperties[it.name] = SimpleBooleanProperty(it.value as Boolean)
                Double::class -> paramProperties[it.name] = SimpleDoubleProperty(it.value as Double)
                Int::class -> paramProperties[it.name] = SimpleIntegerProperty(it.value as Int)
                else -> paramProperties[it.name] = SimpleStringProperty(it.value as String)
            }
        }
    }

    fun assignDefaults() {
        _controlParams.forEach {
            //(paramProperties[it.name] as SimpleStringProperty).value = it.default.toString()
            (paramProperties[it.name] as Property<*>).value = it.default
        }
    }


//    fun getFieldset(ev:EventTarget): Fieldset {
//        println("RETURNING THE PARAMS")
//        return ev.fieldset("Control Parameters") {
//            // TODO Won't always be a string. There may be selections.
//            // TODO:
//            _controlParams.forEach {
//                field(it.name) {
//                    textfield ( (paramProperties[it.name] as SimpleStringProperty ).value){}.bind(paramProperties[it.name] as SimpleStringProperty)
//                }
//            }
//        }
//    }


//    fun <T: Comparable<T> > Node.validateBounds(param: JGeneralParameter, paramProperty: SimpleObjectProperty<T>) {
//
//        val min: String = if (param.hasMin()) param.min.toString() else "-INF"
//        val max: String = if (param.hasMax()) param.min.toString() else "+INF"
//
//        val lastValue = param.value as T
//        if (!isFocused) {
//            //if ((param.hasMax() && ( (paramProperty.value as T) < (param.max as Nothing)))
//            if ((param.hasMax() && (paramProperty.value.compareTo(param.max as T) < 0 )          )
//                || (param.hasMin() && (paramProperty.value.compareTo(param.max as T) < 0 ))
//            ) {
//                val alert = Alert(
//                    Alert.AlertType.ERROR,
//                    //header = "Out of range",
//                    "Value for parameter ${param.name} must be in the range\n" +
//                            "[${min}, ${max}]"
//                )
//                alert.dialogPane.minHeight = Region.USE_PREF_SIZE
//                alert.show()
//                (paramProperties[param.name] as SimpleObjectProperty<T>).value = lastValue
//            }
//        }
//    }


    private fun Field.createParamInput(param: JGeneralParameter): Control {

        if (param.value.javaClass.kotlin == Boolean::class) {
            return checkbox(property = paramProperties[param.name] as SimpleBooleanProperty) {
            }
        }

        if (param.value.javaClass.kotlin == Double::class) {
            val min: String = if (param.hasMin()) param.min.toString() else "(No Minimum)"
            val max: String = if (param.hasMax()) param.min.toString() else "(No Maximum)"

            return doubletextfield(property = paramProperties[param.name] as SimpleDoubleProperty) {
                val lastValue = param.value as Double
                focusedProperty().onChange {
                    // Because SimpleDoubleProperty and SimpleIntegerProperty don't have much in common, we'll repeat this code below.
                    if (!isFocused) {
                        if ((param.hasMax() && ((paramProperties[param.name] as SimpleDoubleProperty).value ) > param.max as Double)
                            || (param.hasMin() && ((paramProperties[param.name] as SimpleDoubleProperty).value ) < param.min as Double)
                        ) {
                            val alert = Alert(
                                Alert.AlertType.ERROR,
                                //header = "Out of range",
                                "Value for parameter ${param.name} must be in the range\n" +
                                        "[${min}, ${max}]"
                            )
                            alert.dialogPane.minHeight = Region.USE_PREF_SIZE
                            alert.show()
                            (paramProperties[param.name] as SimpleDoubleProperty).value = lastValue
                        }
                    }
                }
            }
        }

        if (param.value.javaClass.kotlin == Int::class) {
            val min: String = if (param.hasMin()) param.min.toString() else "(No Minimum)"
            val max: String = if (param.hasMax()) param.min.toString() else "(No Maximum)"
            val lastValue = param.value as Int

            if (param.options.isEmpty()) {
                return integertextfield(property = paramProperties[param.name] as SimpleIntegerProperty) {

                    focusedProperty().onChange {
                        if (!isFocused) {
                            if ((param.hasMax() && ((paramProperties[param.name] as SimpleIntegerProperty).value) > param.max as Int)
                                || (param.hasMin() && ((paramProperties[param.name] as SimpleIntegerProperty).value) < param.min as Int)
                            ) {
                                val alert = Alert(
                                    Alert.AlertType.ERROR,
                                    //header = "Out of range",
                                    "Value for parameter ${param.name} must be in the range\n" +
                                            "[${min}, ${max}]"
                                )
                                alert.dialogPane.minHeight = Region.USE_PREF_SIZE
                                alert.show()
                                (paramProperties[param.name] as SimpleIntegerProperty).value = lastValue
                            }
                        }
                    }
                }
            } else {
                return combobox (property = paramProperties[param.name] as SimpleIntegerProperty, values = param.options.map {

                    // The underlying code currently returns the options as Strings
                    (it.toString()).toInt()
                }) {

                }
            }





        }


        // Have a String. Must look for options.
        return combobox (property = paramProperties[param.name] as SimpleStringProperty, values = param.options.map {
            it.toString()
        }) {

        }
    }


    fun addToFieldset(f: Fieldset, ev: EventTarget) {
        _controlParams.forEach {
            f.field(it.name) {
                createParamInput(it)

                //textfield ( (paramProperties[it.name] as SimpleStringProperty ).value){}.bind(paramProperties[it.name] as SimpleStringProperty)
            }
        }
    }


    private fun assignValuesAsJson(jsonString: String) {
        val valuesMap = parseControlParameters(jsonString)
        valuesMap.forEach { entry ->
            //(paramProperties[entry.key] as SimpleStringProperty).value = entry.value
            (paramProperties[entry.key] as Property<*>).value = when (paramProperties[entry.key]!!::class) {
                SimpleBooleanProperty::class -> entry.value.toBoolean()
                SimpleDoubleProperty::class -> entry.value.toDouble()
                SimpleIntegerProperty::class -> entry.value.toInt()
                else -> entry.value
            }
        }
    }

    fun paramsAsJson(): String = stringPairsToJson(paramProperties.map { it ->
        Pair(
            it.key,
            (it.value as Property<*>).value.toString()
        )
    }.toList())

    init {
        updateControlParams(Algorithm.getControlParameters(clazz.java, algmName))
        if (valuesAsJson == "") {
            assignDefaults()
        } else {
            assignValuesAsJson(valuesAsJson)
        }
    }
}


