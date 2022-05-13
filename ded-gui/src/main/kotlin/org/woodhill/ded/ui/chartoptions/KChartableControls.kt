package org.woodhill.ded.ui.chartoptions

import edu.rochester.urmc.cbim.dediscover.simulation.DataMappingInfo
import edu.rochester.urmc.cbim.dediscover.simulation.NameKey
import edu.rochester.urmc.cbim.jni.JCovariate
import edu.rochester.urmc.cbim.jni.JDDEModel
import edu.rochester.urmc.cbim.jni.JMacro
import org.woodhill.ded.models.KCovariate
import org.woodhill.ded.models.KMappingInfoSet
import org.woodhill.ded.models.KModel
import org.woodhill.ded.ui.chartoptions.ParameterChartOptions.DisplayStyle
import tornadofx.observableListOf


/**
 * Will keep track of what gets displayed. Different things get displayed:
 *  - Dependent Variables
 *  - Macros
 *  - Covariates
 *  - Data (from data table)
 *  - formulas (from column mapping)
 *  - Replicate Means -- when more than one data column maps to the same variable or formula
 */

class ChartableControlsTB(private val kmodel: KModel, val model: JDDEModel, val savedProps: KChartableProperties) {
    private val data = mutableMapOf<String, ParameterChartOptions>()
    private val names = mutableListOf<NameKey>()
    private val nameKeyMap: MutableMap<String, NameKey> = HashMap()
    private val depVarNames = model.dependentVariableNames.toSet();

    val chartableProperties = observableListOf<ParameterChartOptions>()

    // TODO Huge and ugly: Fix.

    fun init() {
        data.clear()
        names.clear()
        model.dependentVariableNames.sorted()
            .forEach { name: String ->
                data[name] = savedProps.getORDefault(name, ParameterChartOptions.ParamType.VARIABLE)
                val nk = NameKey(name, name)
                names.add(nk)
                nameKeyMap[name] = nk
            }

        model.jMacros.sortedBy {
            it.name
        }
            .forEach { m: JMacro ->
                data[m.name] = savedProps.getORDefault(m.name, ParameterChartOptions.ParamType.MACRO)
                val nk = NameKey(m.name)
                names.add(nk)
                nameKeyMap[m.name] = nk
            }

        model.jCovariates.sortedBy {
            it.name
        }
            .forEach { cov: JCovariate ->
                data[cov.name] = savedProps.getORDefault(cov.name, ParameterChartOptions.ParamType.COVARIATE)
                val nk = NameKey(cov.name)
                names.add(nk)
                nameKeyMap[cov.name] = nk
            }


        val mis = KMappingInfoSet.getMappingInfoSet(kmodel)
        val covariates = KCovariate.getCovariates(kmodel)
        val mappings = DataMappingInfo.getDataMappings(mis, covariates, depVarNames)

        /**
         * Formulas that are not a dependent variables.
         */
       mappings.forEach { info ->
            val nk = NameKey(info.formula)

            // TODO FIX: Quick hack to handle null formulas and symbol names, till issue with covariates fixed.
            // Problem occurs because built-in models do not have mapping info.
            if ((info.modelSymbolName == null && info.formula != null)
                && !data.containsKey(info.formula)
            ) {

                // TODO Using two different approached to set defaults: one is to call getOrDefault, the other to call savedProps.visible() (
                    // and others. Settle on one.

                data[nk.display] = ParameterChartOptions(
                    nk.display, savedProps.visible(
                        info.formula, false
                    ), savedProps
                        .log(info.formula, false), savedProps.curveOrShape(
                        info.formula, DisplayStyle.CURVE
                    ), ParameterChartOptions.ParamType.FORMULA
                )

                names.add(nk)
                nameKeyMap[info.formula] = nk
            }
        }

        /**
         * Mapped Data column will only appear in one kmapping info.
         */
        mappings.forEach { info ->
            info.dataColumnName.forEach { dataName ->
                val keyName = dataName + DATA_KEY
                val prop = savedProps.getORDefault(keyName, ParameterChartOptions.ParamType.DATA)
                if ((info.modelSymbolName == null) && (info.formula == null)) {
                    val nk = NameKey(
                        keyName, dataName
                    )
                    names.add(nk)
                    nameKeyMap[keyName] = nk
                    if (savedProps.visible(keyName, true)) {
                        makeMappedVisible(info)
                    }
                    data[keyName] = ParameterChartOptions(
                        nk.display,
                        prop.isVisible,
                        prop.useLog,
                        prop.displayStyle,
                        prop.paramType
                    )
                } else {

                    if (info.modelSymbolName != null) {
                        val nk = NameKey(
                            keyName, dataName + "["
                                    + info.modelSymbolName + "]", info.modelSymbolName
                        )
                        names.add(nk)
                        nameKeyMap[keyName] = nk
                        if (savedProps.visible(keyName, true)) {
                            makeMappedVisible(info)
                        }
                        data[keyName] = ParameterChartOptions(
                            nk.display,
                            prop.isVisible,
                            prop.useLog,
                            prop.displayStyle,
                            prop.paramType
                        )

                    } // Formula mapping
                    else {
                        val nk = NameKey(
                            keyName, dataName + "["
                                    + info.formula + "]", info.formula
                        )
                        names.add(nk)
                        nameKeyMap[keyName] = nk
                        if (savedProps.visible(keyName, true)) {
                            makeMappedVisible(info)
                        }
                        data[keyName] = ParameterChartOptions(
                            nk.display,
                            prop.isVisible,
                            prop.useLog,
                            prop.displayStyle,
                            prop.paramType
                        )
                    }
                }
            }
        }

        // Replicates?
        mappings.forEach { info ->
            println(info)
            if (info.dataColumnName.size > 1) {
                val keyName = info.modelSymbolName + "_MEAN"
                if (info.modelSymbolName != null) {
                    val prop = savedProps.getORDefault(keyName, ParameterChartOptions.ParamType.REPLICATE_MEAN)
                    val nk = NameKey(
                        keyName, info.modelSymbolName
                                + "[Mean]", info.modelSymbolName
                    )
                    names.add(nk)
                    nameKeyMap[keyName] = nk
                    data[keyName] = ParameterChartOptions(
                        nk.display,
                        prop.isVisible,
                        prop.useLog,
                        prop.displayStyle,
                        prop.paramType
                    )
                }
            }
        }


        chartableProperties.clear()
        // TODO Order them: dep-vars, data, ect.
        chartableProperties.addAll(data.values)
    }

