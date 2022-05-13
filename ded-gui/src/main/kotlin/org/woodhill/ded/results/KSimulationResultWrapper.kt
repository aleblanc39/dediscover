package org.woodhill.ded.results

import org.woodhill.ded.models.KSolverResult
import org.woodhill.ded.ui.chartoptions.ChartableControlsTB
import kotlin.math.abs

class SimulationResultWrapper(val solverResult: KSolverResult) {

    val data = KSaveableResultData(solverResult)
    val setName = solverResult.parameterSet.name

    val allNames: List<String>
        get() = data.symbolNames

    /**
     * Needs to be saved
     */


    /**
     * @param chartVar
     * @return
     */
    fun getData(chartVar: String): KSaveableResultData {
        return data.filter { name -> chartVar == name }

    }

    /**
     *
     * @param chartOptions
     * @param log
     * @return
     */
    fun getData(
        chartOptions: ChartableControlsTB,
        log: Boolean
    ): KSaveableResultData {
        return data.filter { name ->
            (chartOptions.contains(name)
                    && chartOptions.isVisible(name)
                    && chartOptions.isLog(name) == log)
        }
    }

    /**
     * @param chartVar
     * @param log
     * @return
     */
    fun getData(
        chartVar: String, log: Boolean,
        chartOptions: ChartableControlsTB
    ): KSaveableResultData {
        return data.filter { name ->
            (chartVar == name && chartOptions.contains(name)
                    && chartOptions.isVisible(name)
                    && chartOptions.isLog(name) == log)
        }
    }

    /**
     * @return
     */
    val timepoints: List<Double> = data.timepoints


    private fun findClosest(allValues: List<Double>, v: Double, startIndex: Int): Int {
        var finalIndex = startIndex
        var lastDiff = Double.MAX_VALUE
        while (finalIndex < allValues.size) {
            if (abs(allValues[finalIndex] - v) < lastDiff) {
                lastDiff = abs(allValues[finalIndex] - v)
                finalIndex++
            } else {
                return finalIndex - 1
            }
        }
        return finalIndex - 1
    }

    /**
     * Returns a list of all the indices corresponding to the timepoints in
     * timepointsUsed. Some timepoints may appear multiple times. Take the closest.
     *
     * Assume the list to be sorted.
     */
    fun getIndicesForTimepoints(timepointsUsed: List<Double>): IntArray {

        val indicesUsed = mutableSetOf<Int>()

        var lastIndex: Int = 0;
        for (thisTIme in timepointsUsed) {
            lastIndex = findClosest(
                timepoints, thisTIme,
                lastIndex
            )
            indicesUsed.add(lastIndex)
        }
        val x = indicesUsed.toIntArray()
        x.sort()
        return x
    }
}
