package org.woodhill.ded.properties

import edu.rochester.urmc.cbim.jni.*
import org.woodhill.ded.models.KModel
import org.woodhill.ded.ui.view.util.updateControlParamValues

enum class ESTIMATION_PROPERTY_TAGS {
    ALGM_OPTIMIZER,
    ALGM_SOLVER,
    ALGM_CI,
    ALGM_OBJECTIVE_FCN
}

fun <T : Algorithm?> defaultFunction(clazz: Class<T>):String = Algorithm.getAvailableMethods(clazz).get(0)
class EstimationTabProperties(model: KModel): KTabProperties(model) {
    override fun tabName(): String = "EstimationTab"

    private val algmClasses = mapOf(
        ESTIMATION_PROPERTY_TAGS.ALGM_OPTIMIZER to JOptimizer::class,
        ESTIMATION_PROPERTY_TAGS.ALGM_OBJECTIVE_FCN to JObjectiveFunction::class,
        ESTIMATION_PROPERTY_TAGS.ALGM_CI to JConfidenceInterval::class,
        ESTIMATION_PROPERTY_TAGS.ALGM_SOLVER to JSolver::class
    )
    private val algmProperties = mutableMapOf<ESTIMATION_PROPERTY_TAGS, AlgorithmProperty>()

    init {
        ESTIMATION_PROPERTY_TAGS.values().forEach {
            algmProperties[it] = AlgorithmProperty(defaultFunction(algmClasses[it]!!.java), algmClasses[it]!!, "")
        }
    }

    fun solverAlgm(): AlgorithmProperty = algmProperties[ESTIMATION_PROPERTY_TAGS.ALGM_SOLVER]!!
    fun ciAlgm(): AlgorithmProperty = algmProperties[ESTIMATION_PROPERTY_TAGS.ALGM_CI]!!
    fun optimizerAlgm(): AlgorithmProperty = algmProperties[ESTIMATION_PROPERTY_TAGS.ALGM_OPTIMIZER]!!
    fun objectiveFcnAlgm(): AlgorithmProperty = algmProperties[ESTIMATION_PROPERTY_TAGS.ALGM_OBJECTIVE_FCN]!!


    override fun initializeProperties() {
        ESTIMATION_PROPERTY_TAGS.values().forEach { algm->
            val algmEntry = get(algm.name)
            algmProperties[algm] = if (algmEntry == null) AlgorithmProperty(defaultFunction(algmClasses[algm]!!.java), algmClasses[algm]!!, "")
            else AlgorithmProperty.createAlgmProperties(algmEntry, algmClasses[algm]!!)
        }
    }

    override fun updateProperties() {
        ESTIMATION_PROPERTY_TAGS.values().forEach { algm ->
            put(algm.name, algmProperties[algm]!!.toJsonString())
        }
    }

    // TODO: Is that right? Seems to assume will only be call once
    val solver:JSolver by lazy { getSolverObject() }

    private fun getSolverObject():JSolver = createAlgmObject(JSolver::class.java, solverAlgm())
    fun getCIObject():JConfidenceInterval = createAlgmObject(JConfidenceInterval::class.java, ciAlgm())
    fun getObjFcnObject():JObjectiveFunction = createAlgmObject(JObjectiveFunction::class.java, objectiveFcnAlgm())
    fun getOptimizerObject():JOptimizer = createAlgmObject(JOptimizer::class.java, optimizerAlgm())

    fun  <T : Algorithm?> createAlgmObject(clazz: Class<T>, props: AlgorithmProperty): T {
        val algmObject = Algorithm.createMethod(clazz, props.name)
        updateControlParamValues(props.controlParamsProperties, algmObject)
        return algmObject
    }
}