package org.woodhill.ded.properties

import edu.rochester.urmc.cbim.jni.JSolver
import org.junit.Test

import org.junit.Assert.*
import org.junit.Before

class AlgorithmPropertyTest {
    @Before
    fun setUp() {
        System.loadLibrary("DEDiscover")
    }


    @Test
    fun testConstructor() {
        val x = AlgorithmProperty(JSolver.s_getAvailableMethods()[0], JSolver::class, "")
        print(x)

    }

    @Test
    fun getNameProperty() {
    }


    @Test
    fun testJsonString() {
        val methodName = JSolver.s_getAvailableMethods()[0]
        val x = AlgorithmProperty(methodName, JSolver::class, "")
        println(x.toJsonString())
    }

    @Test
    fun getAlgmInstanca() {
        val methodName = JSolver.s_getAvailableMethods()[0]
        val x = AlgorithmProperty(methodName, JSolver::class, "")
        assertEquals(methodName, x.algmInstance.getjMethodName())

    }

    @Test
    fun getControlParamsProperties() {
        val methodName = JSolver.s_getAvailableMethods()[0]
        val x = AlgorithmProperty(methodName, JSolver::class, "")
        for (p in x.algmInstance.controlParameters) {
            println(p.name)
        }
    }
}