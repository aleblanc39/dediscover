package org.woodhill.ded.ui.popups

import org.woodhill.ded.models.EstimableParameterValue
import tornadofx.*

fun EstimableParameterValue.expandParameterRangeDialog(newValue: Double): Boolean {

    if (newValue in minValue..maxValue) {
        return true
    }

    val bound = if (maxValue < newValue) "upper bound" else "lower bound"

    val newBoundValue: Double = if (newValue > maxValue) {
        if (newValue <= 0) 1.0 else 2 * newValue
    } else {
        if (newValue >= 0) 0.0 else 2 * newValue
    }
    val message = "New value for ${modelSymbol.name} is outside the range\n[${minValue}, ${maxValue}]. " +
            "\nWould you like to change the $bound to $newBoundValue?"
    confirm( header = "Parameter Bound Change Confirmation", content = message, actionFn = {
        if (newValue < minValue) {
            minValue = newBoundValue
        } else {
            maxValue = newBoundValue
        }
        return true
    })

    return false
}

fun EstimableParameterValue.newLowerBound(newBound: Double): Boolean {

    if (newBound <= defaultValue) {
        return true
    }

    if (newBound > maxValue) {
        error(header = "Bound error", content = "The new lower bound is higher than the current upper bound.")
        return false
    }

    val newValue = newBound + (maxValue - newBound)/2
    val message = "New lower bound value for ${modelSymbol.name} is higher than \nthe current parameter value." +
            "\nWould you like to change the current value to $newValue"
    confirm( header = "New Value Change Confirmation", content = message, actionFn = {
        defaultValue = newValue
        minValue = newBound
        return true
    })
    return false
}

fun EstimableParameterValue.newUpperBound(newBound: Double): Boolean {
    if (newBound >= defaultValue) {
        return true
    }

    if (newBound < minValue) {
        error(header = "Bound error", content = "The new upper bound is lower than \nthe current lower bound.")
        return false
    }
    val newValue = minValue + (newBound - minValue)/2
    val message = "New upper bound value for ${modelSymbol.name} is lower than \nthe current parameter value. " +
            "\nWould you like to change the current value  to $newValue"
    confirm( header = "New Value Change Confirmation", content = message, actionFn = {
        defaultValue = newValue
        maxValue = newBound
        return true
    })
    return false
}
