package org.woodhill.ded.models

import edu.rochester.urmc.cbim.jni.JGeneralParameter
import edu.rochester.urmc.cbim.jni.JXDECSVFile
import edu.rochester.urmc.cbim.jni.SolverResult
import org.woodhill.ded.util.*
import java.io.File
import java.util.stream.IntStream
import javax.json.Json
import javax.json.JsonObjectBuilder

/**
 * Simulation result. data has form:
 *
 * Each row (outer array) corresponds to a var
 * Each column(inner array) corresponds to timepoint
 */


// TODO Do we need mutable lists for arguments. Or and var? Object should be immutable.

data class KSolverResult(
    var id: Int,
    val solver: String,
    private val _varNames: MutableList<String>,
    var dirty: Boolean,
    val parameterSet: KParameterSet,
    val timepoints: List<Double>,
    val controlParams: Map<String, String>,
    private val _data: MutableList<DoubleArray>
) {

    val varNames get() = _varNames
    val data get() = _data

    constructor(solverResult: SolverResult, macroResults: JXDECSVFile, formulasResult: JXDECSVFile, parameterSet: KParameterSet, solverName: String, contolParams: Array<JGeneralParameter>) : this(
        computeNextID(),
        solverName,
        solverResult.jgetVariableNames().toMutableList(),
        true,
        parameterSet,
        solverResult.jgetTimepoints().asList(),
        getControlParametersMap(contolParams),
        doubleArrayFromJNIObject(solverResult)
    ) {

        // Add macro results then formulas
        // TODO Wrap into function taking macroResults, formulaResults as parameter
        if (macroResults.getjNbTimepoints() > 0) {
            if (macroResults.getjNbTimepoints() != solverResult.jgetNbTimepoints()) {
                throw java.lang.Exception("Mismatch in number of timepoints between solver result and macro values")
            }

            macroResults.getjVariableNames().zip(IntRange(0, macroResults.getjNbVariables()-1)).forEach {macro->
                _varNames.add(macro.first)
                val dataForVar = mutableListOf<Double>()
                macroResults.getjData().forEach {row->
                    dataForVar.add(row[macro.second])
                }
                _data.add(dataForVar.toDoubleArray())
            }
        }

        if (formulasResult.getjNbTimepoints() > 0) {
            if (formulasResult.getjNbTimepoints() != solverResult.jgetNbTimepoints()) {
                throw java.lang.Exception("Mismatch in number of timepoints between solver result and macro values")
            }
            formulasResult.getjVariableNames().zip(IntRange(0, formulasResult.getjNbVariables()-1)).forEach {frm->
                _varNames.add(frm.first)
                val dataForVar = mutableListOf<Double>()
                formulasResult.getjData().forEach {row->
                    dataForVar.add(row[frm.second])
                }
                _data.add(dataForVar.toDoubleArray())
            }
        }
    }


    // TODO Figure out why this doesn't work
//            it.asSequence().zip(data.asSequence()) { a, b ->
//                {
//                    println("Adding with i = $i")
//                    b[i] = a
//                }
//            }

    /**
     * Write to file. For now assume necessary validation has been performed.
     *
     * TODO Add control params to json, solver type,
     */
    fun exportAsJson(filepath: String) {
        File(filepath).bufferedWriter().use {out ->
            out.write(resultsAsJson())

        }
    }

    fun exportAsCSV(filepath: String) {
        File(filepath).bufferedWriter().use {out ->
            out.write("Time, " + _varNames.joinToString(separator = ","))
            out.newLine()
            IntRange(0, timepoints.size-1).forEach { i->
                val line = mutableListOf(timepoints[i])
                line.addAll(_data.map{it[i]})
                out.write(line.joinToString(separator= ","))
                out.newLine()
            }
        }
    }


    private fun resultValuesBuilder(): JsonObjectBuilder {
        val resultValues = Json.createObjectBuilder()
        _varNames.zip(_data) { a, b ->
            Pair(a, b)
        }.forEach { (a, b) ->
            resultValues.add(a,b.asList().joinToString(" "))
        }
        return resultValues
    }


    fun dataAsJson(): String {
        return resultValuesBuilder().build().toString()
    }

    private fun resultsAsJson(): String {
        val jsonObj = Json.createObjectBuilder()
        jsonObj.add("solver", solver)
        jsonObj.add("paramValues", DEDUtils.paramSetAsJsonBuilder(parameterSet))
        jsonObj.add("controlParams", DEDUtils.controlParamsAsJsonBuilder(controlParams))
        jsonObj.add("timepoints", timepoints.joinToString(separator = " "))
        jsonObj.add("results", resultValuesBuilder())

        return jsonObj.build().toString()
    }


    fun controlParamsAsJson(): String {
        return stringPairsToJson(controlParams.toList())
    }


    companion object {
        var nextID = -1

        fun computeNextID() = nextID--

        fun doubleArrayFromJNIObject(solverResult: SolverResult):MutableList<DoubleArray> {
            val ret = mutableListOf<DoubleArray>()

            IntStream.range(0, solverResult.jgetNbVariables()).forEach { varIndex ->
                run {
                    val values = mutableListOf<Double>()
                    solverResult.jgetResults().forEach {
                        values.add(it[varIndex])
                    }
                    ret.add(DoubleArray(values.size) { indx ->
                        values.get(indx)
                    })
                }
            }
            return  ret
        }

        fun getSolverResults(model:KModel): MutableList<KSolverResult> = DBLoadMethods.instance.loadSolverResults(model)
    }

    fun duplicate(toParameterSet: KParameterSet):KSolverResult {
        return KSolverResult(
            id = computeNextID(),
            solver = solver,
            _data = _data,
            _varNames = _varNames,
            controlParams = controlParams,
            dirty = dirty,
            parameterSet = toParameterSet,
            timepoints = timepoints
        )
    }
}

