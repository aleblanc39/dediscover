package org.woodhill.ded.properties

import org.woodhill.ded.models.KModel
import org.woodhill.ded.util.DBLoadMethods
import org.woodhill.ded.util.DBSaveMethods
import kotlin.reflect.KClass

abstract class KTabProperties(val model: KModel): KSessionProperties() {

    abstract fun tabName(): String
    abstract fun initializeProperties()
    abstract fun updateProperties()

    fun saveProperties() {
        updateProperties()
        DBSaveMethods.instance.saveTabProperties(model, this)
    }

    fun saveCurrentMap() {
        DBSaveMethods.instance.saveTabProperties(model, this)

    }


    companion object {
         fun<T: KTabProperties> loadTabProperties(model:KModel, clazz: KClass<T> ): T = DBLoadMethods.instance.loadTabProperties(model, clazz)
    }

}