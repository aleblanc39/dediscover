package org.woodhill.ded.ui.view.datatable

import javafx.beans.property.SimpleStringProperty
import javafx.scene.control.TableView
import javafx.scene.input.KeyCode
import javafx.scene.input.KeyEvent
import javafx.scene.layout.HBox
import javafx.scene.layout.VBox
import javafx.stage.FileChooser
import org.woodhill.ded.session.SaveSessionRequest
import org.woodhill.ded.ui.view.DatatatableStructureChangeEvent
import org.woodhill.ded.ui.view.util.notImplemented
import org.woodhill.ded.ui.view.util.validateEstimation
import org.woodhill.ded.util.DEDUtils
import org.woodhill.ded.util.DatatableFile
import org.woodhill.ded.workspace.TabbedModelsView
import org.woodhill.ded.workspace.popups.RenameColumnQuery
import org.woodhill.ded.workspace.popups.getDatatableExportFileName
import tornadofx.*


class DataPane : Fragment("Data") {
    override val scope = super.scope as TabbedModelsView.SessionScope
    private val sessionViewModel = scope.model

    override val root = titledpane("Data") {
        vbox() {
            add(createDatatable())
            add(createButtons())
        }
    }

    private fun createButtons(): HBox {
        return hbox {
            button("Import").action {
                importData()
            }
            button("Export").action {
                val filename = getDatatableExportFileName(sessionViewModel.item)
                if (!filename.trim().isEmpty()) {
                    sessionViewModel.item.exportDatable(filename)
                }
            }
            button("Clear Data").action { notImplemented() }

        }
    }


    fun importData() {
        val box = root.content as VBox
        val filenames = chooseFile(
            "Data File to Import",
            owner = currentWindow,
            filters = arrayOf(FileChooser.ExtensionFilter("The Filters", "*.txt", "*.csv")),
            mode = FileChooserMode.Single
        )
        if (filenames.isNotEmpty()) {
            // Need to read, validate file
            val dataRead = DatatableFile(filenames[0].path)

            // TODO All updates should happen through datatable.
            // TODO Too specific here?
            sessionViewModel.item.mappingInfoSet.updateMapping(dataRead.columns)
            sessionViewModel.item.datatable.update()
            if (box != null) {
                box.clear()
                box.add(createDatatable())
                box.add(createButtons())
            }
            fire(DatatatableStructureChangeEvent(scope))
        }
    }


    private fun createDatatable(): TableView<List<SimpleStringProperty>> {
        val datatable = sessionViewModel.item.datatable
        datatable.opOnChange = { validateEstimation(sessionViewModel) }

        val thisTableView = tableview<List<SimpleStringProperty>>() {
            items = datatable.rows.asObservable()

            contextmenu {
                item("Rename column").action {
                    if (selectedColumn!!.text == datatable.indexColumnName) {
                        error("Can not delete the ${datatable.indexColumnName} column")
                    }
                    val newColName =
                        RenameColumnQuery.queryNewColumnName(selectedColumn!!.text, datatable.colNames.toSet())
                    if (newColName.isNotEmpty()) {
                        datatable.renameColumn(selectedColumn!!.text, newColName)
                        // Updating relevant values in datatable will not update the column name.
                        selectedColumn!!.text = newColName
                    }
                    fire(DatatatableStructureChangeEvent(scope))
                }

                item("Add row").action {
                    datatable.addRow(selectionModel.selectedIndex)
                }
                item("Add Column").action {

                    // TODO Should query for name, give chance to cancel
                    val colName = DEDUtils.generateNewName("New-Col-", datatable.colNames.toSet())
                    val selectedColumnIndex = columns.indexOf(selectedColumn!!)
                    datatable.addColumn(selectedColumnIndex, colName)

                    // Can only add a new colum nat the end
                    column(colName, String::class) {
                    }.makeEditable()

                    // Remove that column, then add it at the proper position
                    val newCol = columns[columns.size - 1]
                    columns.removeIf { it == newCol }
                    columns.add(selectedColumnIndex + 1, newCol)

                    // Shift the elements right from the new column position.
                    columns.subList(selectedColumnIndex, columns.size).forEachIndexed { index, colObj ->
                        colObj.value {
                            it.value[index + selectedColumnIndex]
                        }
                    }
                    fire(DatatatableStructureChangeEvent(scope))
                    refresh()

                }

                item("Delete Row(s)").action {
                    datatable.deleteRows(selectionModel.selectedIndices.filter { it < datatable.rows.size-2 })
                    refresh()
                }
                item("Delete Column").action {
                    // TODO javafx has no easy way to select multiple columns.
                    if (selectedColumn!!.text == datatable.indexColumnName) {
                        error("Can not delete the '${datatable.indexColumnName} column")
                    } else {
                        datatable.removeColumn(selectedColumn!!.text)
                        val selectedColumnIndex = columns.indexOf(selectedColumn!!)
                        columns.remove(selectedColumn)

                        // TODO The following code is written 3 times. Wrap in a method
                        // TODO Check for race conditions here: columns.onchange() will be triggered here.

                        columns.subList(selectedColumnIndex, columns.size).forEachIndexed { index, colObj ->
                            colObj.value {
                                it.value[index + selectedColumnIndex]
                            }
                        }

                        fire(DatatatableStructureChangeEvent(scope))
                        refresh()
                    }
                }
            }

            multiSelect(true)
            enableDirtyTracking()
            enableCellEditing()
            subscribe<SaveSessionRequest> { _ ->
                editModel.commit()
            }

            columns.onChange {
                val indexCol = columns.findLast { it.text == datatable.indexColumnName }
                if (indexCol != null) {
                    if (columns[0] != indexCol) {
                        columns.removeAt(columns.indexOf(indexCol))
                        columns.add(0, indexCol)
                    } else {
                        if (columns.size > 1) {
                            datatable.reindex(columns.map { it.text }.toList().subList(1, columns.size ))
                        }
                    }
                }
            }

            // TODO HACK ALERT: items.onChange doesn't seem to work when adding rows to the table.
            datatable.changeCounter.onChange { refresh() }


            addEventFilter(KeyEvent.KEY_PRESSED) { event ->
                if (event.code == KeyCode.TAB) {
                    if (selectionModel.selectedCells.size == 1 &&
                        selectionModel.selectedCells[0].column + 1 < datatable.mappingInfoSet.mappings.size
                    ) {

                        selectionModel.selectedCells[0].tableView.edit(
                            selectionModel.selectedCells[0].row,
                            selectionModel.selectedCells[0].tableView.columns[selectionModel.selectedCells[0].column + 1]
                        )
                    }
                }
            }
        }

        thisTableView.column("Index", String::class) {
            value {
                it.value[0]
            }
            comparator = Comparator() { i: String, i1: String ->
                i.toInt() - i1.toInt()
            }

        }
        datatable.colNames.subList(1, datatable.colNames.size).forEachIndexed { index, colName ->
            thisTableView.column(colName, String::class) {
                value {
                    it.value[index + 1]
                }
            }.makeEditable()
        }

        return thisTableView
    }

}

