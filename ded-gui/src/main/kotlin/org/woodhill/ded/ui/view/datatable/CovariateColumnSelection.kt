package org.woodhill.ded.ui.view.datatable

import javafx.scene.control.MultipleSelectionModel
import javafx.scene.control.SelectionMode
import org.woodhill.ded.models.KMappingInfo
import org.woodhill.ded.session.SessionViewModel
import tornadofx.*

class CovariateColumnSelection(model:SessionViewModel) : Fragment("Select Column(s)") {
    var mySelectionModel: MultipleSelectionModel<KMappingInfo> by singleAssign()

    val availableUseFor =
        listOf(KMappingInfo.MappingType.IGNORE, KMappingInfo.MappingType.COVARIATE, KMappingInfo.MappingType.PLOT_ONLY)
    val availableColumns = model.item.mappingInfoSet.mappings.values.filter {
        (it.usedFor in availableUseFor)
    }

    var returnedSelection = mutableListOf<KMappingInfo>()

    override val root = vbox() {


        listview<KMappingInfo>() {
            mySelectionModel = selectionModel
            items = availableColumns.asObservable()
            selectionModel.selectionMode = SelectionMode.MULTIPLE
            cellFormat {
                text = it.column_name
            }
        }
        hbox {
            button("Accept").action {
                mySelectionModel.selectedItems.forEach{returnedSelection.add(it)}
                close()
            }
            button("Cancel").action {
                close()
            }
        }
    }

    init {
        availableColumns.forEach {
            if (it.usedFor == KMappingInfo.MappingType.COVARIATE) {
                mySelectionModel.select(it)
            }
        }
    }

    companion object {
        fun getMappedColumns(model:SessionViewModel): List<KMappingInfo> {
            println("Will get mapped columns")
            val selectionWindow = CovariateColumnSelection(model)
            selectionWindow.openModal(block = true)
            val returned = selectionWindow.returnedSelection
            selectionWindow.close()
            return returned
        }
    }
}