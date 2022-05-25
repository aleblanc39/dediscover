package org.woodhill.ded.ui.view.workspace

import edu.rochester.urmc.cbim.jni.EstimatorResult
import edu.rochester.urmc.cbim.jni.JConfidenceIntervalResult
import edu.rochester.urmc.cbim.jni.Threadable
import javafx.beans.property.SimpleBooleanProperty
import javafx.scene.Node
import org.woodhill.ded.session.SessionElements
import org.woodhill.ded.ui.view.ProgressController
import org.woodhill.ded.ui.view.MenuAffectingEvent
import org.woodhill.ded.ui.view.PlotsUpdatingEvent
import org.woodhill.ded.ui.view.SimulationDoneEvent
import tornadofx.View
import tornadofx.onChange

abstract class DEDTabView( title: String = "") : View(title) {
    override val scope = super.scope as TabbedModelsView.SessionScope
    val model = scope.model
    val sessionElements: SessionElements = scope.model.item

    // Pause and Resume always communicated from model object. Interrupt can be communicated from model or algorithm
    fun pg(cancelThreadable:Threadable = model.modelObj.value) = ProgressController.getProgressControler(scope, pauseFunction = {model.modelObj.value.pauseThread() },
        cancelFunction =  {cancelThreadable.interruptThread() },
        resumeFunction = {model.modelObj.value.resumeThread() }
        )

    val simtabProperties = model.item.simTabProperties
    fun currentParamSet() = simtabProperties.currentParameterSetProperty.value
    fun currentParamSetObj() = model.item.parameterSetByName(currentParamSet())

    fun disableOnInvalidModel(root: Node) {
        root.isDisable = !model.item.globalProperties.validModel
        model.item.globalProperties.validModelProperty.onChange {
            root.isDisable = !model.item.globalProperties.validModel
        }
    }

    protected fun fireOnSimulationSuccess()  {
        fire(PlotsUpdatingEvent(scope))
        fire(MenuAffectingEvent())
        fire(SimulationDoneEvent())

    }


    protected fun afterEstimation(estimationResult: Any) {
        val emt = sessionElements.estimationTabProperties

        if (estimationResult is Pair<*,*>) {
            sessionElements.estimationResultsProperties.hasValidResultsProperty.value = true
            sessionElements.estimationResults.updateResults(estimationResult as Pair<EstimatorResult, JConfidenceIntervalResult>, emt.solverAlgm(), emt.ciAlgm(), emt.optimizerAlgm(), emt.objectiveFcnAlgm())
            scope.tabPane.selectionModel.select(SessionTopView.TABINDEX.ESTIMATION_RESULTS.index)

        } else {
            // TODO Need to be more detailed here. Estimation may have been canceled.
            //sessionElements.estimationResultsProperties.hasValidResultsProperty.value = false
        }
    }

    /**
     * Input as a variable number of properties. Will disable the node if at least one is false.
     */
    fun disableOnProperty(root: Node, vararg props: SimpleBooleanProperty) {

        fun disabled():Boolean = props.map{it.value}.contains(false)

        root.isDisable = disabled()
        for (prop in props) {
            prop.onChange {
                root.isDisable = disabled()
            }
        }
    }
}