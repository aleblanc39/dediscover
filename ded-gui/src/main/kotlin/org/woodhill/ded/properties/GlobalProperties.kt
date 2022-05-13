package org.woodhill.ded.properties

import javafx.beans.property.SimpleBooleanProperty
import org.woodhill.ded.models.KModel
import tornadofx.*

class GlobalProperties(model:KModel):KTabProperties(model) {

    val validModelProperty = SimpleBooleanProperty(false)
    var validModel by validModelProperty

    val estimableModelProperty = SimpleBooleanProperty(false)
    var estimableModel by estimableModelProperty

    private val VALID_MODEL_KEY = "validModel"
    private val ESTIMABLE_MODEL_KEY = "estimableModel"

    override fun tabName() = "globalprops"

    override fun initializeProperties() {
        validModel = get(VALID_MODEL_KEY)?.toBoolean() ?: false
        estimableModel = get(ESTIMABLE_MODEL_KEY)?.toBoolean() ?: false
    }

    override fun updateProperties() {
        put(VALID_MODEL_KEY, validModel)
        put(ESTIMABLE_MODEL_KEY, estimableModel)
    }

}