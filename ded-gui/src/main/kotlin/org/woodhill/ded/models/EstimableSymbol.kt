package org.woodhill.ded.models

import org.woodhill.ded.util.DBLoadMethods
import org.woodhill.ded.util.DBSaveMethods

open class EstimableSymbol(
    var id: Int,
    model: KModel,
    name: String,
    occurenceIndex: Int = -1,
    description: String = ""
) : ModelSymbol(model, name, occurenceIndex, description) {


    companion object {

        fun loadModelParameters(model:KModel):List<EstimableSymbol> = DBLoadMethods.instance.loadModelParameters(model)
        fun loadInitialConditions(model:KModel): List<EstimableSymbol> = DBLoadMethods.instance.loadDependentVariables(model)

        fun updateModelParameters(model:KModel, newParameters: List<EstimableSymbol>) {
            DBSaveMethods.instance.saveModelParameters(model, newParameters)
        }
        fun updateDependentVariables(model:KModel, newParameters: List<EstimableSymbol>) {
            DBSaveMethods.instance.saveDependentVariables(model, newParameters)
        }
    }
}
