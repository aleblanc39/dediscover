package org.woodhill.ded.models

import com.nhaarman.mockitokotlin2.doReturn
import com.nhaarman.mockitokotlin2.mock
import edu.rochester.urmc.cbim.jni.*
import io.mockk.every
import io.mockk.mockk
import org.junit.Before
import org.junit.Test
import org.mockito.Mockito.*
import org.junit.Assert.*
import org.mockito.Mockito
import kotlin.random.Random
import kotlin.reflect.KClass

class KSolverResultTest {



    @Before
    fun setUp() {
        System.loadLibrary("DEDiscover")
    }


    /**
     * Will test the constructor
     */

    private fun getResults( nbTimepoints:Int, nbVars: Int): Array<DoubleArray> {
        val rand = Random(0)
        val ret = arrayOfNulls<DoubleArray>(nbTimepoints)
        (0 until nbTimepoints).forEach{
            ret[it] = DoubleArray(nbVars){ rand.nextDouble()}

        }

        return ret.filterNotNull().toTypedArray()
    }



    @Test
    fun constructorTest() {
//        mock<SolverResult> {
//
//            //on {getNbTimepoints()}  doReturn  5
//            on {timepoints} doReturn doubleArrayOf(0.0,1.0,2.0,3.0,4.0)
//            on {variableNames} doReturn arrayOf("a", "b", "c")
//            on {nbVariables} doReturn 3
//            var rand = Random(0)
//
//            on {results} doReturn getResults(5, 3)
//        }
//
//        val jsolverMock = mock<JSolver> {
//            on{methodName} doReturn "RK 45"
//            on{controlParameters} doReturn arrayOf<JGeneralParameter>()
//        }
       // val kSolverResult = KSolverResult(solverResultMock, KParameterSet.Companion.getParameterSet(1), jsolverMock)
    }


    @Test
    fun dataAsJson() {
        println(System.getenv("LD_LIBRARY_PATH"))

        val solverResultMock = mockk<SolverResult> {
            every {jgetNbTimepoints()}  returns  5
            every {jgetTimepoints()} returns doubleArrayOf(0.0,1.0,2.0,3.0,4.0)
            every {jgetVariableNames()} returns arrayOf("a", "b", "c")
            every {jgetNbVariables()} returns 3
            every {jgetResults()} returns getResults(5, 3)
        }


        println("Moking the xdecsv file")

        val macroResultMock = mockk<JXDECSVFile> {
            every{getjNbTimepoints()}  returns 5
            every {getjTimepoints()} returns doubleArrayOf(0.0,1.0,2.0,3.0,4.0)
            every {getjVariableNames()} returns arrayOf("mac1", "mac2")
            every {getjNbVariables()} returns 2
            every {getjData()} returns getResults(5, 2)
        }
        println("Mocking the param set")
        val paramSetMock = mock<KParameterSet> {

        }
    }

    @Test
    fun controlParamsAsJson() {
    }

    @Test
    fun store() {
    }

    @Test
    fun getId() {
    }


    @Test
    fun getVarNames() {
    }

    @Test
    fun getDirty() {
    }

    @Test
    fun getData() {
    }
}