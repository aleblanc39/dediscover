package org.woodhill.ded.properties

import edu.rochester.urmc.cbim.jni.JConfidenceInterval
import edu.rochester.urmc.cbim.jni.JObjectiveFunction
import edu.rochester.urmc.cbim.jni.JOptimizer
import edu.rochester.urmc.cbim.jni.JSolver
import org.junit.Test

import org.junit.Assert.*
import org.junit.Before

class EstimationTabPropertiesTest {

    @Before
    fun setUp() {
        System.loadLibrary("DEDiscover")
    }


    @Test
    fun tabName() {
        println(ESTIMATION_PROPERTY_TAGS.ALGM_OBJECTIVE_FCN.name)
        println (defaultFunction(JObjectiveFunction::class.java))
        println (defaultFunction(JConfidenceInterval::class.java))
        println (defaultFunction(JSolver::class.java))
        println (defaultFunction(JOptimizer::class.java))

    }

    @Test
    fun initializeProperties() {
    }

    @Test
    fun updateProperties() {
    }
}