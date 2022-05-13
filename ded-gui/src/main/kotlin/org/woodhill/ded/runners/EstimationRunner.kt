package org.woodhill.ded.runners

import edu.rochester.urmc.cbim.jni.EstimatorResult
import edu.rochester.urmc.cbim.jni.JDDEModel
import edu.rochester.urmc.cbim.jni.JEstimator
import org.woodhill.ded.models.KParameterSet

class EstimationRunner(private val estimator: JEstimator) {

    fun estimate(parameterSet: KParameterSet):EstimatorResult {
        estimator.setParameterSet(parameterSet.jniParameterSet())
        return estimator.estimate()
    }
}