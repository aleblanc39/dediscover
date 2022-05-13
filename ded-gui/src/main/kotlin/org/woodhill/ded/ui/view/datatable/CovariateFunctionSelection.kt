package org.woodhill.ded.ui.view.datatable

import edu.rochester.urmc.cbim.jni.JTimeVaryingFunction
import javafx.beans.property.SimpleStringProperty
import org.woodhill.ded.models.TVFunctionMapping
import org.woodhill.ded.session.SessionViewModel
import tornadofx.*

class CovariateFunctionSelection(
    model: SessionViewModel,
    currentName: String,
    currentParams: Map<String, String>
) : Fragment("Select Function") {

    // Have a map of defined functions in case a user keeps changing his/her name
    val createdFunctions = mutableMapOf<String, JTimeVaryingFunction>()
    private val availableMethods = JTimeVaryingFunction.s_getAvailableMethods().asList().asObservable()
    val selectedName = SimpleStringProperty(currentName)

    val functionCurrentlyDefined: Boolean = currentName.trim().length != 0
    var currentTVFunction = if (functionCurrentlyDefined) {
        val x = 0
        val tvf = JTimeVaryingFunction(currentName)

        // Will handle the case where some params may be missing
        tvf.controlParameters.forEach {
            if (currentParams.containsKey(it.name)) {
                it.value = currentParams[it.name]
            }
        }
        tvf
    } else {
        JTimeVaryingFunction()
    }

    val currentDescription = if (functionCurrentlyDefined) currentTVFunction.longMethodDescription else "Description..."
    val currentType = if (functionCurrentlyDefined) currentTVFunction.shortMethodDescription else "Type..."

    val typeProperty = SimpleStringProperty(currentType)
    val descriptionProperty = SimpleStringProperty(currentDescription)

    // The number of properties for control parameters will depend on actual function.
    val controlParamProperties = mutableListOf<SimpleStringProperty>()


    var acceptChanges = false
    override val root = vbox() {
        form {
            fieldset {
                field("Name:") {
                    combobox(selectedName, availableMethods) { }
                }
                field("Type:") {
                    textfield(currentType) {
                        editableProperty().set(false)
                    }.bind(typeProperty)
                }

                field("Description:") {
                    textfield(currentDescription) {
                        editableProperty().set(false)
                    }.bind(descriptionProperty)
                }
            }
        }
    }

    init {
        selectedName.onChange {
            currentTVFunction = JTimeVaryingFunction(selectedName.value)
            typeProperty.set(currentTVFunction.shortMethodDescription)
            descriptionProperty.set(currentTVFunction.longMethodDescription)

            root.children[0].getChildList()?.set(1, addControlParams(currentTVFunction))

        }

        root.children[0].add(addControlParams(currentTVFunction))
        root.children[0].add(
            hbox {
                button("Accept").action {
                    acceptChanges = true
                    close()
                }
                button("Cancel").action {
                    close()
                }
            }
        )
    }

    private fun addControlParams(tvFunction: JTimeVaryingFunction): Fieldset {

        controlParamProperties.clear()
        return fieldset("Function Parameters:") {
            tvFunction.controlParameters.asList().forEach {
                // TODO The time varying functions must hold the values.

                val controlParamValue = it.value.toString()
                controlParamProperties.add(SimpleStringProperty(controlParamValue))
                field(it.name) {
                    textfield(it.value.toString()).bind(controlParamProperties.last())
                }
            }
        }
    }

    companion object {
        fun getTVFunction(
            model: SessionViewModel,
            currentMapping: TVFunctionMapping
        ): Pair<String, List<Pair<String, String>>> {
            println("Creating selection window object")
            val selectionWindow = CovariateFunctionSelection(model, currentMapping.tvFunctionName, currentMapping.getControlParamsMap())
            println("Time to wait")

            selectionWindow.openModal(block = true)
            selectionWindow.controlParamProperties.forEach{
                println(it.value)
            }
            if (selectionWindow.acceptChanges) {
                val controlParams = mutableListOf<Pair<String, String>>()
                for (p in selectionWindow.currentTVFunction.controlParameters.zip(selectionWindow.controlParamProperties)) {
                    controlParams.add(Pair(p.first.name, p.second.value))
                }
                return Pair(selectionWindow.currentTVFunction.methodName, controlParams)
            } else {
                // Cancelled or window closed
                //return Pair(currentMapping.tvFunctionName, parseControlParameters(currentMapping.controlParamsJson).toList())
                return Pair(currentMapping.tvFunctionName, currentMapping.getControlParamsMap().toList())
            }
        }
    }
}