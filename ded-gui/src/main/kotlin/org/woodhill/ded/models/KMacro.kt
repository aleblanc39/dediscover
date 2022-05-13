package org.woodhill.ded.models

import javafx.beans.property.SimpleStringProperty
import org.woodhill.ded.util.DBLoadMethods.Companion.instance
import tornadofx.getValue
import tornadofx.setValue

class KMacro(model:KModel, var id: Int, name: String,  formula: String, description: String = "", occurenceIndex: Int = -1) :
    ModelSymbol(model, name, occurenceIndex, description) {

    val formulaProperty = SimpleStringProperty(formula)
    var formula:String by formulaProperty

    constructor(model:KModel, name:String, formula:String, occurenceIndex: Int = -1): this(model, nextID(), name, formula, "", occurenceIndex)

    override fun toString():String = "$name--$formula--$occurenceIndex--$description"

    companion object {
        private var newObjectID:Int = -1
        private fun nextID():Int = newObjectID--

        fun getMacros(model:KModel): List<KMacro> = instance.loadMacros(model)
    }
}
