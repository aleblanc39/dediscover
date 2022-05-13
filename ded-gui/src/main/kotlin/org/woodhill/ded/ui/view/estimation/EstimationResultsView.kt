package org.woodhill.ded.ui.view.estimation

import edu.rochester.urmc.cbim.jni.JSolver
import javafx.geometry.Pos
import org.woodhill.ded.results.EstimatedRow
import org.woodhill.ded.results.FixedRow
import org.woodhill.ded.results.EstimationCIResults
import org.woodhill.ded.ui.ProgressController
import org.woodhill.ded.ui.threading.performSimulationOnSets
import org.woodhill.ded.ui.view.estimation.popups.AddAndSimulate
import org.woodhill.ded.ui.view.util.doubleTableColumn
import org.woodhill.ded.workspace.DEDTabView
import tornadofx.*


const val prefHeaderWidth = 200.0
class EstimationResultsView : DEDTabView("Est: Results") {

    //fun pg() = ProgressController.getProgressControler(scope, {},{},{})
    private val estimationResult: EstimationCIResults = sessionElements.estimationResults
    override var root = titledpane("Estimation Results", collapsible = false) {
        vbox {
            titledpane("Fit Information", collapsible = false) {
                vbox {
                    alignment = Pos.BASELINE_CENTER
                    hbox {
                        prefWidth = prefHeaderWidth
                        hbox {
                            label("Optimal value:")
                            textfield(estimationResult.optimalValueProperty) {
                                isEditable = false
                            }

                        }
                        hbox {
                            prefWidth = prefHeaderWidth
                            label("Residual Sum of Squares (RSS):")
                            textfield(estimationResult.rssValueProperty) {
                                isEditable = false
                            }
                        }
                    }
                    hbox {
                        hbox {
                            label("AIC:")
                            textfield(estimationResult.aicValueProperty) {
                                isEditable = false

                            }

                        }
                        hbox {
                            label("Number of Estimates:")
                            textfield(estimationResult.nbEstimatesProperty) {
                                isEditable = false
                            }
                        }
                    }
                    hbox {
                        hbox {
                            label("AICc:")
                            textfield(estimationResult.aiccValueProperty) {
                                isEditable = false

                            }

                        }
                        hbox {
                            label("Number of Observations:")
                            textfield(estimationResult.nbObservationsProperty) {
                                isEditable = false
                            }
                        }
                    }
                    hbox {
                        hbox {
                            label("BIC:")
                            textfield(estimationResult.bicValueProperty) {
                                isEditable = false
                            }
                        }
                    }
                }
            }
            titledpane("Estimated Parameters", collapsible = false) {
                tableview<EstimatedRow> {
                    items = estimationResult.estimatedRows
                    column("Name", EstimatedRow::nameProperty)
                    doubleTableColumn( "EstimatedValue", EstimatedRow::estimatedValueProperty)

//                    column("EstimatedValue", EstimatedRow::estimatedValueProperty) {
//                        cellFactory = DoubleResultTableCell.forTableColumn()
//                    }
                    doubleTableColumn( "Standard Error", EstimatedRow::standardErrorProperty)
                    doubleTableColumn("CI Low Bound", EstimatedRow::ciLowerBoundProperty)
                    doubleTableColumn("CI High Bound", EstimatedRow::ciHighBoundProperty)
                    doubleTableColumn("p-value", EstimatedRow::pvalueProperty)
                    doubleTableColumn( "t-statistic", EstimatedRow::tstatProperty)
                    doubleTableColumn("Min", EstimatedRow::minProperty)
                    doubleTableColumn("Max", EstimatedRow::maxProperty)
                    column("Description", EstimatedRow::descriptionProperty)
                }
            }
            titledpane("Fixed Parameters", collapsible = false) {
                tableview<FixedRow> {
                    items = estimationResult.fixedRows
                    column("Name", FixedRow::nameProperty)
                    column("Fixed Value", FixedRow::fixedValueProperty)
                    column("Min", FixedRow::minProperty)
                    column("Max", FixedRow::maxProperty)
                    column("Description", FixedRow::descriptionProperty)
                }
            }
            hbox {
                button("Add and Simulate") {
                    action {
                        val newSetName = AddAndSimulate.addAndSimulate(model)
                        if (newSetName.trim().isNotEmpty()) {
                            val ps = sessionElements.copyParameterSet(newSetName, model.item.estimationParameterSet)
                            estimationResult.estimatedRows.forEach {
                                ps.getParamWithName(it.nameProperty.value).defaultValueProperty.value =
                                    it.estimatedValueProperty.value
                                ps.getParamWithName(it.nameProperty.value).maxValueProperty.value = it.maxProperty.value
                                ps.getParamWithName(it.nameProperty.value).minValueProperty.value = it.minProperty.value
                            }
                            estimationResult.fixedRows.forEach {
                                ps.getParamWithName(it.nameProperty.value).defaultValueProperty.value =
                                    it.fixedValueProperty.value
                                ps.getParamWithName(it.nameProperty.value).maxValueProperty.value = it.maxProperty.value
                                ps.getParamWithName(it.nameProperty.value).minValueProperty.value = it.minProperty.value
                            }

                            simtabProperties.currentParameterSetProperty.value = ps.name
                            scope.tabPane.selectionModel.select(2)
                            performSimulationOnSets(
                                //find(),
                                pg(),
                                { fireOnSimulationSuccess() },
                                model,
                                model.item.solverTabProperties.algmProperties.algmInstance!! as JSolver,
                                model.item.parameterSetByName(currentParamSet())
                            )

                        }
                    }
                }
            }
        }

    }

    override fun onDock() {
        super.onDock()
        disableOnProperty(
            root,
            model.item.globalProperties.validModelProperty,
            model.item.estimationResultsProperties.hasValidResultsProperty
        )
    }
}

