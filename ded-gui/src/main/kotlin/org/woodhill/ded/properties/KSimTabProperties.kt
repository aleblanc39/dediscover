package org.woodhill.ded.properties

import edu.rochester.urmc.cbim.dediscover.simulation.NameKey
import javafx.beans.property.SimpleBooleanProperty
import javafx.beans.property.SimpleDoubleProperty
import javafx.beans.property.SimpleObjectProperty
import javafx.beans.property.SimpleStringProperty
import org.woodhill.ded.models.KModel
import org.woodhill.ded.ui.view.util.SimulationChartingStyle
import tornadofx.onChange
import java.util.*

/**
 * TODO 1: We're using a combination of enum PROPERTIES and const variables for the property
 * names. Should use one or the other, not both.
 *
 * TODO 2: Create a map with the names in properties. Then can save all props by iterating through map.
 * Put this in base class, apply same method for all tabs.
 */
class KSimTabProperties(model: KModel) : KTabProperties(model) {

    override fun tabName(): String = "simtab"

    enum class PROPERTIES() {
        combineLogLinear,
        autoRefreshPlots,
        selectedDisplayStyle,
        VARS_ON_ROWS,
        VARS_ON_COLS;
    }

    val simulationStartTime = SimpleDoubleProperty(0.0)
    val simulationStopTime = SimpleDoubleProperty(5.0)
    val simulationStepSize = SimpleDoubleProperty(0.1)

    val combineLinearLog = SimpleBooleanProperty(true)
    val autoRefreshPlots = SimpleBooleanProperty(true)
    val selectedDisplayStyle = SimpleObjectProperty<SimulationChartingStyle>(SimulationChartingStyle.ONE_CHART)

    val xAxiDisplayVar = SimpleObjectProperty<NameKey>(NameKey())
    val yAxiDisplayVar = SimpleObjectProperty<NameKey>(NameKey())

    private val varsOnCols = SimpleBooleanProperty(true)
    private val varsOnRows = SimpleBooleanProperty(false)

    val currentParameterSetProperty = SimpleStringProperty("Default")

    override fun initializeProperties() {
        simulationStartTime.value = get(START_TIME, "0.0").toDouble()
        simulationStopTime.value = get(STOP_TIME, "5.0").toDouble()
        simulationStepSize.value = get(INTERVAL, "0.1").toDouble()

        varsOnCols.value = get(PROPERTIES.VARS_ON_COLS.toString())?.toBoolean() ?: true
        varsOnRows.value = get(PROPERTIES.VARS_ON_ROWS.toString())?.toBoolean() ?: false

        combineLinearLog.value = get(PROPERTIES.combineLogLinear.toString())?.toBoolean() ?: true
        autoRefreshPlots.value = get(PROPERTIES.autoRefreshPlots.toString())?.toBoolean() ?: true
        selectedDisplayStyle.value = SimulationChartingStyle.fromString(
            get(PROPERTIES.selectedDisplayStyle.toString()) ?: SimulationChartingStyle.ONE_CHART.toString()
        )

        xAxiDisplayVar.value = nameKey(get(X_AXIS_DISPLAY, ""))
        yAxiDisplayVar.value = nameKey(get(Y_AXIS_DISPLAY, ""))

        currentParameterSetProperty.value = get(SELECTED_PARAM_SET, "Default")

        currentParameterSetProperty.onChange {
            put(SELECTED_PARAM_SET, currentParameterSetProperty.value)
        }
    }


    private fun nameKey(nk: String): NameKey {
        if (nk.isEmpty())
            return NameKey()
        val parts = nk.split( ":")
        return NameKey(parts[0], parts[1], parts[2])
    }

    private fun nkToString(nk:NameKey): String = "${nk.key}:${nk.display}:${nk.mappedSymbolName}"

    fun setPlotVisibility(paramSet: String, visible: Boolean) {
        put(PLOT_VISIBLE + paramSet, visible)
    }

    fun plotVisible(paramSet: String, def: Boolean) = get(PLOT_VISIBLE + paramSet)?.toBoolean() ?: def

    fun setLogBucketing(log: Boolean) {
        put(LOG_BUCKETING, log)
    }

    fun sensitivityLogBucketing(def: Boolean): Boolean {
        val b =
            get(LOG_BUCKETING) as Boolean
        return b ?: def
    }



    fun setTransposeTabular(value: Boolean) {
        put(TABULAR_DATA_TRANSPOSED, value)
    }

    fun transposeTabular(def: Boolean): Boolean {
        val b =
            get(TABULAR_DATA_TRANSPOSED) as Boolean
        return b ?: def
    }

    fun setCombineLogLinear(value: Boolean) {
        put(COMBINE_LOG_AND_LINEAR, value)
    }

//    fun combineLogLinear(def: Boolean): Boolean {
//        val b =
//            get(COMBINE_LOG_AND_LINEAR)
//        return if (b==null) def else (b as Boolean)
//    }

    fun setGroupBySelection(value: SimulationChartingStyle) {
        put(GROUP_BY_SELECTION, value.chartingStyle)
    }

    fun groupBySelection(def: SimulationChartingStyle): SimulationChartingStyle {
        val b =
            get(GROUP_BY_SELECTION) as String
        return if (b == null) {
            def
        } else {
            SimulationChartingStyle.valueOf(b)
        }
    }

    fun setSelectedParamSet(paramSet: String) {
        put(SELECTED_PARAM_SET, paramSet)
    }

