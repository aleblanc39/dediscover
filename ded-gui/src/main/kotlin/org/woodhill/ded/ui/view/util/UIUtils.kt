package org.woodhill.ded.ui.view.util

import javafx.scene.control.Alert
import javafx.scene.control.TextInputControl
import javafx.scene.paint.Color
import javafx.scene.text.Text
import tornadofx.*

fun validateDouble(control: TextInputControl, validColor:Color = Color.WHITE, invalidColor:Color = Color.RED) {
    setBackgroundcolor(control, if (control.text.filter { e -> e != ',' }.isDouble()) validColor else invalidColor)
}
fun validateInt(control: TextInputControl, validColor:Color = Color.WHITE, invalidColor:Color = Color.RED) {
    setBackgroundcolor(control, if (control.text.filter { e -> e != ',' }.isInt()) validColor else invalidColor)
}

fun setBackgroundcolor(control:TextInputControl, color: Color = Color.WHITE) {

    val red = color.red * 255
    val green = color.green * 255
    val blue = color.blue * 255

    control.style = "-fx-control-inner-background: rgb($red,$green, $blue);"
}
fun setBackgroundcolor(control: Text, color: Color = Color.WHITE) {

    val red = color.red * 255
    val green = color.green * 255
    val blue = color.blue * 255

    control.style = "-fx-control-inner-background: rgb($red,$green, $blue);"
}

fun notImplemented() = alert(type = Alert.AlertType.INFORMATION, header = "Functionality not implemented yet")
