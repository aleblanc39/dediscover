package org.woodhill.ded.ui.threading

import edu.rochester.urmc.cbim.jni.JSolver
import edu.rochester.urmc.cbim.jni.JXDECSVFile
import edu.rochester.urmc.cbim.jni.SolverResult
import javafx.scene.control.Alert
import org.woodhill.ded.models.KMappingInfo
import org.woodhill.ded.models.KParameterSet
import org.woodhill.ded.runners.SimulationRunner
import org.woodhill.ded.session.SessionViewModel
import org.woodhill.ded.ui.ProgressController
import org.woodhill.ded.ui.view.plots.updateSolverResultMap
import tornadofx.*
import java.lang.Exception


/**
 * Class to handle model simulation. Will create the thread where the simulation will run.
 *
 * In the original code there was the SimulationThreadWorker and SimulationRunnerModel
 *
 * TODO Create a thread. Here? Create separate class to handle the popup.
 *
 */

data class SimulationResult(val simulation:SolverResult, val macros:JXDECSVFile = JXDECSVFile(arrayOf<String>(), DoubleArray(0),
    arrayOf<DoubleArray>() ), val formulas: JXDECSVFile = JXDECSVFile(arrayOf<String>(), DoubleArray(0),
    arrayOf<DoubleArray>() ))


private fun performSimulation(pg: ProgressController, parameterSet: KParameterSet, sessionViewModel: SessionViewModel, solverObj:JSolver): Any {
    val simulationRunner = SimulationRunner(
        solverObj,
        sessionViewModel.modelObj.value,
        sessionViewModel.item.mappingInfoSet
    )
    pg.changeMessage("Simulating ${parameterSet.name}")

    solverObj.startTime = sessionViewModel.item.simTabProperties.simulationStartTime.value
    solverObj.stopTime = sessionViewModel.item.simTabProperties.simulationStopTime.value
    solverObj.stepSize = sessionViewModel.item.simTabProperties.simulationStepSize.value

    if (pg.isCancelled) {
        return "Cancelled called"
    }


    try {
        return simulationRunner.solve(parameterSet)
    } catch( e:Exception) {
        return e.message!!
    }

}

/**
 * Perform simulation on each set, and save a solver result for each. Only update db when all completed with success.
 */

fun performSimulationOnSets(
    pg: ProgressController,
    actionOnSuccess: () -> Unit,
    sessionViewModel: SessionViewModel,
    solverObj: JSolver,
    vararg parameterSets: KParameterSet
    ) {

    val simResults = mutableMapOf<KParameterSet, SimulationResult>()
    var errorMessage = ""
    runAsync {
        sessionViewModel.item.updateCovariateMappings()
        for (paramSet in parameterSets) {
            if (pg.isCancelled) {
                break
            }
            val result = performSimulation(pg, paramSet, sessionViewModel, solverObj)
            if (result is SolverResult) {

                pg.changeMessage("Computing macros for ${paramSet.name}")

                val macroValues = sessionViewModel.modelObj.value.computeMacroValues(
                    result.timepoints,
                    result.data.data,
                    paramSet.modelParameterValues.map{it.defaultValue}.toDoubleArray(),
                    mapOf<String, List<org.apache.commons.math3.util.Pair<Double, Double>>>()
                )

                // Add the formulas
                val depVars = sessionViewModel.item.modelObj.dependentVariableNames;
                val formulas = sessionViewModel.item.mappingInfoSet.mappings.values.filter {
                    it.usedFor == KMappingInfo.MappingType.ESTIMATION && it.isFormula()}.filter { !depVars.contains(it.formula.trim()) }.map {
                        it.formula
                }.distinct()
                val formulaResults =  sessionViewModel.modelObj.value.computeFormulas_nat(result.timepoints, result.data.data,
                    paramSet.modelParameterValues.map{it.defaultValue}.toDoubleArray(),
                    formulas.toTypedArray())

                simResults[paramSet] = SimulationResult(result, macroValues, formulaResults)
            } else {
                this.status
                // TODO Even cancel comes here, never in the fail or cancel below.
                println("Exception while simulating")
                println(result.toString())
                errorMessage = "An error was detected during simulation."
                break
            }
        }
        println("")
    } ui {
        if (pg.isCancelled) {
            errorMessage = "Simulation(s) cancelled by user"
        } else {
            if (errorMessage == "") {

                updateSolverResultMap(
                    sessionViewModel.item,
                    simResults,
                    solverObj.methodName,
                    solverObj.controlParameters
                )
                actionOnSuccess()
            } else {
                println("Error message: $errorMessage")
            }
        }
    } fail {

        errorMessage = if (pg.isCancelled)  "Simulation(s) cancelled by user" else
            "Simulation(s) failed with message ${it.message}. Discarding all results."
    } cancel {
        errorMessage = "Some unknown error occured."

    } finally {
        pg.close()
        if (errorMessage != "") {
            alert(Alert.AlertType.INFORMATION, errorMessage)
        }
    }

    pg.openModal()
}
