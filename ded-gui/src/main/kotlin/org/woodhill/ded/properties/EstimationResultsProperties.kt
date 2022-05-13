package org.woodhill.ded.properties

import javafx.beans.property.SimpleBooleanProperty
import org.woodhill.ded.models.KModel

class EstimationResultsProperties(model: KModel): KTabProperties(model) {

    override fun tabName(): String = "EstimationResults"

    enum class PROPERTIES() {
        HasValidResults
    }
    val hasValidResultsProperty = SimpleBooleanProperty(false)

    override fun initializeProperties() {
        hasValidResultsProperty.value = get(PROPERTIES.HasValidResults.toString())?.toBoolean()?:false
    }

    override fun updateProperties() {
        put(PROPERTIES.HasValidResults.toString(), hasValidResultsProperty.value)
    }

}