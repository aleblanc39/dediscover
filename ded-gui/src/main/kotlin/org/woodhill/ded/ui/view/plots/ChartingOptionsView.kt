package org.woodhill.ded.ui.view.plots

import javafx.collections.ObservableList
import javafx.scene.control.ScrollPane
import javafx.scene.control.cell.ComboBoxTableCell
import javafx.scene.layout.HBox
import org.woodhill.ded.session.SaveSessionRequest
import org.woodhill.ded.ui.chartoptions.ParameterChartOptions
import org.woodhill.ded.ui.fxext.columnCheckboxContextMenu
import org.woodhill.ded.ui.view.PlotsUpdatingEvent
import org.woodhill.ded.ui.view.EquationsChangeEvent
import org.woodhill.ded.workspace.DEDTabView
import tornadofx.*

class ChartingOptionsUpdatingEvent(scope: Scope) : FXEvent(EventBus.RunOn.ApplicationThread, scope)


class ChartingOptionsView: DEDTabView("Charting Options") {
    private var tableViewEditModel: TableViewEditModel<ParameterChartOptions> by singleAssign()
    override val root = vbox () {
        add(paramTable())
        add(footer())
    }

    // TODO Global select vor visible, log
    fun paramTable(): ScrollPane = scrollpane {
        tableview<ParameterChartOptions> {
            title = "Charting Options"
            isEditable = true
            items = model.chartableControls.value.chartableProperties
            column("Type", ParameterChartOptions::paramTypeProperty)
            column("Name", ParameterChartOptions::name)
            column("Visible", ParameterChartOptions::isVisisbleProperty) {
                contextMenu = columnCheckboxContextMenu(this)
            }.makeEditable()
            column("Symbol", ParameterChartOptions::displayStyleProperty) {
               cellFactory = ComboBoxTableCell.forTableColumn(ParameterChartOptions.DisplayStyle.values().asList().asObservable())
            }
            column("Log Scale", ParameterChartOptions::logProperty) {
                contextMenu = columnCheckboxContextMenu(this)
            }.makeEditable()
            enableCellEditing()
            enableDirtyTracking()

            items.onChange {
                setCallback(items)
            }
            subscribe<EquationsChangeEvent> {
                refresh()
                setCallback(items)
            }
            subscribe<SaveSessionRequest> {
                editModel.commit()
            }
            subscribe<ChartingOptionsUpdatingEvent> {
                model.item.updateChartableProperties()
                //refresh()
                setCallback(items)
            }

            setCallback(items)
        }
    }

    private fun setCallback(items: ObservableList<ParameterChartOptions>) {
        items.forEach{item ->
            item.logProperty.onChange {
                fire(PlotsUpdatingEvent(scope))
            }
            item.isVisisbleProperty.onChange {
                fire(PlotsUpdatingEvent(scope))
            }
            item.displayStyleProperty.onChange {
                fire(PlotsUpdatingEvent(scope))
            }
        }
    }


    private fun  footer():HBox = hbox {
        button("Defaults") {
        }
        prefHeight = 30.0
    }

    init {
        subscribe<ChartingOptionsUpdatingEvent> { event ->
            model.chartableControls.value.refresh()
        }
    }
}

