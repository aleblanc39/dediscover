package org.woodhill.ded.ui.view.util


enum class SimulationChartingStyle(val chartingStyle: String) {
    ONE_CHART("Combined"), BY_PARAM_SET("Parameter Set"), GROUP_BY_PARAM(
        "Variable"
    ),
    BY_PARAM("All Separate"), VAR_TO_VAR(
        "Variable-To-Variable"
    ),
    SEP("--------"), TABULAR("Tabular");

    override fun toString(): String {
        return chartingStyle
    }

    companion object {
        private val map = values().associateBy(SimulationChartingStyle::chartingStyle)
        fun fromString(type: String) = map[type]
    }

}
