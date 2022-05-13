package org.woodhill.ded.models

import javafx.beans.property.SimpleObjectProperty
import javafx.beans.property.SimpleStringProperty
import org.woodhill.ded.util.splitOnCommas
import tornadofx.*


open class KMappingInfo(
    var id: Int,
    var column_name: String,
    var columnIndex: Int,
    var status: MappingStatus,
    originalUsedFor: MappingType,
    originalMapTo: String,
    originalFormula: String,
    description: String,
    var failureMessage: String,
    var data: String
) {


    val descriptionProperty = SimpleStringProperty(description)
    val description: String by descriptionProperty

    val usedForProperty = SimpleObjectProperty<MappingType>(originalUsedFor)
    var usedFor by usedForProperty

    val formulaProperty = SimpleStringProperty(originalFormula)
    var formula: String by formulaProperty

    val mapToProperty = SimpleStringProperty(originalMapTo)
    var mapTo: String by mapToProperty

    constructor(
        column_name: String,
        columnIndex: Int,
        status: MappingStatus = MappingStatus.NONE,
        originalUsedFor: MappingType = MappingType.IGNORE,
        mapTo: String = "",
        formula: String = "",
        description: String = "",
        failureMessage: String = "",
        data: String = ""
    ) : this(
        -1, column_name,
        columnIndex,
        status,
        originalUsedFor,
        mapTo,
        formula,
        description,
        failureMessage,
        data
    )

    enum class MappingStatus(val status: Int) {
        VALID(0), FAILED(1), NONE(2);

        companion object {
            fun valueOf(value:Int):MappingStatus? = values().find { it.status == value }
        }
    }

    enum class MappingType(val mappingType: String) {
        IGNORE("Ignore"), TIME("Time"), ESTIMATION("Estimation"),  COVARIATE("Covariate"), PLOT_ONLY("Plot Only")
    }

    private var dataAsList: MutableList<String> = splitOnCommas(data)!!.toMutableList()
    val dataList: List<String> get() = dataAsList.toList()
    fun nbElements(): Int = dataAsList.size


    // Only used in DatapointDataSer
    fun doubleAtTimepoints(timeColumn: KMappingInfo): List<Double> {
        return timeColumn.dataList.zip(dataList).filter {
            it.first.isDouble()
        }.map { if (it.second.isDouble()) it.second.toDouble() else Double.NaN }

    }

    fun deleteElementAt(index: Int) {
        if (index >= dataAsList.size) {
            throw Exception("Error with array size")
        }
        dataAsList.removeAt(index)
        data =  dataToString()
    }

    fun isFormula(): Boolean = (usedFor == MappingType.ESTIMATION) && (formula.trim().isNotEmpty())

    fun changeElementAt(index: Int, value: String) {
        dataAsList[index] = value
        data =  dataToString()
    }

    fun appendElement(value:String, nbElements:Int = 1) {
        dataAsList.addAll(generateSequence { "" }.take(nbElements).toList())
        data = dataToString()
    }

    fun addElementAt(index:Int, value:String = "") {
        dataAsList.add(index, value)
        data = dataToString()
    }

    private fun dataToString():String {
        return dataAsList.joinToString(separator = ",")
    }

    fun dataSublist(start: Int, end: Int):String {
        return dataAsList.subList(start, end).joinToString(separator = ",")

    }

    fun clone():KMappingInfo = KMappingInfo(column_name, columnIndex, status, usedFor, mapTo, formula, description, failureMessage, data)


}