package org.woodhill.ded.properties

import org.junit.Before
import org.junit.Test

import org.junit.Assert.*
import org.woodhill.ded.models.KModel

class KSimTabPropertiesTest {


    val testParamSetName = "TestParamSet"
    val notAParamSetName = "NotAParamSet"
    @Before
    fun setUp() {
    }

    @Test
    fun setDirty() {
    }

    @Test
    fun containsKey() {
    }

    @Test
    fun keySet() {
    }

    @Test
    fun setPlotVisibility() {
    }

    @Test
    fun plotVisible() {
        val simTabProperties = KSimTabProperties(KModel())
        assertTrue(simTabProperties.plotVisible(notAParamSetName, true))
        assertFalse(simTabProperties.plotVisible(notAParamSetName, false))

        simTabProperties.setPlotVisibility(testParamSetName, true)
        assertTrue(simTabProperties.plotVisible(testParamSetName, false))

        simTabProperties.setPlotVisibility(testParamSetName, false)
        assertFalse(simTabProperties.plotVisible(testParamSetName, false))
    }

    @Test
    fun setAutoRefresh() {
    }

    @Test
    fun autoRefresh() {
    }

    @Test
    fun setSelectedParamSet() {
    }

    @Test
    fun selectedParamSet() {
    }
}