    private fun makeMappedVisible(info: DataMappingInfo) {
        if (info.formula != null) {
            val opt = data[info.formula]
            if (opt != null) {
                data[info.formula] = ParameterChartOptions(
                    info.formula,
                    true, opt.useLog,
                    opt.displayStyle, opt.paramType
                )
            }
        }
        if (info.modelSymbolName != null) {
            val opt = data[info.modelSymbolName]
            if (opt != null) {
                data[info.modelSymbolName] = ParameterChartOptions(
                    info.modelSymbolName,
                    savedProps.visible(info.modelSymbolName, true),
                    opt.useLog, opt.displayStyle, opt.paramType
                )
            }
        }
    }

    fun getNames(): List<NameKey> = names


    /**
     * Whether any of the chart variables are plotted on a linear chart
     *
     * @return
     */
    fun hasLinearCharts(): Boolean {
        return data.values.any { !it.useLog }
    }

    /**
     * Whether any of the chart variables are plotted on a log chart
     *
     * @return
     */
    fun hasLogCharts(): Boolean {
        for (opt in data.values) {
            if (opt.useLog) {
                return true
            }
        }
        return false
    }

    /**
     * @return
     */
    fun isVisible(keys: List<String?>): Boolean {
        for (n in keys) {
            if (isVisible(n!!)) {
                return true
            }
        }
        return false
    }

    /**
     * Whether the chart variable is visible
     *
     * @return
     */
    fun isVisible(key: String): Boolean {
        return if (data.containsKey(key)) {
            data[key]!!.isVisible
        } else {
            false
        }
    }

    /**
     * Whether there is a chart option for the given key
     *
     * @param key
     * @return
     */
    operator fun contains(key: String?): Boolean {
        return data.containsKey(key)
    }

    /**
     * Get the display style for the chart variable
     *
     * @param key
     * @return
     */
    fun getDisplayStyle(key: String?): DisplayStyle {
        return if (data.containsKey(key)) {
            data[key]!!.displayStyle
        } else {
            DisplayStyle.CURVE
        }
    }

    /**
     * Whether the chart variable is plotted on a log scale.
     *
     * @param key
     * @return
     */
    fun isLog(key: String?): Boolean {
        val nk = nameKeyMap[key] ?: return false
        return if (isData(nk.key)) {
            isLog(nk.mappedSymbolName)
        } else {
            if (data.containsKey(key)) {
                data[key]!!.useLog
            } else {
                false
            }
        }
    }

    /**
     * @param name
     * @return
     */
    fun isData(name: String?): Boolean {
        return if (data.containsKey(name)) {
            data[name]!!.paramType == ParameterChartOptions.ParamType.DATA || data[name]!!.paramType == ParameterChartOptions.ParamType.REPLICATE_MEAN ||
                    data[name]!!.paramType == ParameterChartOptions.ParamType.FORMULA
        } else {
            false
        }
    }

    /*
     * Returns a list of the data columns that are visible.
     *
     * Could compute once, then cache if it becomes a performance issue.
     */
    val visibleDataColumns: List<String>
        get() {
            val ret = ArrayList<String>()
            for (name in getNames()) {
                if (isData(name.key) && isVisible(name.key)) {
                    ret.add(name.key)
                }
            }
            return ret
        }

    companion object {
        const val DATA_KEY = ".DATA"
    }

    fun refresh() = init()

    init {
        init()
    }
}
