package org.woodhill.ded.util

import com.github.doyaaaaaken.kotlincsv.dsl.csvReader

/**
 * Purpose of this class is to handle the import/export of datatable related files.
 */


fun emptyLine(l:List<String>):Boolean = (l.size==1 && l[0].trim() == "")

class DatatableFile(filepath:String) {
    data class ColumnInfo(val name:String, val values:List<String>)


    private val _columns = mutableListOf<ColumnInfo>()
    val columns:List<ColumnInfo> = _columns


    init {
        // Reading the csv file. A little complex since we make headers optionals by having a blank line before having
        // data, and allow comment lines.
        val columnsRead = mutableListOf<MutableList<String>>()
        val headers = mutableListOf<String>()
        csvReader().open(filepath) {

            var hasHeaders = true

            while (true) {
                val lineRead = readNext() ?: break
                // readNext() always return a list of size at least one. On a blank line it return a list
                // containing one empty string
                val fieldsInLine = lineRead.size
                // Have not read any data yet and have encountered a blank line.
                if (columnsRead.size == 0 && emptyLine(lineRead)) {
                    hasHeaders = false
                    continue
                }

                // Either a blank line or comment line
                if ( emptyLine(lineRead)|| (lineRead[0].startsWith("//")) || lineRead[0].startsWith("#")) {
                    continue
                }

                // Contains data. Could be headers or data
                if (columnsRead.size == 0) {
                   repeat(fieldsInLine) {
                       columnsRead.add(mutableListOf())
                   }

                    if (hasHeaders) {
                        lineRead.forEach { headers.add(it) }
                    } else {
                        lineRead.zip(columnsRead) {
                            v, col -> col.add(v)
                        }
                        var cnt=0
                        repeat(fieldsInLine) {
                            headers.add("col_name_$cnt")
                            cnt++
                        }
                    }
                } else {
                    if (fieldsInLine != columnsRead.size) {
                        throw Exception("Mismatch in number of columns. Expected ${columnsRead.size}, got $fieldsInLine")
                    }
                    // TODO Don't duplicate the statement
                    lineRead.zip(columnsRead) {
                            v, col -> col.add(v)
                    }
                }
            }
        }

        columnsRead.zip(headers) {
            col, header -> _columns.add(ColumnInfo(header, col))
        }
    }

}