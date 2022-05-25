package org.woodhill.ded.ui.view.util

import edu.rochester.urmc.cbim.jni.Algorithm
import javafx.beans.property.Property
import javafx.event.EventTarget
import org.woodhill.ded.models.KMappingInfo
import org.woodhill.ded.properties.AlgorithmProperty
import org.woodhill.ded.session.SessionViewModel
import org.woodhill.ded.models.ControlParameterProperties
import tornadofx.*


fun getAlgorithmForm(ev: EventTarget, algmProperties: AlgorithmProperty, filter:(String)->Boolean = {true}): Form {
    algmProperties.updateAvailableMethods(filter)
    val retForm = ev.form {
        fieldset {
            field("Name") {
                combobox(
                    algmProperties.nameProperty,
                    algmProperties.availableMethods //getAvailableMethods(filter).asObservable()
                ) {
                }
            }

            field("Type") {
                textfield(algmProperties.algmShortDescProperty).editableProperty().set(false)
            }

            field("Description") {
                textfield(algmProperties.algmLongDescProperty).editableProperty().set(false)
            }

        }
    }

    val fieldset = retForm.getChildList()?.get(0) as Fieldset

    algmProperties.nameProperty.onChange {
        algmProperties.updateAlgm()
        fieldset.children.remove(3, fieldset.children.size)
        algmProperties.controlParamsProperties.addToFieldset(fieldset, ev)
    }

    algmProperties.controlParamsProperties.addToFieldset(fieldset, ev)
    fieldset.add(
        fieldset.button("Defaults") {
            action {
                algmProperties.resetControlParameters()
            }
        }
    )
    return retForm

}

// TODO Seems weird here. Only called on algm creation.

fun updateControlParamValues(values: ControlParameterProperties, algm: Algorithm?) = values.paramValues().forEach {
    if (algm != null) {
        algm.getControlParameter(it.first).value = (it.second as Property<*>).value
    }
}


// TODO Really belong here?
fun validateEstimation(model: SessionViewModel) {
    val mis = model.item.mappingInfoSet
    val globalProperties = model.item.globalProperties

    if (mis.timepoints().size < 5) {
        println("Not enough timepoints")
        globalProperties.estimableModel = false
        return
    }

    // Need at least one column
    val estimatedCols = mis.mappings.values.filter {
        it.usedFor == KMappingInfo.MappingType.ESTIMATION
    }

    if (estimatedCols.isEmpty()) {
        println("No estimated column")
        globalProperties.estimableModel = false
        return
    }

    // All the mappings must be valid
    val x = estimatedCols.map { it.status }

    if (x.isEmpty()) {
        println("Need at least one estimated column")
        globalProperties.estimableModel = false
        return
    }

    if (x.filter { it.status == KMappingInfo.MappingStatus.FAILED.status }.isNotEmpty()) {
        println("Some invalid mapping")
        globalProperties.estimableModel = false
        return
    }
    println("Should now be estimable.")
    globalProperties.estimableModel = true
}
