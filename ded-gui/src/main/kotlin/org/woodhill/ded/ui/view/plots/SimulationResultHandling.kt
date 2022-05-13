package org.woodhill.ded.ui.view.plots

import edu.rochester.urmc.cbim.jni.JGeneralParameter
import edu.rochester.urmc.cbim.jni.JXDECSVFile
import edu.rochester.urmc.cbim.jni.SolverResult
import org.woodhill.ded.models.KParameterSet
import org.woodhill.ded.models.KSolverResult
import org.woodhill.ded.session.SessionElements
import org.woodhill.ded.ui.threading.SimulationResult
import tornadofx.*


//fun updateSolverResultMap(session: SessionElements, results: Map<KParameterSet, Pair<SolverResult, JXDECSVFile>>, solverName: String, controlParams: Array<JGeneralParameter>) {
fun updateSolverResultMap(session: SessionElements, results: Map<KParameterSet, SimulationResult>, solverName: String, controlParams: Array<JGeneralParameter>) {

    val paramSets = results.keys
    session.solverResults.removeIf{it.parameterSet in paramSets}

    results.forEach { (k, v) ->
        session.solverResults.add(KSolverResult(v.simulation, v.macros, v.formulas, k, solverName, controlParams))
    }
}



class SimResultDisplay : Fragment() {
    override val root = label("This is what we display after the results have been displayed")
}