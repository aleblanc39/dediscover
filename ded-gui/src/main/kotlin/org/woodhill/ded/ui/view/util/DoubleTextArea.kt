package org.woodhill.ded.ui.view.util

import edu.rochester.urmc.cbim.dediscover.simulation.ScientificFormatter
import javafx.beans.property.SimpleDoubleProperty
import javafx.beans.property.SimpleIntegerProperty
import javafx.beans.property.StringProperty
import javafx.beans.value.ObservableValue
import javafx.event.EventTarget
import javafx.scene.Node
import javafx.scene.control.TextField
import javafx.util.StringConverter
import tornadofx.*


fun EventTarget.doubletextfield(
    value: String? = null,
    op: DoubleTextField.() -> Unit = {},
    action: () -> Boolean = { true }
) = DoubleTextField(action).attachTo(this, op) {
    if (value != null) it.text = value
}


//private static NumberFormat formatter = new ScientificFormatter(true,
//GUIUtility.getUserDoubleFraction())
val formatter =  ScientificFormatter(true, 3)

class DoubleStringConverter() : StringConverter<Number>() {
    override fun toString(p0: Number): String {
        return formatter.format(p0)
    }

    override fun fromString(p0: String?): Double {
        println("Converting to double")
        if (p0 != null) {
            return p0.toDouble()
        }
        return 0.0
    }
}



fun EventTarget.doubletextfield(
    property: ObservableValue<Number>,
    action: () -> Boolean = { true },
    op: DoubleTextField.() -> Unit = {}
) = doubletextfield(action = action).apply {
    bind(property, converter = DoubleStringConverter())
    op(this)
}

fun EventTarget.integertextfield(value: String? = null, op: IntegerTextField.() -> Unit = {}) =
    IntegerTextField().attachTo(this, op) {
        if (value != null) it.text = value
    }

fun EventTarget.integertextfield(property: ObservableValue<Number>, op: IntegerTextField.() -> Unit = {}) =
    integertextfield().apply {
        bind(property)
        op(this)
    }


class DoubleTextField(val action: () -> Boolean = { true }) : TextField() {
    val prop: StringProperty = textProperty()
    var lastValue: Double = 0.0

    init {
        focusedProperty().onChange {
            if (!isFocused) {
                if (processValueChanged(this as TextField, prop, lastValue)) {
                    if (!action()) {
                        prop.value = lastValue.toString() // TODO Format here
                    }
                }
                lastValue = prop.value.toDouble()
            }
        }

        prop.onChange {
            validateDouble(this)
        }
    }

}

class IntegerTextField : TextField() {
    val prop: StringProperty = textProperty()
    var lastValue: Int = 0

    init {
        prop.onChange {
            validateInt(this)
        }
    }
}


fun createDoubletextfield(
    node: Node,
    property: SimpleDoubleProperty,
    action: () -> Boolean = { true }
): DoubleTextField {
    return node.doubletextfield(property, action = action)
}

fun createIntegerTextField(
    node: Node,
    property: SimpleIntegerProperty,
    action: () -> Boolean = { true }
): IntegerTextField {
    return node.integertextfield(property) {
        focusedProperty().onChange {
            println("forcused changes for property: $property")
            if (processValueChanged(this, property, lastValue)) {
                if (!action()) {
                    property.value = lastValue
                }
            }
            lastValue = property.value
        }
    }
}

fun processValueChanged(tf: TextField, prop: StringProperty, lastValue: Double): Boolean {
    println("Processing changed value.")
    if (!tf.text.filter { e -> e != ',' }.isDouble()) {
        tf.text = lastValue.toString()
        setBackgroundcolor(tf)
        return false
    }
    return prop.value.toDouble() != lastValue
}

fun processValueChanged(tf: TextField, prop: SimpleIntegerProperty, lastValue: Int): Boolean {
    if (!tf.text.filter { e -> e != ',' }.isInt()) {
        tf.text = lastValue.toString()
        setBackgroundcolor(tf)
        return false
    }
    return prop.value != lastValue
}
