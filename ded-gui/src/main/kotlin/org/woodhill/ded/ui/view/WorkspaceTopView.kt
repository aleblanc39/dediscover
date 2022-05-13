package org.woodhill.ded.ui.view

import javafx.scene.control.TabPane
import org.woodhill.ded.ui.view.datatable.DatatableView
import org.woodhill.ded.ui.view.estimation.EstimationMethodsView
import org.woodhill.ded.ui.view.estimation.EstimationParametersView
import org.woodhill.ded.ui.view.estimation.EstimationResultsView
import org.woodhill.ded.ui.view.modeleditor.ModelTabView
import org.woodhill.ded.ui.view.plots.PlotsTabView
import org.woodhill.ded.ui.view.simulation.SimMethodTabView
import org.woodhill.ded.workspace.TabbedModelsView
import tornadofx.*

class WorkspaceTopView() : View() {

    override val scope = super.scope as TabbedModelsView.SessionScope
    val model = scope.model

    override var root = tabpane (){
        tab<ModelTabView>()
        tab<SimMethodTabView>()
        tab<PlotsTabView>()
        tab<DatatableView>()
        tab<EstimationParametersView>()
        tab<EstimationMethodsView>()
        tab<EstimationResultsView>()
        tabClosingPolicy = TabPane.TabClosingPolicy.UNAVAILABLE
        useMaxWidth = true
    }

    enum class TABINDEX(val index:Int) {
        EQUATIONS(0),
        SIM_METHODS(1),
        PLOTS(2),
        DATATABLE(3),
        ESTIMATION_PARAMETERS(4),
        ESTIMATION_METHODS(5),
        ESTIMATION_RESULTS(6),
    }

    fun switchToEstimationResultTab() = scope.tabPane.selectionModel.select(TABINDEX.ESTIMATION_RESULTS.index)
    fun switchToDatatableTab() = scope.tabPane.selectionModel.select(TABINDEX.DATATABLE.index)

    override fun onDelete() {
        println("Deleted from WorkspaceTopView")
    }

}