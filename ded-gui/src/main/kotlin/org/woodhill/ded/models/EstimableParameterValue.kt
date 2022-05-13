package org.woodhill.ded.models

import javafx.beans.property.SimpleBooleanProperty
import javafx.beans.property.SimpleDoubleProperty
import javafx.beans.property.SimpleObjectProperty
import tornadofx.getValue
import tornadofx.select
import tornadofx.setValue

class EstimableParameterValue(
    var id: Int,
    val parameterSet: KParameterSet,
    val modelSymbol: EstimableSymbol,
    initDefault: Double = 0.5,
    minValue: Double = 0.0,
    maxValue: Double = 1.0,
    slider: Boolean = true,
    estimated: Boolean = false
) {

    private val estimableSymbolProperty = SimpleObjectProperty<ModelSymbol>(modelSymbol)

    val nameProperty = estimableSymbolProperty.select(ModelSymbol::nameProperty)
    val name: String by nameProperty

    val descProperty = estimableSymbolProperty.select(ModelSymbol::descProperty)


    val defaultValueProperty = SimpleDoubleProperty(initDefault)
    var defaultValue by defaultValueProperty

    val minValueProperty = SimpleDoubleProperty(minValue)
    var minValue by minValueProperty

    val maxValueProperty = SimpleDoubleProperty(maxValue)
    var maxValue by maxValueProperty

    val sliderProperty = SimpleBooleanProperty(slider)
    val slider by sliderProperty

    val estimatedProperty = SimpleBooleanProperty(estimated)
    val estimated by estimatedProperty

    constructor(
        parameterSet: KParameterSet,
        modelSymbol: EstimableSymbol,
        default: Double = 0.5,
        minValue: Double = 0.0,
        maxValue: Double = 1.0,
        slider: Boolean = true,
        estimated: Boolean = false
    ) : this(
        nextID(),
        parameterSet,
        modelSymbol,
        default,
        minValue,
        maxValue,
        slider,
        estimated
    ) {

    }

    fun duplicate(forParamSet: KParameterSet): EstimableParameterValue {
        return EstimableParameterValue(
            forParamSet,
            modelSymbol,
            defaultValue,
            minValue,
            maxValue,
            slider,
            estimated
        )
    }

    /**
     * Same functionality as duplicate(KParameterSet), but for another model symbol
     */
    fun duplicate(forParamSet: KParameterSet, ms:EstimableSymbol): EstimableParameterValue {
            return EstimableParameterValue(
                forParamSet,
                ms,
                defaultValue,
                minValue,
                maxValue,
                slider,
                estimated
            )
        }


    companion object ModelSymbolValues {
        var id = -1
        fun nextID(): Int = (id--)
    }

}