package org.woodhill.ded.results

import org.woodhill.ded.models.KSolverResult
import java.util.*


class KSaveableResultData : KResultData {

    val timepoints = mutableListOf<Double>()
    private val data = mutableListOf<DoubleArray>()
    val symbolNames = mutableListOf<String>()
    private var depVarSize = 0

    var count = 0
    var myCount = 0

    constructor(result: KSolverResult) {
        timepoints.addAll(result.timepoints)
        symbolNames.addAll(result.varNames)

        data.addAll(result.data)
        depVarSize = symbolNames.size
        myCount = count++
    }

    override fun getItemCount(): Int {
        return timepoints.size
    }

    override fun timepoint(item: Int): Double {
        return timepoints[item]
    }

    /**
     * Creates a SaveableResultData based on the parent data, with possibly
     * a subset of the timepoints, or repeated timepoints, not necessarily in order.
     *
     * AL 2020: New version -- only filter on variables, maybe other criteria
     *
     * @param parentData
     * @param filter
     */
    constructor(
        parentData: KSaveableResultData,
        //filter: ResultDataFilter
        accept: (String) -> Boolean
    ) {
        timepoints.addAll(parentData.timepoints)
        depVarSize = parentData.depVarSize
        val dataSeries: MutableList<DoubleArray> = ArrayList()
        var count = 0
        for (name in parentData.symbolNames) {
            if (accept(name)) {
                symbolNames.add(name)
                dataSeries.add(
                    extractAtTimepoints(
                        null,
                        parentData.data[count]
                    )
                )
            }
            count++
        }
        //data = Array(names.size) { DoubleArray(timepoints.size) }

        count = 0
        for (d in dataSeries) {
            data.add(d)
            count++
        }
    }

    /**
     *
     * @param indices Index where values are to be taken from
     * @param data    Array where values to be returned are taken.
     * @return subarray of data made of the indices contained in the array indices
     */
    private fun extractAtTimepoints(indices: IntArray?, data: DoubleArray): DoubleArray {
        if (indices == null // Too much optimization!!!|| indices.length == data.length
            || indices.size == 0
        ) {
            return data.clone()
        }
        val ret = DoubleArray(indices.size)
        var cntr = 0
        for (i in indices) {
            ret[cntr++] = data[i]
        }
        return ret
    }


    fun getNames(accept: (String) -> Boolean): List<String> {
        val filteredNames: MutableList<String> = ArrayList()
        for (n in symbolNames) {
            if (accept(n)) {
                filteredNames.add(n)
            }
        }
        return Collections.unmodifiableList(filteredNames)
    }

    override fun filter(accept: (String) -> Boolean): KSaveableResultData {
        return KSaveableResultData(this, accept)
    }


    override fun getNames(): List<String> {
        return symbolNames
    }


    override fun get(seriesIndex: Int, timeIndex: Int): Double {
        return data[seriesIndex][timeIndex]
    }

    private fun findIndex(name: String): Int {
        for (i in symbolNames.indices) {
            if (name == symbolNames[i]) {
                return i
            }
        }
        return -1
    }

    /**
     * @param name
     * @return
     */
    fun getDataColumn(name: String, indices: IntArray): DoubleArray {
        val i = findIndex(name)
        return if (i > -1 && i < data.size) {
            val ret = DoubleArray(indices.size)
            var cntr = 0
            for (indx in indices) ret[cntr++] = data[i][indx]
            ret
        } else {
            throw ArrayIndexOutOfBoundsException(
                "Data series named "
                        + name + " could not be found amongst " + symbolNames
            )
        }
    }

    /**
     * @return
     */
    override fun getNumNames(): Int {
        return symbolNames.size
    }

    /**
     * @param i
     * @return
     */
    override fun getName(i: Int): String {
        return symbolNames[i]
    }

}