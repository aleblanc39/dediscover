package org.woodhill.ded.ui.fxext

import javafx.beans.property.SimpleBooleanProperty
import javafx.util.Callback

import javafx.beans.value.ObservableValue
import javafx.collections.ObservableList
import javafx.event.EventTarget
import javafx.scene.control.ContextMenu
import javafx.scene.control.TableColumn
import javafx.scene.control.TableView
import tornadofx.*
import kotlin.reflect.KProperty1


/**
 * For some reason the following code will add the context menu to all the columns of the table and act on only the
 * first one for which the check_column is used. The next function below does the work, but we need to explicitly call
 * the function from within the column{} block.
 *
 */
inline fun <reified S, T> TableView<S>.check_column(title: String, prop: KProperty1<S, ObservableValue<T>>, noinline op: TableColumn<S, T>.() -> Unit = {}): TableColumn<S, T> {
    val column = TableColumn<S, T>(title)
    column.cellValueFactory = Callback { prop.call(it.value) }
    column.contextMenu = contextmenu {
        item("Select All").action {
            column.tableView.items.forEach {
                val x = column.getCellObservableValue(it)
                (x as SimpleBooleanProperty).value= true
            }
        }
        item("Deselect All").action {
            column.tableView.items.forEach {
                val x = column.getCellObservableValue(it)
                (x as SimpleBooleanProperty).value= false

            }
        }
    }
    addColumnInternal(column)
    return column.also(op)
}


fun <S, T>columnCheckboxContextMenu(c:TableColumn<S, T>):ContextMenu {

    return c.contextmenu {
        item("Select All").action {
            c.tableView.items.forEach {
                val x = c.getCellObservableValue(it)
                (x as SimpleBooleanProperty).value= true
            }
        }
        item("Deselect All").action {
            c.tableView.items.forEach {
                val x = c.getCellObservableValue(it)
                (x as SimpleBooleanProperty).value= false

            }
        }
    }
}