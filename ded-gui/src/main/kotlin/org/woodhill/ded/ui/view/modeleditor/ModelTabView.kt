package org.woodhill.ded.ui.view.modeleditor

import javafx.scene.layout.Priority
import org.woodhill.ded.workspace.DEDTabView
import tornadofx.*

class ModelTabView : DEDTabView("Model") {


    override val root = borderpane {
        style {
            useMaxWidth = true
        }
        left<EquationsEditor>()
        right<ModelConfiguration>()
    }

    // This doesn't work. Display ok but events not captured

//    override val root = hbox {
//        add(EquationsEditor())
//        add(ModelConfiguration())
//        add()
//
//        hgrow = Priority.ALWAYS
//        useMaxWidth = true
//    }
}

