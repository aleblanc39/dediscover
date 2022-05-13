package org.woodhill.ded.models

import javafx.beans.property.SimpleBooleanProperty
import javafx.beans.property.SimpleIntegerProperty
import javafx.beans.property.SimpleStringProperty
import org.woodhill.ded.util.DBLoadMethods
import org.woodhill.ded.util.DBSaveMethods
import tornadofx.getValue
import tornadofx.setValue


class KModel(modelID:Int, title:String, sessionName:String, packageName:String, equations:String, isEditable:Boolean = true) {

    private val modelIDProperty = SimpleIntegerProperty(modelID)
    var modelID by modelIDProperty

    val titleProperty = SimpleStringProperty(title)
    var title: String by titleProperty

    private val sessionNameProperty = SimpleStringProperty(sessionName)
    var sessionName: String by sessionNameProperty

    private val packageNameProperty = SimpleStringProperty(packageName)
    var packageName: String by packageNameProperty

    val equationsProperty = SimpleStringProperty(equations)
    var equations: String by equationsProperty

    val editableProperty = SimpleBooleanProperty(isEditable)
    var editable by editableProperty

    constructor(title:String ="New Model", sessionName:String ="NewModel__" + kotlin.math.abs(newModelID), packageName: String = "UserModels", equations: String=""):this(nextModelID(), title, sessionName, packageName, equations) {

    }

    fun save() {
       DBSaveMethods.instance.saveModelObject(this)
    }

    companion object {

        // Keep a map of new models.
        private val newModels = mutableMapOf<Int, KModel>()

        var newModelID:Int = -1
        fun nextModelID():Int = newModelID--


        // Will always be > 0
        fun getKModel(modelID:Int): KModel = if (modelID < 0) newModels[modelID]!! else DBLoadMethods.instance.loadModelFromDB(modelID)

        fun createNewModel(packageName:String, sessionName:String, title:String = ""):KModel {
            val newModel = KModel(packageName=packageName, sessionName = sessionName, title = title )
            newModels[newModel.modelID] = newModel
            return newModel
        }
    }

}