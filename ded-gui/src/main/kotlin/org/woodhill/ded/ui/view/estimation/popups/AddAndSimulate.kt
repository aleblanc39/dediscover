package org.woodhill.ded.ui.view.estimation.popups

import javafx.beans.property.SimpleStringProperty
import javafx.scene.control.Alert
import org.woodhill.ded.session.SessionViewModel
import org.woodhill.ded.util.DEDUtils.Companion.generateNewName
import tornadofx.*

class AddAndSimulate(val sessionViewModel: SessionViewModel, newNameProperty: SimpleStringProperty): Fragment("New Parameter Set") {
    override val root =  vbox {
        form {
            fieldset() {
                field("name") {
                    textfield(newNameProperty).editableProperty()
                    // TODO On change alway validate against current list, change color in case of duplicate.
                    // OR hidden field about name already existing.
                }
            }
            hbox {
                button("Add and Simulate") {
                    action {
                        if (sessionViewModel.item.parameterSets.map { it.name }.contains(newNameProperty.value)) {
                            alert(type= Alert.AlertType.INFORMATION, header="Invalid set name", content="Duplicate parameter set. Change the name.")
                        } else {
                            close()
                        }
                    }
                }
                button("Cancel").apply {
                    action {
                        newNameProperty.value=""
                        close()
                    }
                }
            }
        }
    }

    companion object {
        fun addAndSimulate(sessionViewModel: SessionViewModel, prefix:String = "EstimationParamSet-"): String {
            val returnedValueProperty = SimpleStringProperty(generateNewName(prefix, sessionViewModel.item.parameterSets.map{it.name}.toSet()))

            AddAndSimulate(sessionViewModel, returnedValueProperty).openModal(block=true)
            return returnedValueProperty.value
        }
    }
}