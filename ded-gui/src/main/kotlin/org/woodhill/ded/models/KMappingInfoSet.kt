package org.woodhill.ded.models

import edu.rochester.urmc.cbim.dediscover.plotting.MappingInfoInterface
import org.woodhill.ded.util.DBLoadMethods
import org.woodhill.ded.util.DatatableFile
import tornadofx.isDouble

// Used to be private constructor. Made public to used with old model import. Need to revise.
class KMappingInfoSet(val model: KModel, val mappings: MutableMap<String, KMappingInfo>) : MappingInfoInterface {

    fun columnSize(): Int {
        return if (mappings.isEmpty())
            0
        else
            mappings.values.first().nbElements()
    }

    private fun getTimeColumn(): KMappingInfo? {
        val timepointsCol = mappings.values.filter {
            it.usedFor.equals(KMappingInfo.MappingType.TIME)
        }

        if (timepointsCol.isEmpty())
            return null
        return timepointsCol[0]
    }


    //val removedColumns = mutableSetOf<Int>()
    fun removeColumns(colName: List<String>) {
        //removedColumns.addAll(columnIndex)

        colName.forEach { mappings.remove(it) }
//     Changing the column index here causes problems  in the datatable display
//        mappings.forEach{
//            it.value.columnIndex = it.value.columnIndex - (columnIndex.filter {deletedIndex ->
//                deletedIndex < it.value.columnIndex
//            }.size)
//        }
    }

    /**
     * Remove the i'th element in each mapping.
     */
    fun deleteElementsAt(indx: Int) {
        mappings.values.forEach {
            it.deleteElementAt(indx)
        }
    }


    /**
     * Not the most direct
     */
    fun getEstimatedColumns(): List<KMappingInfo> = mappings.values.filter {
        it.usedFor == KMappingInfo.MappingType.ESTIMATION
    }

    fun getDataForEstimation(): List<List<Double>> {
        val ret = mutableListOf<List<Double>>()
        val timepointsCol = mappings.values.filter {
            it.usedFor.equals(KMappingInfo.MappingType.TIME)
        }
        if (timepointsCol.isEmpty())
            return ret

        getEstimatedColumns().forEach {
            ret.add(
                timepointsCol[0].dataList.zip(it.dataList).filter {
                    it.first.isDouble()
                }.map { if (it.second.isDouble()) it.second.toDouble() else Double.NaN })
        }
        return ret
    }

    // TODO Implement default time column
    fun updateMapping(newColumns: List<DatatableFile.ColumnInfo>) {
        val oldMapping = mappings.toMap()
        mappings.clear()
        var columnNo = 0
        newColumns.forEach {
            if (oldMapping.containsKey(it.name)) {
                val oldVal = oldMapping[it.name]
                if (oldVal != null) {
                    mappings[it.name] = KMappingInfo(
                        it.name,
                        columnNo++,
                        oldVal.status,
                        oldVal.usedFor,
                        oldVal.mapTo,
                        oldVal.formula,
                        oldVal.description,
                        oldVal.failureMessage,
                        oldVal.data

                    )
                } else {
                    // TODO Implement logger, this is the place to use it
                    println("Warning: Something mapped to null...")
                }
            } else {
                mappings[it.name] = KMappingInfo(
                    it.name,
                    columnNo++,
                    KMappingInfo.MappingStatus.NONE,
                    KMappingInfo.MappingType.IGNORE,
                    "",
                    "",
                    "",
                    "",
                    it.values.joinToString(separator = ",")
                )
            }
        }
    }

    fun cloneForModel(toModel: KModel): KMappingInfoSet {
        val toMappings = mappings.mapValues {
            it.value.clone()
        }
        return KMappingInfoSet(toModel, toMappings.toMutableMap())
    }

    // Add an element before index
    fun addRowBefore(index: Int) {
        mappings.values.forEach {
            it.addElementAt(index)
        }
    }

    fun addColumnAt(loc: Int, name: String) {
        mappings.values.filter { it.columnIndex >= loc }.forEach {
            it.columnIndex += 1
        }
        mappings[name] = KMappingInfo(name, loc)
        mappings[name]?.appendElement("", columnSize())
    }

    fun reindexColumns(newOrder: List<String>) {
        newOrder.forEachIndexed() { indx, value ->
            mappings[value]?.columnIndex = indx
        }
    }

    companion object {
        private val mappingInfoSets = mutableMapOf<KModel, KMappingInfoSet>()

        // AL2021 Currently only called on session creation, and in XDEDataSet.java
        // TODO cache in dbLoadMethods
        fun getMappingInfoSet(model: KModel): KMappingInfoSet {

            return mappingInfoSets.getOrPut(model) {
                if (model.modelID < 0) KMappingInfoSet(model, mutableMapOf())
                else {
                    val mappedColumns = mutableMapOf<String, KMappingInfo>()
                    DBLoadMethods.instance.mappingInfoForModel(model).values.forEach {
                        mappedColumns[it.column_name] = it  // TODO Same logic used elsewhere. Look into it.
                    }
                    KMappingInfoSet(model, mappedColumns)
                }
            }
        }
    }

    override fun nbMappings(): Int {
        return mappings.size
    }

    override fun timepoints(): List<Double> {
        val timepointsCol = mappings.values.filter {
            it.usedFor.equals(KMappingInfo.MappingType.TIME)
        }
        return if (timepointsCol.isEmpty())
            listOf()
        else
            timepointsCol[0].dataList.filter { it.isDouble() }.map {
                it.toDouble()
            }
    }

    // Make sure they are always returned in same order.
    override fun names(i: Int): String {
        return mappings.keys.toList().sorted()[i]
    }

    override fun data(i: Int): List<Double> {
        return mappings[names(i)]?.doubleAtTimepoints(getTimeColumn()!!) ?: listOf()
    }
}

