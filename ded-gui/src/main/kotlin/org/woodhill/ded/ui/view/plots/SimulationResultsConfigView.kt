package org.woodhill.ded.ui.view.plots

import edu.rochester.urmc.cbim.jni.JSolver
import javafx.beans.property.SimpleDoubleProperty
import javafx.scene.control.Alert
import javafx.scene.control.TextField
import javafx.scene.layout.Region
import org.woodhill.ded.ui.threading.performSimulationOnSets
import org.woodhill.ded.ui.view.util.doubletextfield
import org.woodhill.ded.ui.view.util.setBackgroundcolor
import org.woodhill.ded.ui.view.workspace.DEDTabView
import tornadofx.*

class SimulationResultsConfigView : DEDTabView() {
    private val solverTabProperties = model.item.solverTabProperties

    val timeparams = form {
        prefHeight = 30.0
        fieldset {
            hbox(20) {
                field("Start Time") {
                    doubletextfield (simtabProperties.simulationStartTime) {
                        focusedProperty().onChange {
                            processTimechange(this, simtabProperties.simulationStartTime)
                        }
                    }
                }

                field("Stop Time") {
                    doubletextfield (simtabProperties.simulationStopTime) {
                        focusedProperty().onChange {
                            processTimechange(this, simtabProperties.simulationStopTime)
                        }
                    }
                }

                field("Step Size") {
                    doubletextfield(simtabProperties.simulationStepSize) {
                        focusedProperty().onChange {
                            processTimechange(this, simtabProperties.simulationStepSize)
                        }
                    }
                }
            }
        }
    }


    override var root = vbox {
        add(SimulationResultTabs())
        add(timeparams)
    }


    /**
     * Till I find a better solution, define a map prop->value to keep the last value of property
     * to prevent simulating when there were no change
     */

    private val previousPropValues = mutableMapOf<SimpleDoubleProperty, Double>(
        simtabProperties.simulationStartTime to simtabProperties.simulationStartTime.value,
        simtabProperties.simulationStopTime to simtabProperties.simulationStopTime.value,
        simtabProperties.simulationStepSize to simtabProperties.simulationStepSize.value
    )


    /**
     * Will be used by all 3 properties. Only interested when the subject is not focused
     * which may indicate a change of value
     */
    private fun processTimechange(tf: TextField, prop: SimpleDoubleProperty) {
        if (!tf.isFocused) {
            if (!tf.text.filter { e -> e != ',' }.isDouble()) {
                tf.text = previousPropValues[prop].toString()
                setBackgroundcolor(tf)
            } else {
                if (prop.value != previousPropValues[prop]) {
                    if (validadateSimulationTimepoints() && simtabProperties.autoRefreshPlots.value) {
                        performSimulationOnSets(
                            find(),
                            { fireOnSimulationSuccess() },
                            model,
                            solverTabProperties.algmProperties.algmInstance as JSolver,
                            currentParamSetObj()
                        )
                        previousPropValues[prop] = prop.value
                    } else {
                        prop.value = previousPropValues[prop]
                    }
                }
            }
        }
    }
    private fun simulationError(msg:String) {
        val alert = Alert(
            Alert.AlertType.ERROR,
            msg
        )
        alert.dialogPane.minHeight = Region.USE_PREF_SIZE
        alert.headerText = "Simulation Error"
        alert.showAndWait()
    }


    private fun validadateSimulationTimepoints(): Boolean {
        val totalTime = simtabProperties.simulationStopTime.value - simtabProperties.simulationStartTime.value
        if (totalTime <= 0) {
            simulationError("Can not start simulation: Start Time must be smaller that Stop Time")
            return false
        }
        if (simtabProperties.simulationStepSize * 10 > totalTime) {
            simulationError("Can not start simulation: Step size (${simtabProperties.simulationStepSize} " +
                    "is too big for the time span ($totalTime)")
            return false
        }
        return true
    }

}
