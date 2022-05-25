package org.woodhill.ded.ui.view.datatable

import org.woodhill.ded.ui.view.workspace.DEDTabView
import tornadofx.vbox

class DatatableView: DEDTabView("Data") {

    override val root = vbox()
    init {
        with (root) {
            add<DataPane>()
            add<MappingPane>()
            add<CovariatePane>()

        }
     }

    override fun onDock() {
        super.onDock()
        disableOnInvalidModel(root)
    }
}