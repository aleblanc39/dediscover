package org.woodhill.ded.models
import javafx.beans.property.SimpleStringProperty
import org.woodhill.ded.util.DBLoadMethods.Companion.instance
import tornadofx.getValue
import tornadofx.setValue

class KCovariate (model:KModel, var id: Int, name: String,  timeExpression: String, description: String="", occurenceIndex:Int = -1):ModelSymbol(model, name, occurenceIndex, description) {

    constructor(model:KModel, name:String, timeExpression:String, description: String, occurenceIndex: Int = -1):this(model, nextID(), name, timeExpression, description, occurenceIndex)

    val timeExpressionProperty = SimpleStringProperty(timeExpression)
    var timeExpression by timeExpressionProperty

    companion object {

        private var newObjectID:Int = -1
        private fun nextID():Int = newObjectID--

        @JvmStatic
        fun getCovariates(model:KModel):List<KCovariate> = instance.loadCovariates(model)
    }
}
