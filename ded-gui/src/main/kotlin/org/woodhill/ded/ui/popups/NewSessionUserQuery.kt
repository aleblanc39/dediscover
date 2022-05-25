package org.woodhill.ded.ui.popups

import javafx.beans.property.SimpleBooleanProperty
import javafx.beans.property.SimpleStringProperty
import javafx.scene.input.KeyCode
import javafx.scene.input.KeyEvent
import org.woodhill.ded.ui.view.workspace.NewSessionInfo
import tornadofx.*

class NewSessionUserQuery(initialValue: NewSessionBaseValues, validateNewName: (String, String, String) -> String): Fragment("New Session Values") {

    val packageProperty = SimpleStringProperty(initialValue.intermediatePackage)
    val sessionNameProperty = SimpleStringProperty(initialValue.sessionName)
    val sessionTitleProperty = SimpleStringProperty(initialValue.sessionTitle)
    val errorMessageProperty = SimpleStringProperty("")
    var acceptClickedProperty = SimpleBooleanProperty(false)


    override val root = vbox {
        form  {
            fieldset {
                field("Root Package") {
                    text(initialValue.topPackage)
                }
                field("Package (Can be empty)") {
                    textfield("").bind(packageProperty)
                }
                field("Session Name") {
                    textfield().bind(sessionNameProperty)
                }
                field("Session Title") {
                    textfield().bind(sessionTitleProperty)
                }
            }
        }
        hbox {
            button("Accept") {
                action {
                    val errorMsg = validateNewName(initialValue.topPackage, packageProperty.value, sessionNameProperty.value)
                    if (errorMsg == "") {
                        acceptClickedProperty.value = true
                        close()
                    } else {
                        errorMessageProperty.value = errorMsg
                    }
                }
                isDefaultButton = true
                enableWhen {sessionNameProperty.isNotEmpty  }
            }
            button("Cancel").action {
                sessionNameProperty.value = ""
                close()
            }
        }
        text(errorMessageProperty) {
            isVisible = false
            errorMessageProperty.onChange {
               this.isVisible = errorMessageProperty.value != ""
            }
        }


        addEventFilter(KeyEvent.KEY_PRESSED) { event ->
            if (event.code == KeyCode.ENTER)
                close()
        }

    }


    companion object {
        fun getSessionName(initialSessionValues: NewSessionBaseValues, validateNewName:(String, String, String) -> String): NewSessionInfo {
            val selectionWindow = NewSessionUserQuery(initialSessionValues, validateNewName)
            selectionWindow.openModal(block = true)

            val np = if (selectionWindow.packageProperty.value== null) "" else selectionWindow.packageProperty.value.trim()
            val newPackage = initialSessionValues.topPackage.plus(if (np == "") "" else ".".plus(np))

            if (selectionWindow.acceptClickedProperty.value)
                return NewSessionInfo(newPackage, selectionWindow.sessionNameProperty.value, selectionWindow.sessionTitleProperty.value)
            else
                return NewSessionInfo(newPackage, "", selectionWindow.sessionTitleProperty.value)
        }
    }
}

data class NewSessionBaseValues(val topPackage:String = "", val intermediatePackage:String = "", val sessionName:String = "", val sessionTitle:String = "")