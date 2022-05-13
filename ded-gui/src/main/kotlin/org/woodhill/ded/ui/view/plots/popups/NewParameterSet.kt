package org.woodhill.ded.ui.view.plots.popups

import javafx.beans.property.SimpleBooleanProperty
import javafx.beans.property.SimpleStringProperty
import javafx.scene.control.Alert
import org.woodhill.ded.session.SessionViewModel
import org.woodhill.ded.util.DEDUtils.Companion.generateNewName
import tornadofx.*

class NewParameterSetPopup(val sessionViewModel: SessionViewModel, newNameProperty: SimpleStringProperty, switchToNewSetProperty:SimpleBooleanProperty): Fragment("New Parameter Set") {

    override val root =  vbox {
        form {
            fieldset() {
                field("name") {
                    textfield(newNameProperty).editableProperty()
                    // TODO On change alway validate against current list, change color in case of duplicate.
                    // OR hidden field about name already existing.
                }
                checkbox("Switch to new Set", property = switchToNewSetProperty)
            }
            hbox {
                button("Create") {
                    action {
                        if (sessionViewModel.item.parameterSets.map { it.name }.contains(newNameProperty.value)) {
                            alert(type=Alert.AlertType.INFORMATION, header="Invalid set name", content="Duplicate parameter set. Change the name.")
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

    init {
    }


    companion object {


        fun getNewParameterSet(sessionViewModel: SessionViewModel, prefix:String = "ParamSet-"): Pair<String, Boolean> {
            val returnedValueProperty = SimpleStringProperty(generateNewName(prefix, sessionViewModel.item.parameterSets.map{it.name}.toSet()))
            val switchToNewSetProperty = SimpleBooleanProperty(true)

            NewParameterSetPopup(sessionViewModel, returnedValueProperty, switchToNewSetProperty).openModal(block=true)
            return Pair(returnedValueProperty.value, switchToNewSetProperty.value)
        }
    }
}