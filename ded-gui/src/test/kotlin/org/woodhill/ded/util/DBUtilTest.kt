package org.woodhill.ded.util

import edu.rochester.urmc.cbim.jni.JDDEModel
import edu.rochester.urmc.cbim.jni.JInitialCondition
import org.woodhill.ded.models.KModel
import org.woodhill.ded.models.KParameterSet
import org.junit.Before
import org.junit.Test
import org.woodhill.ded.models.EstimableSymbol
import org.woodhill.ded.models.ModelSymbol
import java.io.StringReader
import javax.json.Json

class DBUtilTest {



    @Test
    fun getResultDataTest() {


        val jsonStr = "{\"name\":\"RosenBrock Solver\",\"control_params\":\"{\\\"R Tolerance\\\":\\\"0.001\\\",\\\"A Tolerance\\\":\\\"1.0E-6\\\",\\\"Max Steps\\\":\\\"1000000\\\",\\\"Init Step Size\\\":\\\"0.001\\\",\\\"Min Step Size\\\":\\\"1.0E-15\\\",\\\"Max Step Size\\\":\\\"1.0\\\",\\\"Scale Step Size\\\":\\\"true\\\",\\\"Safe Factor\\\":\\\"0.9\\\"}\"}"
        val reader = Json.createReader(StringReader(jsonStr))
        println("READ")
        println(reader.read())

    }


    @Test
    fun loadModelParameterValuesTest() {
        val modelID:Int = 6
        val paramSet = KParameterSet(29, "hello", KModel.getKModel(modelID) )
        //val modelParams = loadModelParametersValues()
        println("# model params:"  + paramSet.modelParameterValues.size)
        println("# dep vars:"  + paramSet.initialConditionsValues.size)

    }

    @Test
    fun synchronizeModelSymbolsTest() {
        val model1 = "dx/dt=1\ndy/dt=2"
        val model2 = "dk/dt=alpha\ndx/dt=3\ndz/dt=3"

        val modelObj = JDDEModel(model1)
        modelObj.jInitialConditions.forEach{
            println(it.name)
            println(it.firstOccurenceLine)
        }
        val dummyModel = KModel()
        val initialConditions = mutableListOf(
            EstimableSymbol(-1, dummyModel,modelObj.jInitialConditions[0].name, modelObj.jInitialConditions[0].jOccurenceOrder),
            EstimableSymbol(-1, dummyModel,modelObj.jInitialConditions[1].name, modelObj.jInitialConditions[0].jOccurenceOrder)
        )

        modelObj.updateJEquations(model2)

        DEDUtils.synchronizeNewModelSymbols(
            initialConditions,
            modelObj.jInitialConditions,
            initialConditions,
            {x: ModelSymbol ->x.name},
            {x: JInitialCondition -> x.name},
            { x: ModelSymbol, y: JInitialCondition -> x.occurenceIndex = y.firstOccurenceLine},
            {x: JInitialCondition -> EstimableSymbol(-1, dummyModel, x.name, x.firstOccurenceLine, "") })

        println("After updating model")
        modelObj.jInitialConditions.forEach{
            println("${it.name} -- ${it.firstOccurenceLine}")
        }
        println("After update and sync")
        initialConditions.forEach{
            println("${it.name} -- ${it.occurenceIndex}")
        }
    }


    @Test
    fun loadSolverResultTest() {
//        val solverResultID = 29
//        val solverResult = DBLoadMethods.fetchSolverResult(solverResultID)
//        println("param set ID: " + solverResult.parameterSet.id)
//        println("param set name: " + solverResult.parameterSet.name)
//        println("timepoints: " + solverResult.timepoints)
//        println("data: " + solverResult.data)
//        println("varnames: " + solverResult.varNames)
    }


}