package org.woodhill.ded.ui.view.modeleditor

import javafx.scene.control.Alert
import org.woodhill.ded.application.Styles
import org.woodhill.ded.ui.view.EquationsChangeEvent
import org.woodhill.ded.ui.view.workspace.DEDTabView
import tornadofx.*
import javafx.scene.control.ButtonType

import javafx.scene.control.Alert.AlertType
import javafx.scene.layout.Region
import org.woodhill.ded.ui.fxext.richTextArea


class EquationsEditor : DEDTabView("Equations") {

    override val root = vbox {
        useMaxWidth = true
        form {
            fieldset {

                textfield(model.item.model.titleProperty) {}

                richTextArea(model.item.model.equationsProperty)
                {
                    addClass(Styles.equationEditor)
                }

                hbox {
                    style {

                    }
                    button("parse") {
                        prefWidth = 95.0
                    }.setOnAction {
                        fun parseEqns() {
                            val errors = model.item.parseNewEquations(model.item.model.equations)
                            if (errors.isEmpty()) {
                                println("Firing update")
                                fire(EquationsChangeEvent(scope))

                                // TODO Too specific here?
                                model.item.solverResults.clear()
                            } else {
                                val alert = Alert(
                                    AlertType.ERROR,
                                    errors.joinToString(separator = "\n") { "Line ${it.first}: ${it.second}" },
                                    ButtonType.OK
                                )
                                alert.dialogPane.minHeight = Region.USE_PREF_SIZE
                                alert.show()

                            }
                        }

                        if (model.item.solverResults.isNotEmpty()) {
                            confirm(
                                header = "Parsing Confirmation",
                                content = "Parsing will result in discarding existing results. Continue?",
                                actionFn = {
                                    parseEqns()
                                }
                            )
                        } else {
                            parseEqns()
                        }
                    }
                    button("Help") {
                        prefWidth = 95.0
                    }
                }
                spacing = 10.0
            }
        }
    }

    init {
    }
}