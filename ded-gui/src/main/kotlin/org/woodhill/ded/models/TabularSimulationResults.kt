package org.woodhill.ded.models

import org.woodhill.ded.results.SimulationResultWrapper
import org.woodhill.ded.ui.chartoptions.ChartableControlsTB
import kotlin.math.abs
import kotlin.math.log2

class TabularSimulationResults(simulationResultWrappers: List<SimulationResultWrapper>, chartableControlsTB: ChartableControlsTB) {

    private fun timepointsUsed(simulationResultWrappers: List<SimulationResultWrapper>):Pair<List<Double>, Double>  {
        val tps = mutableSetOf<Double>()
        simulationResultWrappers.forEach {srw->
            tps.addAll(srw.timepoints)
        }
        val tmp =  tps.sorted().toList()
        val meanDist = tmp.subList(0, tmp.size-1).zip(tmp.subList(1,tmp.size)).map {
                (a, b) -> b-a
        }.toList().average()
        println("Mean dist: $meanDist")

        val retList = mutableListOf(tmp[0])
        for (i in 1 until tmp.size)  {
            if ( (tmp[i] - retList[retList.lastIndex]) > (meanDist/tmp.size)) {
                retList.add(tmp[i])
            }
        }

        return Pair(retList, meanDist/tmp.size)
    }
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

    fun assignIndicesToTimepoints(allTimepoints:List<Double>, resultTimepoints: List<Double>): List<Int> {
        if (allTimepoints.size < resultTimepoints.size)
            throw Exception("Something wrong with the timepoints")
        val ret = MutableList(allTimepoints.size){-1}

        var startIndex = 0
        for (i in resultTimepoints.indices) {
            val loc = findClosest(allTimepoints, resultTimepoints[i], startIndex)
            ret[loc] = i
            startIndex = loc

        }
        return ret
    }

    private fun removeDuplicateTimes(times:List<Double>, interval:Double):List<Double> {
        val retList = mutableListOf(times[0])
        for (i in 1 until times.size)  {
            if ( (times[i] - retList[retList.lastIndex]) > interval) {
                retList.add(times[i])
            }
        }
        return retList
    }

    val timepoints_mean = timepointsUsed(simulationResultWrappers)

    private val _varHeaders = mutableListOf<String>()
    val varHeaders: List<String>
        get() = _varHeaders


    private val _variablesByCol = mutableListOf<List<Double>>()
    val variablesByCol: List<List<Double>>
        get() = _variablesByCol

    private val _variablesOnRows = mutableListOf<List<Double>>()
    val variablesOrRows:List<List<Double>>
        get() = _variablesOnRows

    init {

        val indicesForWrapper = mutableMapOf<SimulationResultWrapper, IntArray>()
        simulationResultWrappers.forEach {wrap ->
            val indicesUsed = wrap
                .getIndicesForTimepoints(timepoints_mean.first)
            wrap.allNames.forEach {name ->
                if (chartableControlsTB.isVisible(name)) {
                    _varHeaders.add(
                        name
                                + if (simulationResultWrappers.size > 1) "<" + wrap.setName
                                + ">" else ""
                    )
                    val resultData = wrap.getData(name).getDataColumn(
                        name, indicesUsed
                    )
                    val locations = assignIndicesToTimepoints(timepoints_mean.first,
                        removeDuplicateTimes(wrap.timepoints, timepoints_mean.second))
                    val dataToAdd = MutableList(locations.size){ Double.NaN}


                    val logIf = if (chartableControlsTB.isLog(name)) {x:Double -> log2(x)} else {x:Double->x}
                    for ( (i,value) in locations.withIndex()) {
                        if (value >= 0) {
                            // TODO HERE: log data if required
                            dataToAdd[i] = logIf(resultData[value])
                        }
                    }
                    _variablesByCol.add(dataToAdd.toList())
                }
            }
            indicesForWrapper[wrap] = indicesUsed
        }

        // Transpose (each row is a variable)
        for (i in timepoints_mean.first.indices) {
            _variablesOnRows.add(variablesByCol.map { it[i] })
        }
    }
}