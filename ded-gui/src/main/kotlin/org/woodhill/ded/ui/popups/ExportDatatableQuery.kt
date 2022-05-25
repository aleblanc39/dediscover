package org.woodhill.ded.ui.popups

import org.woodhill.ded.session.SessionElements

fun getDatatableExportFileName(sessionElemnts: SessionElements):String {
    val selectionWindows = ExportSingleFileDialog(sessionElemnts, "Export Datatable", "datatable-")
    selectionWindows.openModal(block = true)
    return selectionWindows.file

}