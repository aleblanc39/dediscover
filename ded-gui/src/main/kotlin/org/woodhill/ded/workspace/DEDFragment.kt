package org.woodhill.ded.workspace

import tornadofx.Fragment

abstract class DEDFragment(title:String = ""):Fragment(title) {
    override val scope = super.scope as TabbedModelsView.SessionScope
    val model = scope.model
    val sessionElements = scope.model.item

}