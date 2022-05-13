package org.woodhill.ded.workspace.popups

import javafx.beans.property.SimpleStringProperty
import javafx.scene.paint.Color
import org.woodhill.ded.ui.view.util.setBackgroundcolor
import tornadofx.*

class RenameColumnQuery(val oldName: String, val currentNames: Set<String>): Fragment("Rename Column") {

    private val newColumnNameProperty = SimpleStringProperty("")
    var newColumnName by newColumnNameProperty
    private val errorMessageProperty = SimpleStringProperty("")
    private var errorMsg: String by errorMessageProperty

    private val displayErrorMessage = "The column name you selected already exists"
    override val root = vbox {
        form  {
            fieldset {
                field("Old column name: ") {
                    text(oldName)
                }
                field("New Colulmn Name") {
                    textfield("").bind(newColumnNameProperty)
                }

            }
        }
        hbox {
            button("Accept") {
                action {
                    if (currentNames.contains(newColumnName)) {
                        errorMsg = displayErrorMessage
                    } else {
                        close()
                    }

                }
                isDefaultButton = true
                enableWhen {newColumnNameProperty.isNotEmpty  }
            }
            button("Cancel").action {
                newColumnName = ""
                close()
            }
        }
        text(errorMessageProperty) {
            setBackgroundcolor(this, Color.RED)
            isVisible = false
            errorMessageProperty.onChange {
                this.isVisible = errorMessageProperty.value != ""
            }
        }
    }

    companion object {
        fun queryNewColumnName(oldColumn:String, currentNames: Set<String>):String {
            val selectionWindow = RenameColumnQuery(oldColumn, currentNames)
            selectionWindow.openModal(block = true)
            return selectionWindow.newColumnName.trim()
        }
    }
}