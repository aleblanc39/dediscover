package org.woodhill.ded.session

import tornadofx.EventBus
import tornadofx.FXEvent
import tornadofx.ItemViewModel
import tornadofx.Scope

class SaveSessionRequest(scope: Scope): FXEvent(EventBus.RunOn.ApplicationThread, scope)

class SessionViewModel : ItemViewModel<SessionElements>() {

    val estimationParameterSet = bind(SessionElements::estimationParameterSet)

    val modelObj = bind(SessionElements::modelObj)

    val macros = bind(SessionElements::macros)
    val covariates = bind(SessionElements::covariates)

    val chartableControls = bind(SessionElements::chartableControls)
    val parameterSets = bind(SessionElements::parameterSets)

    var editScope: Scope? = null

    fun saveSession() {
        fire(SaveSessionRequest(editScope!!))
        this.item.save()
    }
}
