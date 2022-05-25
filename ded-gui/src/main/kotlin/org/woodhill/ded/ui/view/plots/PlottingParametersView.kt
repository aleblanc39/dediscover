package org.woodhill.ded.ui.view.plots

import javafx.scene.control.TabPane
import org.woodhill.ded.ui.view.workspace.DEDTabView
import tornadofx.tabpane

class PlottingParametersView: DEDTabView() {

    override val root = tabpane() {

        tab<ParamConfigView>()
        tab<ChartingOptionsView>()
        // Add tabs for sensitivity test, charting options
        tabClosingPolicy = TabPane.TabClosingPolicy.UNAVAILABLE
    }

}