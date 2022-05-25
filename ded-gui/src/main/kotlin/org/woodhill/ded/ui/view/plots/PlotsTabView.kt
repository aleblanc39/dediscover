package org.woodhill.ded.ui.view.plots

import edu.rochester.urmc.cbim.jni.JSolver
import org.woodhill.ded.ui.view.ProgressController
import org.woodhill.ded.ui.threading.performSimulationOnSets
import org.woodhill.ded.ui.view.*
import org.woodhill.ded.ui.view.workspace.DEDTabView
import tornadofx.*

class PlotsTabView: DEDTabView("Plots") {

    // TODO Replace with hbox
    var simulationRequired:Boolean = false
    override val root = borderpane() {
        left<PlottingParametersView>()
        center<SimulationResultsConfigView>()
    }

    override fun onTabSelected() {
        super.onTabSelected()
        if (!root.isDisabled) {
            if (!model.item.solverResults.filter { it.parameterSet == currentParamSetObj() }.isNotEmpty() || (simulationRequired && simtabProperties.autoRefreshPlots.value)) {
                performSimulationOnSets(
                    //find(),
                    ProgressController.getProgressControler(scope,{},{},{}),
                    { fireOnSimulationSuccess() },
                    model,
                    model.item.solverTabProperties.algmProperties.algmInstance!! as JSolver,
                    currentParamSetObj()
                )
            }
        }
        simulationRequired = false
    }



    override fun onDock() {
        super.onDock()
        disableOnInvalidModel(root)
    }

    init {

        subscribe<SimulationDoneEvent> {
            simulationRequired = false
        }

        subscribe<ResimulateEvent> {
            simulationRequired = true
        }
        subscribe<DatatatableStructureChangeEvent> {
            simulationRequired = true
        }
        subscribe<CovariateMappingChanged> {
            // TODO May not always be required
            simulationRequired = true
        }
        subscribe<DataMappingChangeEvent> {
            simulationRequired = true
        }
    }

}