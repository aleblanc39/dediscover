package org.woodhill.ded.runners

import edu.rochester.urmc.cbim.jni.JDDEModel
import edu.rochester.urmc.cbim.jni.JSolver
import edu.rochester.urmc.cbim.jni.SolverResult
import org.woodhill.ded.models.KMappingInfoSet
import org.woodhill.ded.models.KParameterSet
import java.util.*

class SimulationRunner(
    val solver: JSolver,
    override val model: JDDEModel,
    val mappingInfoSet: KMappingInfoSet
) : AbstractRunner(model) {
    private fun createEstimableSymbolsMap(parameterSet: KParameterSet): Map<String, Double> {
        val ret = mutableMapOf<String, Double>()
        parameterSet.initialConditionsValues.forEach {
            ret[it.modelSymbol.name] = it.defaultValue
        }
        parameterSet.modelParameterValues.forEach {
            ret[it.modelSymbol.name] = it.defaultValue
        }
        return ret
    }


    fun solve(parameterSet: KParameterSet): SolverResult {
        val timepointsForSimulation =
            if (mappingInfoSet != null && mappingInfoSet.timepoints().isNotEmpty()) getTimepointsForSimulation()
            else listOf()
        return solver.solve(model, createEstimableSymbolsMap(parameterSet), timepointsForSimulation)
    }

    /**
     * Compute the timepoints that will be used for the simulation. The points specified in the
     * solver object are always used. Then the points used in the data table are used as well
     * for the residuals. Min and max are determined by solver, and point from the datatable outside
     * this range will be ignored.
     * @return
     */
    private fun getTimepointsForSimulation(): List<Double>? {
        val timesUsed: MutableSet<Double> = TreeSet()

        // TODO Shouldn't be in the solver object. Should read directly from ui
        val endTime: Double = solver.stopTime
        val startTime: Double = solver.startTime
        var d = startTime

        while (d <= endTime) {
            timesUsed.add(d)
            d += solver.stepSize
        }
        //Handle round off error, for the case where the endTime is 100, but after the
        // repeated additions we get 100.0000004.
        // May end up with both 99.99999 and 100, but only one will be displayed in table.
        if (!timesUsed.contains(endTime)) timesUsed.add(endTime)
        for (d in mappingInfoSet.timepoints()) {
            if (d > startTime && d < endTime && !timesUsed.contains(d)) {
                timesUsed.add(d)
            }
        }

        return ArrayList(timesUsed)
    }


}