    fun selectedParamSet(def: String): String {
        val b = get(SELECTED_PARAM_SET) ?: return def
        return b
    }

    /**
     *
     */
    fun clearPlotVisible() {
        for (key in LinkedList(keySet())) {
            if (key.startsWith(PLOT_VISIBLE)) {
                remove(key)
            }
        }
    }

    /**
     */
    fun setTransposeSensitivityTabular(value: Boolean) {
        put(
            TABULAR_SENSITIVITY_DATA_TRANSPOSED,
            value
        )
    }

    fun transposeSensitivityTabular(def: Boolean): Boolean {
        val b =
            get(TABULAR_SENSITIVITY_DATA_TRANSPOSED) as Boolean
        return b ?: def
    }
    /**
     */
//    public void setSensitivityChartStyle(SensitivityGraphStyle value) {
//        put(SENSITIVITY_GRAPH_STYLE_SELECTION, value.name());
//    }
//
//    /**
//     * @param lines
//     * @return
//     */
//    public SensitivityGraphStyle sensitivityChartStyle(SensitivityGraphStyle def) {
//        String b = (String) get(SENSITIVITY_GRAPH_STYLE_SELECTION);
//        if (b == null) {
//            return def;
//        } else {
//            return SensitivityGraphStyle.valueOf(b);
//        }
//    }
    /**
     * @return
     */
    fun selectedNumberSensitivityTests(def: Int): Int {
        val b =
            get(NUM_SENSITIVITY_TESTS) as Int
        return b ?: def
    }

    /**
     */
    fun setSelectedNumberSensitivityTests(value: Int) {
        put(NUM_SENSITIVITY_TESTS, value)
    }

    /**
     * @return
     */
    fun quantileValue(def: String): String {
        val b =
            get(QUANTILE_VALUE)
        return b?.toString() ?: def
    }

    /**
     * @param value
     */
    fun setQuantileValue(value: String) {
        put(QUANTILE_VALUE, value)
    }
    /**
     * @return
     */
//    public SetSelectionMethod testSelectionMethod(SetSelectionMethod def) {
//        String b = (String) get(TEST_SELECTION_METHOD);
//        if (b == null) {
//            return def;
//        } else {
//            return SetSelectionMethod.valueOf(b);
//        }
//    }
//
//    /**
//     */
//    public void setTestSelectionMethod(SetSelectionMethod value) {
//        put(TEST_SELECTION_METHOD, value.name());
//    }
    /**
     * @return
     */
    fun selectedNumberBins(def: Int): Int {
        val b =
            get(NUM_SENSITIVITY_BINS) as Int
        return b ?: def
    }

    /**
     * @param value
     */
    fun setSelectedNumberBins(value: Int) {
        put(NUM_SENSITIVITY_BINS, value)
    }

    override fun updateProperties() {
        put(START_TIME, simulationStartTime.value)
        put(STOP_TIME, simulationStopTime.value)
        put(INTERVAL, simulationStepSize.value)

        put(PROPERTIES.selectedDisplayStyle.toString(), selectedDisplayStyle.value)
        put(PROPERTIES.autoRefreshPlots.toString(), autoRefreshPlots.value)
        put(PROPERTIES.combineLogLinear.toString(), combineLinearLog.value)

        put(PROPERTIES.VARS_ON_COLS.toString(), varsOnCols.value)
        put(PROPERTIES.VARS_ON_ROWS.toString(), varsOnRows.value)

        put(X_AXIS_DISPLAY, nkToString(xAxiDisplayVar.value))
        put(Y_AXIS_DISPLAY, nkToString(yAxiDisplayVar.value))
    }

    init {
        simulationStartTime.onChange {
            put(START_TIME, it)
        }
        simulationStopTime.onChange {
            put(STOP_TIME, it)
        }
        simulationStepSize.onChange {
            put(INTERVAL, it)
        }
    }

    companion object {

        private const val PLOT_VISIBLE = "PLOT_VISIBLE."
        private const val SELECTED_PARAM_SET = "SELECTED_PARAM_SET"
        private const val GROUP_BY_SELECTION = "GROUP_BY_SELECTION"
        private const val COMBINE_LOG_AND_LINEAR = "COMBINE_LOG_AND_LINEAR"
        private const val TABULAR_DATA_TRANSPOSED = "TABULAR_DATA_TRANSPOSED"
        private const val AUTO_REFRESH = "AUTO_REFRESH"
        private const val TABULAR_SENSITIVITY_DATA_TRANSPOSED = "TABULAR_SENSITIVITY_DATA_TRANSPOSED"
        private const val SENSITIVITY_GRAPH_STYLE_SELECTION = "SENSITIVITY_GRAPH_STYLE_SELECTION"
        private const val NUM_SENSITIVITY_TESTS = "NUM_SENSITIVITY_TESTS"
        private const val QUANTILE_VALUE = "QUANTILE_VALUE"
        private const val TEST_SELECTION_METHOD = "TEST_SELECTION_METHOD"
        private const val LOG_BUCKETING = "LOG_BUCKETING"
        private const val NUM_SENSITIVITY_BINS = "NUM_SENSITIVITY_BINS"
        private const val X_AXIS_DISPLAY = "X_AXIS_DISPLAY"
        private const val Y_AXIS_DISPLAY = "Y_AXIS_DISPLAY"



        private const val START_TIME = "START_TIME"
        private const val STOP_TIME = "STOP_TIME"
        private const val INTERVAL = "INTERVAL"


    }
}
