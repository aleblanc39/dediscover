package org.woodhill.ded.ui.view.plots

import edu.rochester.urmc.cbim.dediscover.plotting.DatasetDisplayConfig
import javafx.embed.swing.SwingNode
import javafx.scene.layout.HBox
import org.jfree.chart.ChartPanel
import org.jfree.chart.JFreeChart
import org.jfree.chart.plot.XYPlot
import org.jfree.data.xy.XYDataset
import org.woodhill.ded.results.ChartableSimulationResultModel
import org.woodhill.ded.results.SimulationResultWrapper
import org.woodhill.ded.ui.view.PlotsUpdatingEvent
import org.woodhill.ded.ui.view.util.SimulationChartingStyle
import org.woodhill.ded.ui.view.workspace.DEDTabView
import tornadofx.*

class PlotsDisplayView : DEDTabView("Charts") {

    val props = sessionElements.simTabProperties
    val depVarNames = model.modelObj.value.dependentVariableNames.toSet()
    val header = vbox {
        hbox {
            prefHeight = 30.0
            form() {
                fieldset() {
                    field() {
                        label("Group By:")
                        combobox(props.selectedDisplayStyle, SimulationChartingStyle.values().asList())

                        label("Combine Log/Linear Charts:")
                        checkbox(property = props.combineLinearLog)

                        label("Auto-refresh:")
                        checkbox(property = props.autoRefreshPlots)
                    }
                }
            }
        }
    }

    private fun getVarToVarRow(): HBox {
        return hbox {
            val vars = sessionElements.chartableControls.getNames().filter {
                !sessionElements.chartableControls.isData(it.key)
            }
            if (!vars.isEmpty()) {
                if (props.xAxiDisplayVar.value.key.isEmpty()) {
                    props.xAxiDisplayVar.value = vars.first()
                }
                if (props.yAxiDisplayVar.value.key.isEmpty()) {
                    props.yAxiDisplayVar.value = vars.last()
                }
            }
            prefHeight = 30.0
            form {
                fieldset {
                    field {
                        label("x-axis:")
                        combobox(props.xAxiDisplayVar, vars)

                        label("y-axis:")
                        combobox(props.yAxiDisplayVar, vars)
                    }
                }
            }
        }
    }


    val plots = gridpane {
        gridpaneColumnConstraints {
            minWidth = 100.0
            maxWidth = 300.0
            minHeight = 600.0
        }
    }

    private fun visibleSimulationResultWrappers() = sessionElements.solverResults.filter {
        simtabProperties.plotVisible(it.parameterSet.name, true) }.map{SimulationResultWrapper(it)}

    private val plotsPane = scrollpane(true) {
        minHeight = 600.0
        add(plots)
    }
    override val root = vbox() {
        add(header)
        add( if (simtabProperties.selectedDisplayStyle.value == SimulationChartingStyle.TABULAR) createTabularTable(visibleSimulationResultWrappers(), sessionElements.chartableControls) else plotsPane)
    }

    private fun plotData() {
        sessionElements.plots.clear()
        plotsPane.vvalue = 0.0
        if (props.selectedDisplayStyle.value == SimulationChartingStyle.TABULAR) {
            if (root.children.size == 2) {
                root.children.removeAt(1)
            }
            val srw = visibleSimulationResultWrappers()
            if (srw.isNotEmpty()) {
                root.add(createTabularTable(srw, sessionElements.chartableControls))
            }
            return;
        }

        val simulationModel = ChartableSimulationResultModel(
            visibleSimulationResultWrappers(),
            model.chartableControls.value,
            model.item.covariates,
            model.item.mappingInfoSet,
            model.item.simTabProperties, depVarNames, model.item.model.title
        )

        plots.clear()
        val datasets: Iterable<Array<XYDataset>> = simulationModel.getDataSets(props.selectedDisplayStyle.value)

        if (datasets != null) {
            var nbPlots = 0

            for (dataset in datasets) {
                if (dataset.isEmpty() ||
                    (dataset[0].seriesCount == 0
                    && (dataset.size < 2 || dataset[1].seriesCount == 0))
                ) {
                    println("Doesn't seem to be much in set...")
                    continue
                }
                val config: DatasetDisplayConfig = simulationModel.getDatasetConfig(dataset[0])
                val xyPlot = XYPlot(
                    dataset[0], config.createDomainAxis(), config.createRangeAxis(),
                    config.renderer
                )
                for (i in 1 until dataset.size) {
                    val secondaryConfig: DatasetDisplayConfig = simulationModel.getDatasetConfig(dataset[i])
                    if (dataset[i].seriesCount > 0) {
                        xyPlot.setRangeAxis(i, secondaryConfig.createRangeAxis())
                    }
                    xyPlot.setRenderer(i, secondaryConfig.renderer)
                    xyPlot.setDataset(i, dataset[i])
                    xyPlot.mapDatasetToRangeAxis(i, i)
                    xyPlot.noDataMessage = "No data to display"
                }
                val jfc = JFreeChart(config.title, null, xyPlot, config.hasLegend())
                sessionElements.plots.add(jfc)
                val chart = ChartPanel(jfc, true, true, true, true, true)
                config.configure(chart, jfc, xyPlot)
                val x = SwingNode()
                x.content = chart
                plots.add(x, nbPlots % 2, nbPlots / 2)
                plots.constraintsForRow(nbPlots / 2).prefHeight = 300.0
                plots.constraintsForRow(nbPlots / 2).minHeight = 300.0
                nbPlots++
            }
        }
    }

    init {

        props.selectedDisplayStyle.onChange {

            if (props.selectedDisplayStyle.value != SimulationChartingStyle.VAR_TO_VAR && header.children.size == 2) {
                header.children.removeAt(1)
            }
            if (props.selectedDisplayStyle.value == SimulationChartingStyle.TABULAR ) {
                root.children.removeAt(1)
                root.add(createTabularTable(visibleSimulationResultWrappers(), sessionElements.chartableControls))

            }

            if (props.selectedDisplayStyle.value == SimulationChartingStyle.VAR_TO_VAR && header.children.size == 1) {
                header.add(getVarToVarRow())
            }

           if (props.selectedDisplayStyle.value != SimulationChartingStyle.TABULAR && root.children[1] != plotsPane) {
               root.children.removeAt(1)
               root.add(plotsPane)
            }
            plotData()
        }


        props.xAxiDisplayVar.onChange {
            plotData()
        }
        props.yAxiDisplayVar.onChange {
            plotData()
        }

        subscribe<PlotsUpdatingEvent> {
            plotData()
        }
        plotData()
    }

}