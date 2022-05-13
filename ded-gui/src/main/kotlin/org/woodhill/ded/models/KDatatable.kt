package org.woodhill.ded.models

import javafx.beans.property.SimpleIntegerProperty
import javafx.beans.property.SimpleStringProperty
import javafx.collections.ObservableList
import org.woodhill.ded.util.DEDUtils
import tornadofx.observableListOf
import tornadofx.onChange
import java.io.File

/**
 * The datatable in the UI will expect the data on a row by row basis. We store it and compute mostly with
 * colum by column basis.
 *
 * Still not completely defined. Right now
 * Row deletion from GUI will only update the `rows` property, not the mappingInfoSet, BUT
 * ImportData updates the mappingInfoSet. May prove difficult to perform an undo.
 *
 * Maybe should change import to go directoy into rows, bypassing mappingInfoSet. Only change mappingInfoSet on save.
 *
 * BUT
 *
 * MappingInfoSet probably required in computation. So every update should modify mappingInfoSet.
 *
 */

class KDatatable(val mappingInfoSet: KMappingInfoSet) {
    val defaultNbCols = 5
    val defaultEmptyRows = 2
    var opOnChange: () -> Unit = {}
    val indexColumnName = "Index"

    val rows = mutableListOf<ObservableList<SimpleStringProperty>>()
    val colNames = mutableListOf<String>()

    init {
        update()
    }

    val changeCounter = SimpleIntegerProperty(0)

    fun hasNewData(): Boolean = rows.size > defaultEmptyRows

    fun update(emptyToAdd:Int = defaultEmptyRows) {
        rows.clear()
        colNames.clear()
        colNames.add(indexColumnName)

        // If there is no datatable defined yet (e.g. new session) will create a default datatable
        if (rows.isEmpty() && mappingInfoSet.columnSize() == 0) {
            DEDUtils.generateNewNames(defaultNbCols, "Default-Col-").forEachIndexed { indx, value ->
                mappingInfoSet.mappings[value] = KMappingInfo(value, indx)
            }
        }

        // Add two empty rows at the bottom
        mappingInfoSet.mappings.values.forEach { mappingInfo ->
            mappingInfo.appendElement("", emptyToAdd)
        }

        // Populate first column in datatable with the index
        repeat(mappingInfoSet.columnSize()) {
            rows.add(observableListOf(SimpleStringProperty(it.toString())))
        }

        // Create rows that can be used by javafx tableview.
        mappingInfoSet.mappings.values.sortedBy {
            it.columnIndex
        }.forEach { km ->
            colNames.add(km.column_name)
            km.dataList.asSequence().zip(rows.asSequence()).zip(IntRange(0, km.dataList.size - 1).asSequence())
                .forEach { rowVal ->
                    val prop = SimpleStringProperty(rowVal.first.first)
                    rowVal.first.second.add(prop)
                    prop.onChange {
                        km.changeElementAt(rowVal.second, prop.value)
                        if (rowVal.second + defaultEmptyRows >= km.dataList.size) {
                            addEmptyRow(rowVal.second)
                        }

                        opOnChange()
                    }
                }
        }
    }

    private fun addEmptyRow(rowModified: Int) {
        // If the last row has been modified, and defaultEmptyRows rows. Otherwise subtract one
        // for each row above
        val nbNewRows = defaultEmptyRows - (mappingInfoSet.columnSize() - rowModified -1)
        update(nbNewRows)
        changeCounter.value += 1
    }


    fun renameColumn(oldName: String, newName: String) {
        val mi = mappingInfoSet.mappings.values.findLast { it.column_name == oldName }
         if (mi == null) {
             throw Exception("Column missing. This is a programming error...")
         }
        println("Renaming column to $newName")
        mi.column_name = newName
        update(0)

    }

    fun reindex(colNames: List<String>) {
        mappingInfoSet.reindexColumns(colNames)
    }


    fun addColumn(location:Int, colName: String) {
        mappingInfoSet.addColumnAt(location, colName)
        update(0)
    }

    /**
     * Add a row above the row at the index
     */
    fun addRow(rowNumber: Int) {
        mappingInfoSet.addRowBefore(rowNumber)
        update(0)
        changeCounter.value += 1
    }

    fun deleteRows(rowNumbers: List<Int>) {
       rowNumbers.zip((0..rowNumbers.size-1).toList()).forEach {
            rows.removeAt(it.first - it.second)
            mappingInfoSet.deleteElementsAt(it.first - it.second)
        }

        update(0)
    }

    fun removeColumn(colName: String) {
        mappingInfoSet.removeColumns(listOf(colName))
    }

    /**
     * Export datatable as CSV file.
     */
    fun exportData(filename: String) {
        File(filename).bufferedWriter().use { out ->

            out.write(mappingInfoSet.mappings.values.sortedBy { it.columnIndex }.map { it.column_name }
                .joinToString(separator = ","))
            out.newLine()

            rows.subList(0, rows.size - defaultEmptyRows).forEach { row ->
                out.write(row.subList(1, row.size).joinToString(separator = ",") { it.value })
                out.newLine()
            }
        }
    }


}