package org.woodhill.ded.ui.view.util

import edu.rochester.urmc.cbim.dediscover.simulation.ScientificFormatter
import javafx.scene.control.TableCell
import javafx.scene.control.TableColumn
import javafx.scene.control.TableView
import javafx.util.Callback
import tornadofx.column


// Hard code format for now. Can make it customizable later.

class DoubleResultTableCell<S> : TableCell<S,  Number>() {
    val formatter = ScientificFormatter(true, 3)

    init {
        isEditable = false
    }

    override fun updateItem(p0:  Number?, p1: Boolean) {
        super.updateItem(p0, p1)
        text = if (p0 == null)
            ""
        else
            formatter.format(p0)
    }


    companion object {
        fun <S> forTableColumn(): Callback<TableColumn<S, Number>, TableCell<S, Number>>? {
            return Callback {
                DoubleResultTableCell()
            }
        }
    }
}

inline fun <reified S> TableView<S>.doubleTableColumn(title:String, prop: kotlin.reflect.KProperty1<S, javafx.beans.value.ObservableValue<Number>>): TableColumn<S, Number> {

    return column(title, prop) {
        cellFactory = DoubleResultTableCell.forTableColumn<S>()
    }
}