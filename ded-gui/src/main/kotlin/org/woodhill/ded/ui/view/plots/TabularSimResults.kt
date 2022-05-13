package org.woodhill.ded.ui.view.plots

import javafx.event.EventTarget
import javafx.scene.control.TableView
import javafx.scene.control.ToggleGroup
import javafx.scene.layout.VBox
import org.woodhill.ded.models.TabularSimulationResults
import org.woodhill.ded.results.SimulationResultWrapper
import org.woodhill.ded.ui.chartoptions.ChartableControlsTB
import tornadofx.*
import java.util.stream.IntStream
import kotlin.math.abs

fun doubleDisplay(n:Any):String = if ((abs(n as Double)) < 0.01 && (abs(n as Double)) != 0.0) "%e".format(n) else "%.2f".format(n)

fun EventTarget.createTabularTable(simulationResultWrappers: List<SimulationResultWrapper>, chartableControls: ChartableControlsTB): VBox {
    val toggleGroup = ToggleGroup()
    val b = vbox {
        hbox {
            form {
                fieldset {
                    field("Variables in") {
                        radiobutton("Rows", toggleGroup)
                        radiobutton("Columns", toggleGroup)
                    }
                }
            }
        }

    }

    val simResultTable = TabularSimulationResults(simulationResultWrappers, chartableControls)

    val byRow = b.createRowTabularSimResultTable(simResultTable)

    val byCol = b.createColumnTabularSimResultTable(simResultTable)

    // Always start with by column. Later can think about saving the state.
    //b.add(byCol)
    byRow.removeFromParent()

    toggleGroup.selectToggle(toggleGroup.toggles[1])

    toggleGroup.selectedToggleProperty().onChange {
        if (it != null) {
            b.children.removeAt(1)
            if (it == toggleGroup.toggles[0]) {
                b.add(byRow)
            } else {
                b.add(byCol)
            }
        }
    }
    return b;
}


fun VBox.createColumnTabularSimResultTable(simResultTable: TabularSimulationResults): TableView<List<Double>> {
    /**
     * Data when each column is a variable. items needs to be a set of rows starting with timepoint,
     * then value for each var in correct order. May be repeating work done in TabularSimulationResults.
     * Can optimize later.
     */


//    Why doesn't this work!!!
//    simResultTable.timepoints.zip(simResultTable.variablesByCol) {
//    }

    val tableData = observableListOf<List<Double>>()
    for ((indx, value)  in simResultTable.timepoints_mean.first.withIndex()) {
        val newList = mutableListOf(value)
        newList.addAll(simResultTable.variablesOrRows[indx])
        tableData.add(newList)
    }

    val tv = tableview<List<Double>> {
        items = tableData
    }

    tv.column("Timepoint", Double::class) {
        value{
            doubleDisplay(it.value[0])
        }
    }

    IntStream.range(1, tableData[0].size).forEach { i ->
        tv.column(simResultTable.varHeaders[i-1], Double::class) {
            value {
                doubleDisplay(it.value[i])
            }
        }
    }
    return tv
}

fun VBox.createRowTabularSimResultTable(simResultTable: TabularSimulationResults): TableView<List<Any>> {
    val tableData = observableListOf<List<Any>>(
    )

    for ((indx, value) in simResultTable.varHeaders.withIndex()) {
        val newList = mutableListOf<Any>(value)
        newList.addAll(simResultTable.variablesByCol[indx])
        tableData.addAll(newList)

    }

    val tv = tableview<List<Any>> {
        items = tableData
    }

    tv.column("Variable", String::class) {
        value{
            it.value[0]
        }
    }

    IntStream.range(1, tableData[0].size).forEach { i ->
        tv.column(doubleDisplay(simResultTable.timepoints_mean.first[i-1]), Double::class) {
            value {
                doubleDisplay(it.value[i])
            }
        }
    }

    return tv
}