package org.woodhill.ded.workspace.popups

import javafx.beans.property.SimpleStringProperty
import javafx.scene.input.KeyCode
import javafx.scene.input.KeyEvent
import tornadofx.*

class NewPackageUserQuery(currentPackage: String, validateNewPackage: (String) -> String) : Fragment("New Package") {

    val packageProperty = SimpleStringProperty()
    val errorMessageProperty = SimpleStringProperty("")

    override val root = vbox {
        form {
            fieldset {
                field("Root Package") {
                    text(currentPackage)
                }
                field("Package") {
                    textfield().bind(packageProperty)
                }
            }
        }
        hbox {
            button("Accept") {
                action {
                    val errorMsg = validateNewPackage(currentPackage.plus(".").plus(packageProperty.value))
                    if (errorMsg != "") {
                        errorMessageProperty.value = errorMsg
                    } else {
                        close()
                    }
                }
                isDefaultButton = true
            }

            button("Cancel").action {
                packageProperty.value = ""
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

        /**
         * Return the new package name. Empty string if there is no new package.
         */
        fun getNewPackageName(currentPackage: String, validateNewPackage: (String) -> String): String {
            val selectionWindow = NewPackageUserQuery(currentPackage, validateNewPackage)
            selectionWindow.openModal(block = true)

            return if (selectionWindow.packageProperty.value.isNotEmpty()) {
                currentPackage.plus(".").plus(selectionWindow.packageProperty.value)
            } else {
                ""
            }
        }
    }
}