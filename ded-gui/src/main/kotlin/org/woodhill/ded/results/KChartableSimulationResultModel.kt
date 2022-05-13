package org.woodhill.ded.results

import edu.rochester.urmc.cbim.dediscover.plotting.*
import edu.rochester.urmc.cbim.dediscover.simulation.*
import org.jfree.data.xy.XYDataset
import org.woodhill.ded.models.KCovariate
import org.woodhill.ded.models.KMappingInfoSet
import org.woodhill.ded.properties.KSimTabProperties
import org.woodhill.ded.ui.chartoptions.ChartableControlsTB
import org.woodhill.ded.ui.view.util.SimulationChartingStyle
import java.util.*
import java.util.function.Function


class ChartableSimulationResultModel(
    solverResults: List<SimulationResultWrapper>,
    controlTB: ChartableControlsTB,
    val covariates: List<KCovariate>,
    val mappingInfoSet: KMappingInfoSet,
    simTabProperties: KSimTabProperties,
    private val depVarNames: Set<String>,
    private val plotsTitlePrefix: String
) :
    AbstractChartableResultModel(solverResults, controlTB, simTabProperties) {


    override val isValid: Boolean
        get() = TODO("not implemented") //To change initializer of created properties use File | Settings | File Templates.

    override val isTabular: Boolean
        get() = TODO("not implemented") //To change initializer of created properties use File | Settings | File Templates.

    override val isTabularTransposed: Boolean
        get() = TODO("not implemented") //To change initializer of created properties use File | Settings | File Templates.

    private val symbolMappedTo = Function<String, String> { datakey: String ->
        DataMappingInfo
            .getModelSymbolMappedTo(mappingInfoSet, covariates, datakey)
    }

    private val isFormulaMapping =
        Function<String, Boolean> { stringVal -> DataMappingInfo.isFormula(mappingInfoSet, stringVal) }

    private fun getDataMappings() = DataMappingInfo.getDataMappings(mappingInfoSet, covariates, depVarNames);


    /**
     * Get the datasets. This class will have the information to filter the
     * results based on timepoints, in this case the solver steps.
     *
     * @return
     */
    fun getDataSets(chartingStyle: SimulationChartingStyle): Iterable<Array<XYDataset>> {

        // sets is sent as parameter to all functions. This is from the old code. Not sure why this is not returned instead.
        val sets = LinkedList<Array<XYDataset>>()
        if (simulationResultWrappers.isNotEmpty()) {
            when (chartingStyle) {
                SimulationChartingStyle.ONE_CHART -> {
                    getCombinedSet(sets)
                }
                SimulationChartingStyle.BY_PARAM_SET -> {
                    getParamSetDataset(sets)
                }
                SimulationChartingStyle.GROUP_BY_PARAM -> {
                    getByParamDataset(sets)
                }
                SimulationChartingStyle.BY_PARAM -> {
                    getIndividualDatasets(sets)
                }
                SimulationChartingStyle.VAR_TO_VAR -> {
                    getVarToVarDataset(sets)
                }
                else -> {
                }
            }
        }
        return sets
    }

    /**
     * Get the parameter sets that create plots by variable. If multiple
     * ParameterSets are involved, each plot will have a data series for
     * each set for the given variable.
     *
     * @param sets
     */
    private fun getByParamDataset(sets: LinkedList<Array<XYDataset>>) {

        if (simulationResultWrappers.isNotEmpty()) { // To avoid concurency exception...(???)
            val allNames: List<String> =
                ArrayList(simulationResultWrappers[0].allNames)
            for (chartVar in allNames) {
                println("--- looking at $chartVar")
                if (chartableControlsTB.contains(chartVar)) {
                    val ds = ByParamDataSet(
                        chartableControlsTB,
                        chartVar,
                        mappingInfoSet,
                        getDataMappings(),
                        chartableControlsTB.isLog(chartVar),
                        chartableControlsTB.isVisible(chartVar),
                        symbolMappedTo,
                        isFormulaMapping,
                        *simulationResultWrappers.toTypedArray()
                    )
                    val l = ArrayList<ByParamDataSet>()
                    l.add(ds)
                    sets.add(arrayOf(ds))
                    displayConfigs[ds] = ParamDisplayConfig(
                        plotsTitlePrefix,
                        null,
                        chartVar, chartableControlsTB.isLog(chartVar),
                        this, true, Function<String, SeriesGrouping> { title: String ->
                            l[0].getSeriesGroupings(
                                title,
                                chartableControlsTB
                            )
                        }, null, *setNames(simulationResultWrappers)
                    )
                } else {
                    // AL2021 Still needed?
                    //makeAllDirty()
                }
            }
        }
    }

    /**
     * Get the datasets by parameter, across Parameter Sets
     *
     * @param sets
     */
    private fun getParamSetDataset(sets: LinkedList<Array<XYDataset>>) {
        simulationResultWrappers.forEach { res ->
            val setList: MutableList<XYDataset> = LinkedList()
            var logds: CombinedDataSet? = null
            var lineards: CombinedDataSet? = null
            if (chartableControlsTB.hasLogCharts()) {
                logds = CombinedDataSet(
                    chartableControlsTB,
                    mappingInfoSet,
                    getDataMappings(),
                    true,
                    symbolMappedTo,
                    isFormulaMapping,
                    res
                )
            }
            // }
            // if (hasLinearCharts(chartableControlsTB.data)
            // || (logds == null)) {
            if (chartableControlsTB.hasLinearCharts()) {
                lineards = CombinedDataSet(
                    chartableControlsTB,
                    mappingInfoSet,
                    getDataMappings(),
                    false,
                    symbolMappedTo,
                    isFormulaMapping,
                    res
                )
            }
            // }
            if (lineards != null) {
                setList.add(lineards)
                val tempSet: XDEDataSet = lineards
                displayConfigs[lineards] = SolverSetDisplayConfig(
                    plotsTitlePrefix,
                    res, false,
                    this@ChartableSimulationResultModel,
                    { title ->
                        tempSet
                            .getSeriesGroupings(
                                title,
                                chartableControlsTB
                            )
                    }, lineards.seriesKeys //determineSeriesList(lineards, logds)
                )
            }
            if (logds != null) {
                setList.add(logds)
                val tempSet: XDEDataSet = logds
                displayConfigs[logds] = SolverSetDisplayConfig(
                    plotsTitlePrefix,
                    res, true,
                    this@ChartableSimulationResultModel,
                    { title ->
                        tempSet.getSeriesGroupings(
                            title, chartableControlsTB
                        )
                    }, logds.seriesKeys //determineSeriesList(logds, lineards)
                )
            }
            if (isCombineLogLinearSets) {
                if (setList.size > 0) {
                    sets.add(setList.toTypedArray())
                }
            } else {
                for (s in setList) {
                    sets.add(arrayOf(s))
                }
            }

        }
    }

    private fun getIndividualDatasets(sets: LinkedList<Array<XYDataset>>) { // SeriesGroup shapeSeries = new SeriesGroup(1);
        simulationResultWrappers.forEach { res ->
            for (chartVar in res.allNames) {
                if (!chartableControlsTB.contains(chartVar)) {
                    //makeAllDirty()
                } else {
                    val mappedDataName = getMappedDataName(chartVar)
                    val ds = SingleParamDataSet(
                        chartableControlsTB,
                        res.setName,
                        mappingInfoSet,
                        getDataMappings(),
                        chartVar,
                        res,
                        chartableControlsTB.isVisible(chartVar),
                        isFormulaMapping
                    )
                    sets.add(arrayOf(ds))
                    displayConfigs[ds] = ParamDisplayConfig(
                        plotsTitlePrefix,
                        res.setName,
                        chartVar, chartableControlsTB.isLog(chartVar),
                        this@ChartableSimulationResultModel,
                        mappedDataName != null,
                        Function<String, SeriesGrouping> { title ->
                            ds.getSeriesGroupings(
                                title,
                                chartableControlsTB
                            )
                        }, null, res.setName
                    )
                }
            }
        }
    }

    /**
     * @param sets
     */
    private fun getVarToVarDataset(sets: LinkedList<Array<XYDataset>>) {
        simulationResultWrappers.forEach { res ->
            if ((simtabProps.xAxiDisplayVar.value.key.isNotEmpty())
                && (simtabProps.yAxiDisplayVar.value.key.isNotEmpty())
            ) {
                val ds = VarToVarDataSet(
                    simtabProps.xAxiDisplayVar.value.key,
                    simtabProps.yAxiDisplayVar.value.key,
                    res
                )
                sets.add(arrayOf(ds))
                displayConfigs[ds] = VarToVarDisplayConfig(
                    plotsTitlePrefix,
                    simtabProps.xAxiDisplayVar.value,
                    simtabProps.yAxiDisplayVar.value, res.setName,
                    chartableControlsTB
                        .isLog(simtabProps.yAxiDisplayVar.value.key),
                    this as AbstractChartableResultModel
                )
            }
        }
    }

    /**
     * @param chartVar
     *
     * In original code would return a column name if the chart var was the formula. Null Otherwise
     *
     * @return
     */
    private fun getMappedDataName(chartVar: String): String? {
        val record = mappingInfoSet.mappings.values.find { it.formula == chartVar } ?: return null
        return record.formula
    }

    private fun getCombinedSet(sets: LinkedList<Array<XYDataset>>) {

        if (simulationResultWrappers.isNotEmpty()) {
            val setList = mutableListOf<XYDataset>()
            var logds: CombinedDataSet? = null
            var lineards: CombinedDataSet? = null
            if (chartableControlsTB.hasLinearCharts()) {
                lineards = CombinedDataSet(
                    chartableControlsTB,
                    mappingInfoSet,
                    getDataMappings(),
                    false,
                    symbolMappedTo,
                    isFormulaMapping,
                    *simulationResultWrappers
                        .toTypedArray()
                )
                val tempSet: XDEDataSet = lineards
                displayConfigs[lineards] = CombinedDisplayConfig(
                    plotsTitlePrefix,
                    false,
                    this, Function<String, SeriesGrouping> { title ->
                        tempSet.getSeriesGroupings(
                            title,
                            chartableControlsTB
                        )
                    }, lineards.seriesKeys, //determineSeriesList(lineards, logds),
                    *setNames(simulationResultWrappers)
                )
                setList.add(lineards)
            }
            if (chartableControlsTB.hasLogCharts()) {
                logds = CombinedDataSet(
                    chartableControlsTB,
                    mappingInfoSet,
                    getDataMappings(),
                    true,
                    symbolMappedTo,
                    isFormulaMapping,
                    *simulationResultWrappers
                        .toTypedArray()
                )
                val tempSet: XDEDataSet = logds
                displayConfigs[logds] = CombinedDisplayConfig(
                    plotsTitlePrefix,
                    true,
                    this, Function<String, SeriesGrouping> { title ->
                        tempSet.getSeriesGroupings(
                            title,
                            chartableControlsTB
                        )
                    }, logds.seriesKeys, //determineSeriesList(logds, lineards),
                    *setNames(simulationResultWrappers)
                )
                setList.add(logds)
            }

            // AL Return only 1 or two list elements, each with 1 set?
            if (isCombineLogLinearSets) {
                sets.add(setList.toTypedArray())
            } else {
                for (s in setList) {
                    sets.add(arrayOf(s))
                }
            }
        }
    }

}