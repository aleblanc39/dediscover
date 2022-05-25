package org.woodhill.ded.ui.view.datatable

import javafx.collections.ObservableList
import javafx.scene.control.TableCell
import javafx.scene.control.TableColumn
import javafx.scene.control.TableView
import javafx.scene.control.cell.ComboBoxTableCell
import javafx.scene.control.cell.TextFieldTableCell
import javafx.util.Callback
import javafx.util.StringConverter
import org.woodhill.ded.models.EstimableSymbol
import org.woodhill.ded.models.KMacro
import org.woodhill.ded.models.KMappingInfo
import org.woodhill.ded.session.SaveSessionRequest
import org.woodhill.ded.ui.view.*
import org.woodhill.ded.ui.view.plots.ChartingOptionsUpdatingEvent
import org.woodhill.ded.ui.view.util.validateEstimation
import org.woodhill.ded.ui.view.workspace.TabbedModelsView
import tornadofx.*

val FORMULA_STR = "FORMULA"

fun getOptionsList(initialConditions: List<EstimableSymbol>, macros: List<KMacro>): ObservableList<String> {
    val ret = mutableListOf(FORMULA_STR, "------")
    for (x in initialConditions) {
        ret.add("${x.name} (${x.description})")
    }
    for (x in macros) {
        ret.add("${x.name} (${x.description})")
    }
    return ret.asObservable()
}

// vals is the list of options. Can it be changed?
// The var it is assigned to is declared final in ComboBoxTable Cell
class MapToTableCell<S, T>(var0: StringConverter<String>, vals: ObservableList<T>) :
    ComboBoxTableCell<S, T>(null, vals) {

    init {
        isEditable = false
    }

    override fun updateItem(value: T, empty: Boolean) {
        super.updateItem(value, empty)
        if (empty || value == null) {
            text = null
            graphic = null
        } else {
            if (value.toString().contains("--")) {
                text = FORMULA_STR
            }
        }

        if (this.index >= 0 && this.index < this.tableView.items.size) {
            (this.rowItem as KMappingInfo).usedForProperty.onChange {
                if (it != null) {
                    isEditable = it.equals(KMappingInfo.MappingType.ESTIMATION)
                }
            }

            if ((this.rowItem as KMappingInfo).usedForProperty.value == KMappingInfo.MappingType.ESTIMATION) {
                isEditable = true
            }
        }
    }

    // var1 contains the options. Needs to be able to change
    companion object {
        fun <S, T> forTableColumn(
            var1: ObservableList<T>
        ): Callback<TableColumn<S, T>, TableCell<S, T>> {
            return Callback {
                MapToTableCell<S, T>(
                    MappingPane.MyStringConverter(),
                    var1
                )
            }
        }
    }
}


// TODO Problem here. List of options

class FormulaTableCell : TextFieldTableCell<KMappingInfo, String>(MappingPane.MyStringConverter<String>()) {
    init {
        isEditable = false
    }

    override fun updateItem(value: String?, empty: Boolean) {
        super.updateItem(value, empty)
        if (this.index >= 0 && this.index < this.tableView.items.size) {

            this.rowItem.mapToProperty.onChange {
                isEditable = it.equals(FORMULA_STR)
                if (!it.equals(FORMULA_STR) && !it.equals("")) {
                    println("New mapto Property $it")
                    val newFormula = it?.substring(0, it.indexOf(' '))
                    updateItem(newFormula, false)
                    try {
                        this.rowItem.formulaProperty.set(newFormula)
                    } catch (ex: Exception) {
                        println("Exception caught...")
                    }
                }
            }

            this.rowItem.usedForProperty.onChange {
                if (!it?.equals(KMappingInfo.MappingType.ESTIMATION)!!) {
                    isEditable = false
                }
            }

            if (this.rowItem.usedForProperty.value == KMappingInfo.MappingType.ESTIMATION && this.rowItem.mapToProperty.value == FORMULA_STR) {
                isEditable = true
            }
        }
    }


    companion object {
        fun forTableColumn(): Callback<TableColumn<KMappingInfo, String>, TableCell<KMappingInfo, String>> {
            return Callback {
                FormulaTableCell()
            }
        }
    }
}

