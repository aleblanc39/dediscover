package org.woodhill.ded.properties

import edu.rochester.urmc.cbim.jni.Algorithm
import edu.rochester.urmc.cbim.jni.JSolver
import org.woodhill.ded.models.KModel
import tornadofx.onChange

class KSolverTabProperties(model: KModel) : KTabProperties(model) {

    override fun tabName(): String = "solvertab"

    // TODO Try to eliminate this. We're creating an object that will never be used.
    var algmProperties: AlgorithmProperty = AlgorithmProperty(JSolver.s_getAvailableMethods()[0], JSolver::class, "")


    // This is called after after loading from the database. If algmPEntry is null then this is a new session,
    override fun initializeProperties() {
        val algmPEntry = get(ALGM_PROPERTIES)
        algmProperties = if (algmPEntry == null) AlgorithmProperty(JSolver.s_getAvailableMethods()[0], JSolver::class, "")
        else AlgorithmProperty.createAlgmProperties(algmPEntry, JSolver::class)
    }


    override fun updateProperties() {
        put(ALGM_PROPERTIES, algmProperties.toJsonString())
    }

    private fun updateSolver() =
        algmProperties.controlParamsProperties.updateControlParams(Algorithm.getControlParameters(JSolver::class.java, algmProperties.name))

    init {
        algmProperties.nameProperty.onChange {
            updateSolver()
        }
    }
    companion object {
        private const val ALGM_PROPERTIES = "ALGM_PROPERTIES"
    }

}