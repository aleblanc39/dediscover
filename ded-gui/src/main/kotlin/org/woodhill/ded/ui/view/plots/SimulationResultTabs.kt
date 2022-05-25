package org.woodhill.ded.ui.view.plots

import javafx.scene.control.TabPane
import org.woodhill.ded.ui.view.workspace.DEDTabView
import tornadofx.tabpane

class SimulationResultTabs:DEDTabView() {
    override val root = tabpane() {
        tab<PlotsDisplayView>()
        tab<ResidualsDisplayView>()
        tab<ParameterValuesDisplayView>()
        tabClosingPolicy = TabPane.TabClosingPolicy.UNAVAILABLE
    }

}