class MappingPane() : Fragment("Mappings") {
    override val scope = super.scope as TabbedModelsView.SessionScope
    private val model = scope.model
    private val usedForGeneralOptions = listOf(
        KMappingInfo.MappingType.IGNORE,
        KMappingInfo.MappingType.TIME,
        KMappingInfo.MappingType.ESTIMATION,
        KMappingInfo.MappingType.PLOT_ONLY
    )

    override val root = titledpane("Data Mapping") {
        vbox {
            scrollpane(fitToWidth = true) {
                add(createMappingTable())
            }
            hbox {
                button("Clear Mappings") { }
            }

            subscribe<DatatatableDataChangeEvent> {
                clear()
                add(createMappingTable())
                hbox {
                    button("Clear Mappings") { }
                }
            }
        }
    }

    private fun createMappingTable(): TableView<KMappingInfo> {
        val x = tableview<KMappingInfo>() {
            items = model.item.mappingInfoSet.mappings.values.toList().asObservable()
            column("Status", KMappingInfo::status)
            column("Name", KMappingInfo::column_name)

            column("Used for", KMappingInfo::usedFor) {
                cellFactory = ComboBoxTableCell.forTableColumn(usedForGeneralOptions.asObservable())
            }

            column("Map to", KMappingInfo::mapTo) {

                cellFactory =
                    MapToTableCell.forTableColumn(getOptionsList(model.item.initialConditions, model.item.macros))
                subscribe<EquationsChangeEvent> {
                    cellFactory =
                        MapToTableCell.forTableColumn(getOptionsList(model.item.initialConditions, model.item.macros))
                }
                subscribe<SymbolDescChangeEvent> {
                    cellFactory =
                        MapToTableCell.forTableColumn(getOptionsList(model.item.initialConditions, model.item.macros))
                }

            }

            column("Formula", KMappingInfo::formula) {
                cellFactory = FormulaTableCell.forTableColumn()
                fire(DataMappingChangeEvent(scope))
            }

            column("Description", KMappingInfo::descriptionProperty).makeEditable()

            onEditCommit {
                if (it != null) setStatus(it)
                validateEstimation(model)
                fire(ChartingOptionsUpdatingEvent(scope))
                refresh()

            }
            enableDirtyTracking()
            subscribe<SaveSessionRequest> { _ ->
                editModel.commit()
            }
            subscribe<CovariateMappingChanged> {
                refresh()
            }
            subscribe<DatatatableStructureChangeEvent> {
                items = model.item.mappingInfoSet.mappings.values.toList().asObservable()
                fire(DataMappingChangeEvent(scope))
                fire(ChartingOptionsUpdatingEvent(scope))
                refresh()
            }
        }

        // This is a hack. Need fixing. Still?
        x.items.forEach { item ->
            if (item.usedFor == KMappingInfo.MappingType.ESTIMATION) {
                item.usedFor = KMappingInfo.MappingType.IGNORE
                item.usedFor = KMappingInfo.MappingType.ESTIMATION
            }
            item.usedForProperty.onChange { usedFor ->
                if (usedFor != KMappingInfo.MappingType.ESTIMATION) {
                    item.mapTo = ""
                    item.formula = ""
                }
                if (usedFor == KMappingInfo.MappingType.TIME) {
                    x.items.filter { other ->
                        other != item && other.usedFor == KMappingInfo.MappingType.TIME
                    }.forEach { other ->
                        other.usedForProperty.value = KMappingInfo.MappingType.IGNORE
                    }
                }
                fire(DataMappingChangeEvent(scope))
            }

        }

        return x
    }

    class MyStringConverter<T>() : StringConverter<T>() {
        override fun toString(p0: T): String {
            return p0 as String
        }

        override fun fromString(p0: String?): T {
            return p0 as T
        }
    }

    init {
        subscribe<DataMappingChangeEvent> {
            validateEstimation(model)
        }
    }

    private fun setStatus(mappingInfo: KMappingInfo) {
        if (mappingInfo.usedFor != KMappingInfo.MappingType.ESTIMATION) {
            mappingInfo.status = KMappingInfo.MappingStatus.NONE
        } else if (mappingInfo.mapTo != "FORMULA") {
            println("Not a formula")
            mappingInfo.status = KMappingInfo.MappingStatus.VALID
        } else {
            println("A formula. Will evaluate.")
            val errors = model.item.modelObj.validateFormula(mappingInfo.formula)
            mappingInfo.status =
                if (errors.isEmpty()) KMappingInfo.MappingStatus.VALID else KMappingInfo.MappingStatus.FAILED
        }
    }
}
