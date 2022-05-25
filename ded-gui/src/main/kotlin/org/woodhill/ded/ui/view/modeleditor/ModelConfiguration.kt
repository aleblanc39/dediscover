package org.woodhill.ded.ui.view.modeleditor

import javafx.collections.ObservableList
import javafx.scene.Node
import javafx.scene.layout.VBox
import org.woodhill.ded.application.Styles
import org.woodhill.ded.models.EstimableParameterValue
import org.woodhill.ded.models.KCovariate
import org.woodhill.ded.models.KMacro
import org.woodhill.ded.session.SaveSessionRequest
import org.woodhill.ded.ui.fxext.columnCheckboxContextMenu
import org.woodhill.ded.ui.view.EquationsChangeEvent
import org.woodhill.ded.ui.view.SymbolDescChangeEvent
import org.woodhill.ded.ui.view.util.createDoubletextfield
import org.woodhill.ded.ui.view.workspace.DEDTabView
import org.woodhill.ded.ui.popups.expandParameterRangeDialog
import org.woodhill.ded.ui.popups.newLowerBound
import org.woodhill.ded.ui.popups.newUpperBound
import tornadofx.*

class ModelConfiguration : DEDTabView() {

    private val numberColumnWidth = 100.0

    private fun modelConfigurationElements(): List<Node> {
        val ret = mutableListOf<Node>()
        ret.add(getEstimationSymbolTable("Model Parameters", currentParamSetObj().modelParameterValues.asObservable()))
        ret.add(
            getEstimationSymbolTable(
                "Initial Conditions",
                currentParamSetObj().initialConditionsValues.asObservable()
            )
        )

        ret.add(
            vbox {
                label("Macros")
                tableview<KMacro> {
                    items = model.macros.value.asObservable()
                    items.forEach{
                        it.descProperty.onChange {
                            fire(SymbolDescChangeEvent(scope))
                        }
                    }
                    column("Name", KMacro::nameProperty)
                    column("Formula", KMacro::formulaProperty)
                    column("Description", KMacro::description).makeEditable()
                    subscribe<EquationsChangeEvent> {
                        refresh()
                    }
                    subscribe<SaveSessionRequest> {
                        editModel.commit()
                    }
                    enableCellEditing()
                    enableDirtyTracking()
                }
            }
        )
        ret.add(
            vbox {
                label("Covariates")
                tableview<KCovariate> {
                    items = model.covariates.value.asObservable()
                    items.forEach{
                        it.descProperty.onChange {
                            fire(SymbolDescChangeEvent(scope))
                        }
                    }
                    column("Name", KCovariate::name)
                    column("Time Expression", KCovariate::timeExpressionProperty)
                    column("Description", KCovariate::description).makeEditable()
                    subscribe<EquationsChangeEvent> {
                        refresh()
                    }
                    subscribe<SaveSessionRequest> { event ->
                        editModel.commit()
                    }
                    enableCellEditing()
                    enableDirtyTracking()
                }
            })
        return ret
    }

    private fun getEstimationSymbolTable(displayLabel: String, params: ObservableList<EstimableParameterValue>): VBox {
        return vbox {
            label(displayLabel)
            tableview<EstimableParameterValue> {
                title = displayLabel
                items = params
                items.forEach{
                    it.descProperty.onChange {
                        fire(SymbolDescChangeEvent(scope))
                    }
                }

                isEditable = true
                column("Name", EstimableParameterValue::nameProperty)
                column("Slider", EstimableParameterValue::sliderProperty) {
                    contextMenu = columnCheckboxContextMenu(this)
                }.makeEditable()

                column("Default", Double::class) {
                    prefWidth = numberColumnWidth
                    isEditable = true
                    value {
                        it.value.defaultValue
                    }
                }.cellFormat {

                    graphic = createDoubletextfield(this, rowItem.defaultValueProperty) {
                        enableDirtyTracking()
                        rowItem.expandParameterRangeDialog(
                            rowItem.defaultValue
                        )
                    }
                }

                column("Min", Double::class){
                    prefWidth = numberColumnWidth
                    value {
                        it.value.minValue
                    }

                }.cellFormat {
                    graphic = createDoubletextfield(this, rowItem.minValueProperty) {
                        rowItem.newLowerBound(
                            rowItem.minValue
                        )
                    }
                }


                column("Max", Double::class){
                    prefWidth = numberColumnWidth

                    value {
                        it.value.maxValue
                    }
                }.cellFormat {
                    graphic = createDoubletextfield(this, rowItem.maxValueProperty) {
                        println("Will validate max")

                        rowItem.newUpperBound(
                            rowItem.maxValue
                        )
                    }
                }

                column("Description", EstimableParameterValue::descProperty).makeEditable()
                enableCellEditing()
                enableDirtyTracking()
                subscribe<SaveSessionRequest> {
                    editModel.commit()
                }
                subscribe<EquationsChangeEvent> {
                    refresh()
                }
            }
        }
    }


    override val root = vbox {
        useMaxWidth = true
        addClass(Styles.modelComponents)
    }

    init {
        modelConfigurationElements().forEach {
            root.add(it)
        }

        subscribe<EquationsChangeEvent> {
            println("Equations changed in init from ModelConfiguration.")
            root.clear()
            modelConfigurationElements().forEach {
                root.add(it)
            }
        }
    }
}