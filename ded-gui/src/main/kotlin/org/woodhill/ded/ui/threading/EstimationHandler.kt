package org.woodhill.ded.ui.threading

import edu.rochester.urmc.cbim.jni.JSolverBasedEstimator
import edu.rochester.urmc.cbim.jni.exception.XDEException
import javafx.scene.control.Alert
import javafx.scene.layout.Region
import org.woodhill.ded.runners.EstimationRunner
import org.woodhill.ded.session.SessionElements
import org.woodhill.ded.ui.ProgressController
import tornadofx.*


private fun executeEstimation(sessionElements: SessionElements, pg: ProgressController): Any? {

    sessionElements.updateCovariateMappings()
    val estimatedColumns = sessionElements.mappingInfoSet.getEstimatedColumns()

    try {
        // Quick fix. Set solver timepoints to min, max of timepoints for table.
        pg.messageProperty.value = "Performing estimation"
        // TODO Probably shouldn't be done here. DataGenerator should know to use points in table, assign to solver.
        sessionElements.estimationTabProperties.solver.startTime =
            sessionElements.mappingInfoSet.timepoints().min()!!
        sessionElements.estimationTabProperties.solver.stopTime = sessionElements.mappingInfoSet.timepoints().max()!!
        sessionElements.estimationTabProperties.solver.stepSize =
            (sessionElements.mappingInfoSet.timepoints().max()!! - sessionElements.mappingInfoSet.timepoints()
                .min()!!) / 100.0

        val estimator = JSolverBasedEstimator(
            sessionElements.modelObj,
            sessionElements.estimationTabProperties.getOptimizerObject(),
            sessionElements.estimationTabProperties.getObjFcnObject(),
            sessionElements.estimationTabProperties.solver,
            sessionElements.mappingInfoSet.timepoints(),
            estimatedColumns.map {
                it.formula
            },
            sessionElements.mappingInfoSet.getDataForEstimation()
        )
        pg.cancelFunction = { estimator.optimizer.interruptThread() }

        val estimationRunner = EstimationRunner(estimator)

        val estimatorResult = estimationRunner.estimate(sessionElements.estimationParameterSet)

        pg.messageProperty.value = "Performing CI Computation"

        val ci = sessionElements.estimationTabProperties.getCIObject()
        ci.assignObjects(
            sessionElements.estimationParameterSet.jniParameterSet(),
            estimatorResult,
            sessionElements.modelObj,
            sessionElements.estimationTabProperties.solver,
            sessionElements.estimationTabProperties.getOptimizerObject(),
            sessionElements.mappingInfoSet.timepoints(),
            estimatedColumns.map {
                it.formula
            },
            sessionElements.mappingInfoSet.getDataForEstimation()
        )
        val ciResults = ci.run()

        return Pair(estimatorResult, ciResults)

    } catch (e: Exception) {

        println("GOT AN EXCEPTION")
        return e
    }
}


fun performEstimation(
    pg: ProgressController,
    sessionElements: SessionElements,
    actionOnSuccess: (Any) -> Unit
) {
    var alertType = Alert.AlertType.INFORMATION
    var finalMessage = "Estimation completed"
    var result: Any? = ""
    var estimationSuccess = true
    runAsync {
        result = executeEstimation(sessionElements, pg)

        if (result is Exception) {
            alertType = Alert.AlertType.ERROR
            finalMessage = (result as Exception).message!!
            estimationSuccess = false
        }
        else if (result !is Pair<*, *>) {
            alertType = Alert.AlertType.ERROR
            finalMessage = "An error has occured"
            estimationSuccess = false
        }
    } ui {

        if (pg.isCancelled) {
            estimationSuccess = false
            finalMessage = "Estimation cancelled by user"
        }

    } fail {
        estimationSuccess = false
        alertType = Alert.AlertType.ERROR
        finalMessage = "Estimation failed."
    } cancel {
        estimationSuccess = false
        finalMessage = "Estimation failed."

    } finally {
        pg.close()

        val alert = Alert(
            alertType,
            finalMessage
        )
        alert.dialogPane.minHeight = Region.USE_PREF_SIZE
        alert.show()

        if (estimationSuccess)
            actionOnSuccess(result!!)
    }
    pg.openModal()
}