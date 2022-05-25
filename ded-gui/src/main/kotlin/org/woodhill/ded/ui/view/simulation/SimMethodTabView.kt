package org.woodhill.ded.ui.view.simulation

import edu.rochester.urmc.cbim.jni.JSolver
import org.woodhill.ded.ui.threading.performSimulationOnSets
import org.woodhill.ded.ui.view.EquationsChangeEvent
import org.woodhill.ded.ui.view.util.getAlgorithmForm
import org.woodhill.ded.ui.view.workspace.DEDTabView
import tornadofx.action
import tornadofx.button
import tornadofx.hbox
import tornadofx.titledpane

class SimMethodTabView() : DEDTabView("Sim: Methods") {

    val solverTabProperties = model.item.solverTabProperties
    //fun pg() = ProgressController.getProgressControler(scope, {},{},{})

    private val simtabPoperties = model.item.simTabProperties
    private var initialized = false;
    override val root = titledpane("Solver", collapsible = false) {
        val eqType: String = if (model.modelObj.value.nbDelayExpressions == 0) "ODE" else "DDE"
        solverTabProperties.algmProperties.updateAvailableMethods { x -> JSolver.isAttributeDefined(x, eqType) }
        getAlgorithmForm(this, solverTabProperties.algmProperties)
        hbox {
            button("Simulate").apply {
                action {
                    performSimulationOnSets(
                        pg(),
                        { fireOnSimulationSuccess() },
                        model,
                        solverTabProperties.algmProperties.algmInstance!! as JSolver,
                        model.item.parameterSetByName(simtabPoperties.currentParameterSetProperty.value)
                    )
                }
            }
        }
    }

    init {
        solverTabProperties.algmProperties.updateAvailableMethods { x -> JSolver.isAttributeDefined(x, if (model.modelObj.value.nbDelayExpressions>0) "DDE" else "ODE" )}
        solverTabProperties.algmProperties.updateAlgm(solverTabProperties.algmProperties.availableMethods[0])

        subscribe<EquationsChangeEvent> {
            System.err.println("Equations Changed Event in SimMethodsTabView")
            val isCurrentDDE = solverTabProperties.algmProperties.algmInstance.methodAttributes.asList().contains("DDE")
            if (model.modelObj.value.nbDelayExpressions > 0 && (!isCurrentDDE || !initialized)) {
                solverTabProperties.algmProperties.updateAvailableMethods { x -> JSolver.isAttributeDefined(x, "DDE") }
                solverTabProperties.algmProperties.updateAlgm(solverTabProperties.algmProperties.availableMethods[0])
            }
            if (model.modelObj.value.nbDelayExpressions == 0 && (isCurrentDDE || !initialized)) {
                solverTabProperties.algmProperties.updateAvailableMethods { x -> JSolver.isAttributeDefined(x, "ODE") }
                solverTabProperties.algmProperties.updateAlgm(solverTabProperties.algmProperties.availableMethods[0])
            }
            initialized = true
        }
    }


    override fun onDock() {
        super.onDock()
        disableOnInvalidModel(root)
    }
}