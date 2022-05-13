package org.woodhill.ded.ui.view.datatable

import javafx.beans.property.SimpleObjectProperty
import javafx.beans.property.SimpleStringProperty
import javafx.collections.ObservableList
import javafx.scene.control.TableView
import org.woodhill.ded.models.KCovariate
import org.woodhill.ded.models.KMappingInfo
import org.woodhill.ded.models.KMappingInfoSet
import org.woodhill.ded.models.TVFunctionMapping
import org.woodhill.ded.session.SessionViewModel
import org.woodhill.ded.ui.view.CovariateMappingChanged
import org.woodhill.ded.ui.view.DatatatableDataChangeEvent
import org.woodhill.ded.ui.view.EquationsChangeEvent
import org.woodhill.ded.workspace.TabbedModelsView
import tornadofx.*

class CovariateTableEntry(
    val tvFunctionMapping: TVFunctionMapping
) {

    private val covariateProperty = SimpleObjectProperty<KCovariate>(tvFunctionMapping.covariate)
    val tvFunctionMappingProperty = SimpleObjectProperty<TVFunctionMapping>(tvFunctionMapping)
    val nameProperty = covariateProperty.select(KCovariate::nameProperty)
    val usedProperty = tvFunctionMappingProperty.select(TVFunctionMapping::usedProperty)
    val columnMappingProperty = SimpleStringProperty(mappedColumnNames())

    fun updateColumnMapping() = run { columnMappingProperty.value = mappedColumnNames() }

    private fun mappedColumnNames(): String {
        return tvFunctionMappingProperty.value.tableColumns.joinToString(separator = ", ") {
            it.column_name
        }
    }
}

fun createTableEntries(sessionViewModel: SessionViewModel): ObservableList<CovariateTableEntry> {
    val entries = mutableListOf<CovariateTableEntry>()
    sessionViewModel.item.covariates.forEach {
        entries.add(CovariateTableEntry(sessionViewModel.item.tvFunctionMappings[it]!!))
    }
    return entries.asObservable()
}


class CovariatePane : Fragment("Covariates") {
    override val scope = super.scope as TabbedModelsView.SessionScope
    private val model = scope.model

    private val _tableEntries = createTableEntries(model)
    private val tableEntries: ObservableList<CovariateTableEntry> get() = _tableEntries

    override val root = titledpane("Covariate Mapping") {
        vbox {
            scrollpane(fitToWidth = true) {
                tableview<CovariateTableEntry> {

                    onLeftClick {
                        println("Left clicked into the table at ${this.selectionModel.selectedIndex}")
                    }

                    items = tableEntries
                    column("Macro", CovariateTableEntry::nameProperty)
                    column("Used", CovariateTableEntry::usedProperty).makeEditable()
                    column("Data Column(s)", CovariateTableEntry::columnMappingProperty)
                    column("Time Varying Function", CovariateTableEntry::tvFunctionMappingProperty)

                    columnResizePolicy = SmartResize.POLICY
                    subscribe<EquationsChangeEvent> {
                        val currentCovariates = model.item.covariates.map { it.name }
                        val currentCovariatesInTable = _tableEntries.map { it.nameProperty.value }
                        _tableEntries.removeIf { !currentCovariates.contains(it.nameProperty.value) }
                        model.item.covariates.filter { !currentCovariatesInTable.contains(it.name) }.forEach {
                            _tableEntries.add(CovariateTableEntry(model.item.tvFunctionMappings[it]!!))
                        }
                    }

                    subscribe<DatatatableDataChangeEvent> {
                        //TODO Update here as well.
                    }
                }.onLeftClick {
                    val cells = (this.content as TableView<*>).selectionModel.selectedCells
                    if (!cells.isEmpty()) {

                        val cell = cells[0]
                        val contents = (this.content as TableView<CovariateTableEntry>).items[0]

                        if (cell.column == 2) {
                            val selectedColumns = CovariateColumnSelection.getMappedColumns(model)
                            val newCovariateColumnNames = selectedColumns.map { it.column_name }
                            var covariateMappingChanged = false
                            model.item.mappingInfoSet.mappings.values.forEach {
                                if (it.column_name in newCovariateColumnNames) {
                                    if (it.usedFor != KMappingInfo.MappingType.COVARIATE) {
                                        it.usedFor = KMappingInfo.MappingType.COVARIATE
                                        covariateMappingChanged = true
                                    }
                                } else {
                                    if (it.usedFor == KMappingInfo.MappingType.COVARIATE) {
                                        it.usedFor = KMappingInfo.MappingType.IGNORE
                                        covariateMappingChanged = true
                                    }
                                }
                            }
                            if (covariateMappingChanged) {
                                val tvFunctionMapping = contents.tvFunctionMapping
                                tvFunctionMapping.tableColumns.clear()
                                newCovariateColumnNames.forEach {
                                    KMappingInfoSet.getMappingInfoSet(model.item.model).mappings[it]?.let { it1 ->
                                        tvFunctionMapping.tableColumns.add(
                                            it1
                                        )
                                    }
                                }
                                contents.updateColumnMapping()
                                fire(CovariateMappingChanged(scope))
                            }
                        }

                        if (cell.column == 3) {
                            val functionParams = CovariateFunctionSelection.getTVFunction(
                                model,
                                contents.tvFunctionMappingProperty.get()
                            )
                            val tvf = contents.tvFunctionMappingProperty.value
                            tvf.tvFunctionName = functionParams.first
                            tvf.clearControlParams()
                            tvf.addControlParams(functionParams.second)
                        }
                        (this.content as TableView<*>).refresh()
                    }
                }
            }
        }
    }
}
