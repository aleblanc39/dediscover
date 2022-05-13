package org.woodhill.ded.models

import javafx.beans.property.SimpleStringProperty
import org.woodhill.ded.ui.view.SymbolDescChangeEvent
import tornadofx.getValue
import tornadofx.onChange
import tornadofx.setValue

abstract class ModelSymbol(val model:KModel, name:String, var occurenceIndex:Int, description:String) {
    val nameProperty = SimpleStringProperty(name)
    var name: String by nameProperty

    val descProperty = SimpleStringProperty(description)
    var description: String by descProperty



}