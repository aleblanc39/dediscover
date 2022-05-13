package org.woodhill.ded.results

import edu.rochester.urmc.cbim.dediscover.plotting.DatasetDisplayConfig
import org.jfree.data.xy.XYDataset
import org.woodhill.ded.properties.KSimTabProperties
import org.woodhill.ded.ui.chartoptions.ChartableControlsTB

/**
 * Class to handle SimulationResultModels that are meant to be plotted. Will
 * contain a (raw) getSimulationResults()Model, and classes inheriting this
 * class will extract their values from the RawSimulationResultModel. Also
 * handles what is dirty.
 *
 */
open  abstract class AbstractChartableResultModel(
    val simulationResultWrappers: List<SimulationResultWrapper>,
    val chartableControlsTB: ChartableControlsTB,
    val simtabProps: KSimTabProperties
) : KPlotResultsModel {

    val displayConfigs = mutableMapOf<XYDataset, DatasetDisplayConfig>()

    /**
     * @param setNames
     * @return
     */
    override fun getResultsEndTime(setNames: Array<String>): Double {
        // TODO Functionaly shouldn't be here. Should really operate on the XYDataSet themselves which contain
        // ResultSetWraps.
        val x:List<Double> = simulationResultWrappers.mapNotNull{it.timepoints.max()}
        return if (x.isEmpty()) Double.MAX_VALUE else x.max()!!
    }
    /**
     * @param setNames
     * @return
     */
    override fun getResultsStartTime(setNames: Array<String>): Double {
        val x = simulationResultWrappers.mapNotNull{it.timepoints.min()}
        return if (x.isEmpty()) Double.MIN_VALUE else x.min()!!
    }
    /**
     * @param dataset
     * @return
     */
    override fun getDatasetConfig(dataset: XYDataset): DatasetDisplayConfig {
        return displayConfigs[dataset]!!
    }

    /**
     * @return the combineLogLinearSets
     */
    val isCombineLogLinearSets: Boolean
        get() = simtabProps.combineLinearLog.value

    /**
     * Returns an array containing the (parameter set) names of all the
     * solverResults
     *
     * @param simulationResultWrappers
     * @return
     */
    protected fun setNames(
        simulationResultWrappers: List<SimulationResultWrapper>
    ): Array<String> {
        val names: MutableList<String> = ArrayList()
        for (sr in simulationResultWrappers) {
            names.add(sr.setName!!)
        }
        return names.toTypedArray()
    }

}