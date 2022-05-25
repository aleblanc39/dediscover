package org.woodhill.ded.ui.view.estimation

import edu.rochester.urmc.cbim.jni.JSolver
import javafx.scene.control.ScrollPane
import javafx.scene.control.TitledPane
import javafx.scene.layout.Pane
import javafx.scene.layout.Priority
import org.woodhill.ded.properties.AlgorithmProperty
import org.woodhill.ded.ui.threading.performEstimation
import org.woodhill.ded.ui.view.EquationsChangeEvent
import org.woodhill.ded.ui.view.util.getAlgorithmForm
import org.woodhill.ded.ui.view.workspace.DEDTabView
import tornadofx.*

class EstimationMethodsView : DEDTabView("Est: Methods") {
    private val estimationTabProperties = model.item.estimationTabProperties
    private var initialized = false;


//    fun afterEstimation(estimationResult: Any) {
//        // TODO Parameter should be an estimation result?
//        println("Estimation done. Must do something.")
//        println(estimationResult::class.simpleName)
//        if (estimationResult is EstimatorResult) {
//            sessionElements.estimationResults.updateResults(estimationResult)
//        }
//    }

    override val root = titledpane("Estimation Methods", collapsible = false) {
        val modelType = if (model.modelObj.value.nbDelayExpressions == 0) "ODE" else "DDE"
        hgrow = Priority.ALWAYS
        vgrow = Priority.ALWAYS
        vbox {
            useMaxWidth = true
            gridpane {
                hboxConstraints {
                    hgrow = Priority.ALWAYS
                    isFillWidth = true
                }
                row {
                    getTitledPane(this, "Objective Function", estimationTabProperties.objectiveFcnAlgm())
                    getTitledPane(this, "Confidence Interval", estimationTabProperties.ciAlgm())
                }
                row {
                    getTitledPane(
                        this,
                        "Solver",
                        estimationTabProperties.solverAlgm(),
                        { x -> JSolver.isAttributeDefined(x, modelType) })
                    getTitledPane(this, "Optimizer", estimationTabProperties.optimizerAlgm())
                }

            }

            hbox {
                button("Estimate") {
                    action {
                        performEstimation(
                            pg(),
                            model.item
                        ) { x -> afterEstimation(x) }
                    }
                    enableWhen(model.item.globalProperties.estimableModelProperty)
                }
            }
        }
    }


    init {
        val solverAlgm = estimationTabProperties.solverAlgm()
        solverAlgm.updateAvailableMethods { x ->
            JSolver.isAttributeDefined(
                x,
                if (model.modelObj.value.nbDelayExpressions > 0) "DDE" else "ODE"
            )
        }
        solverAlgm.updateAlgm(solverAlgm.availableMethods[0])
        subscribe<EquationsChangeEvent> {
            // TODO: This code is duplicated in SimMethodsTabView. Should factor oou.

            val isCurrentDDE = solverAlgm.algmInstance.methodAttributes.asList().contains("DDE")
            if (model.modelObj.value.nbDelayExpressions > 0 && (!isCurrentDDE || !initialized)) {
                solverAlgm.updateAvailableMethods { x -> JSolver.isAttributeDefined(x, "DDE") }
                solverAlgm.updateAlgm(solverAlgm.availableMethods[0])
            }
            if (model.modelObj.value.nbDelayExpressions == 0 && (isCurrentDDE || !initialized)) {
                solverAlgm.updateAvailableMethods { x -> JSolver.isAttributeDefined(x, "ODE") }
                solverAlgm.updateAlgm(solverAlgm.availableMethods[0])
            }
        }
        initialized = true
    }


    override fun onDock() {
        super.onDock()
        disableOnInvalidModel(root)
    }


}

fun getTitledPane(
    pane: Pane,
    name: String,
    algmProperty: AlgorithmProperty,
    filter: (String) -> Boolean = { true }
): TitledPane = pane.titledpane(name, collapsible = false) {
    gridpaneColumnConstraints {
        percentWidth = 50.0
    }

    prefHeight = 600.0
//    minHeight = 300.0
    useMaxHeight = true
    // prefWidth = 450.0
    scrollpane {
        vbarPolicy = ScrollPane.ScrollBarPolicy.ALWAYS
        getAlgorithmForm(this, algmProperty, filter)
    }
}