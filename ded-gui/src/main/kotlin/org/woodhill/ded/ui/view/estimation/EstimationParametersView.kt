package org.woodhill.ded.ui.view.estimation

import javafx.beans.property.SimpleListProperty
import javafx.beans.property.SimpleObjectProperty
import javafx.collections.ObservableList
import javafx.scene.control.TableView
import javafx.scene.layout.HBox
import javafx.scene.layout.VBox
import org.woodhill.ded.models.EstimableParameterValue
import org.woodhill.ded.models.KParameterSet
import org.woodhill.ded.ui.fxext.columnCheckboxContextMenu
import org.woodhill.ded.ui.threading.performEstimation
import org.woodhill.ded.ui.view.plots.ParamConfigView
import org.woodhill.ded.ui.view.util.createDoubletextfield
import org.woodhill.ded.ui.view.workspace.DEDTabView
import org.woodhill.ded.ui.popups.expandParameterRangeDialog
import org.woodhill.ded.ui.popups.newLowerBound
import org.woodhill.ded.ui.popups.newUpperBound
import tornadofx.*


class EstimationParametersView : DEDTabView("Est: Parameters") {
    val numberColumnWidth = 100.0
    val defaultParamSet = model.item.parameterSetByName(model.item.defaulParameterSetName)


    // Should we use default?
    val modelParamCopyFromProperty = SimpleObjectProperty<KParameterSet>(defaultParamSet)
    val initConditionCopyFromProperty = SimpleObjectProperty<KParameterSet>(defaultParamSet)
    private val nonEstParamSetsProperty =   SimpleListProperty(observableListOf<KParameterSet>())
    private val nonEstParameterSets by nonEstParamSetsProperty

    override var root = titledpane("Estimation Parameters", collapsible = false) {
        vbox {
            titledpane("Model Paramters") {
                add(
                    tableAndButtons(
                        model.estimationParameterSet.value.modelParameterValues,
                        modelParamCopyFromProperty
                    )
                )
            }
            titledpane("Initial Conditions") {
                add(
                    tableAndButtons(
                        model.estimationParameterSet.value.initialConditionsValues,
                        initConditionCopyFromProperty
                    )
                )
            }
            hbox {
                button("Estimate") {
                    action {
                        performEstimation(
                            find(),
                            model.item
                        ) { x -> afterEstimation(x) }
                    }
                    enableWhen(model.item.globalProperties.estimableModelProperty)
                }
            }
        }
    }


    fun tableAndButtons(
        params: ObservableList<EstimableParameterValue>,
        copyValProp: SimpleObjectProperty<KParameterSet>
    ): VBox {
        return vbox {
            add(createTableView(params))
            add(createButtons(params, copyValProp))
        }
    }

    fun createTableView(symbols: ObservableList<EstimableParameterValue>): TableView<EstimableParameterValue> {
        val thisTableView = tableview<EstimableParameterValue> {
            items = symbols
            column("Name", EstimableParameterValue::nameProperty)
            column("Estimated", EstimableParameterValue::estimatedProperty){
                contextMenu = columnCheckboxContextMenu(this)
            }.makeEditable()


            // This code is similar to some code in ModelConfiguration, and may be duplicated if we
            // implement the two-step estimation. Should factor out.
            column("Default", Double::class) {
                prefWidth = numberColumnWidth

                value {
                    it.value.defaultValue
                }
            }.cellFormat {
                graphic = createDoubletextfield(this, rowItem.defaultValueProperty) {
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

            column("Max",  Double::class){
                prefWidth = numberColumnWidth
                value {
                    it.value.maxValue
                }
            }.cellFormat {
                graphic = createDoubletextfield(this, rowItem.maxValueProperty) {
                    rowItem.newUpperBound(
                        rowItem.maxValue
                    )
                }
            }

            column("Description", EstimableParameterValue::descProperty)
        }
        return thisTableView
    }




    fun createButtons(
        params: ObservableList<EstimableParameterValue>,
        copyValProp: SimpleObjectProperty<KParameterSet>
    ): HBox {
        return hbox {
            label("From parameter set")
            combobox<KParameterSet>(copyValProp) {
                //items = KParameterSet.parameterSetsForModel(model.item.model). filter{it.name != model.item.estimationParameterSetName}.asObservable()
                items = nonEstParamSetsProperty.value
                converter = ParamConfigView.KParameterSetStringConverter(model.item.model)
            }
            button("Full range").action {
                params.forEach { param ->
                    val fromParam = copyValProp.value.getParamWithName(param.name)
                    param.defaultValue = fromParam.defaultValue
                    param.maxValue = fromParam.maxValue
                    param.minValue = fromParam.minValue
                }
            }
            button("Values Only").action {
                params.forEach { param ->
                    val fromParam = copyValProp.value.getParamWithName(param.nameProperty.value)
                    param.defaultValue = fromParam.defaultValue
                    if (param.maxValue < param.defaultValue) {
                        param.maxValue= fromParam.defaultValue
                    }
                    if (param.minValue > param.defaultValue) {
                        param.minValue= fromParam.defaultValue
                    }
                }
            }
        }

    }

    override fun onDock() {
        super.onDock()
        disableOnInvalidModel(root)

        nonEstParameterSets.clear()
        sessionElements.parameterSets.filter { it.name != model.item.estimationParameterSetName }.forEach {
            nonEstParameterSets.add(it)
        }

        sessionElements.parameterSets.onChange {
            nonEstParameterSets.clear()
            sessionElements.parameterSets.filter { it.name != model.item.estimationParameterSetName }.forEach {
                nonEstParameterSets.add(it)
            }
        }
    }

}