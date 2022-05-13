package org.woodhill.ded.properties

import org.jfree.chart.ChartPanel
import org.jfree.chart.JFreeChart
import org.jfree.chart.axis.Axis
import org.jfree.chart.plot.PlotOrientation
import org.woodhill.ded.util.DBLoadMethods.Companion.instance
import java.awt.*
import java.lang.reflect.InvocationTargetException
import java.lang.reflect.Method
import java.util.*

class KPlotProperties():KSessionProperties() {


    private val CHART_TITLE_COLOR = "title.color"

    private val CHART_TITLE_FONT_NAME = "title.font.name"

    private val CHART_TITLE_VISIBLE = "title.visible"

    /**
     *
     */
    private val DOMAIN_AXIS_RANGE_MAX = "domain.range.max"

    /**
     *
     */
    private val DOMAIN_AXIS_RANGE_MIN = "domain.range.min"

    /**
     *
     */
    private val DOMAIN_TICKS = "domain.ticks"

    /**
     *
     */
    private val DOMAIN_TITLE = "domain.title"

    /**
     *
     */
    private val RANGE_AXIS_RANGE_MAX = "range.range.max"

    /**
     *
     */
    private val RANGE_AXIS_RANGE_MIN = "range.range.min"

    /**
     *
     */
    private val RANGE_TICKS = "range.ticks"

    /**
     *
     */
    private val RANGE_TITLE = "range.title"

    /**
     *
     */
    private val TITLE = "title"

    var titleKey: String? = null

    constructor(title: String):this() {
        titleKey = title
    }

    fun getBasename(): String {
        return KPlotProperties::class.java.name + ":" + titleKey + "_"
    }

    /**
     * @return
     */
    fun getDomainMax(): Double {
        return get(DOMAIN_AXIS_RANGE_MAX) as Double
    }

    /**
     * @return
     */
    fun getDomainMin(): Double {
        return get(DOMAIN_AXIS_RANGE_MIN) as Double
    }

    /**
     * @return
     */
    fun getDomainTitle(def: String?): String? {
        return if (containsKey(DOMAIN_TITLE)) {
            get(DOMAIN_TITLE) as String?
        } else {
            def
        }
    }

    /**
     * @return
     */
    fun getRangeMax(): Double {
        return get(RANGE_AXIS_RANGE_MAX) as Double
    }

    /**
     * @return
     */
    fun getRangeMin(): Double {
        return get(RANGE_AXIS_RANGE_MIN) as Double
    }

    /**
     * @return
     */
    fun getRangeTitle(def: String?): String? {
        return if (containsKey(RANGE_TITLE)) {
            get(RANGE_TITLE) as String?
        } else {
            def
        }
    }

    /**
     * @return
     */
    fun getTitle(def: String?): String? {
        return if (containsKey(TITLE)) {
            get(TITLE) as String?
        } else {
            def
        }
    }

    /**
     * @return
     */
    fun getTitleColor(def: Color?): Paint? {
        val color = get(CHART_TITLE_COLOR) as String?
        return if (color == null) {
            def
        } else {
            Color.decode(color)
        }
    }

    /**
     * @param font
     * @return
     */
    fun getTitleFont(font: Font?): Font? {
        val fontName = get(CHART_TITLE_FONT_NAME) as String?
        return fontName?.let { makeFont(it) } ?: font
    }

    /**
     * @return
     */
    fun hasDomainRange(): Boolean {
        return containsKey(DOMAIN_AXIS_RANGE_MIN)
    }

    /**
     * @return
     */
    fun hasRangeRange(): Boolean {
        return containsKey(RANGE_AXIS_RANGE_MIN)
    }

    /**
     * @return
     */
    fun isDomainTicksVisible(): Boolean {
        return containsKey(DOMAIN_TICKS) && get(DOMAIN_TICKS) as Boolean
    }

    /**
     * @return
     */
    fun isRangeTicksVisible(): Boolean {
        return containsKey(RANGE_TICKS) && get(RANGE_TICKS) as Boolean
    }

    /**
     * @param b
     * @return
     */
    fun isShowTitle(b: Boolean): Boolean {
        val visible = get(CHART_TITLE_VISIBLE) as String?
        return if (visible != null) {
            java.lang.Boolean.parseBoolean(visible)
        } else {
            b
        }
    }

    /**
     * @param upperBound
     */
    fun setDomainMax(upperBound: Double) {
        put(DOMAIN_AXIS_RANGE_MAX, upperBound)
    }

    /**
     * @param lowerBound
     */
    fun setDomainMin(lowerBound: Double) {
        put(DOMAIN_AXIS_RANGE_MIN, lowerBound)
    }

    /**
     * Whether the plot properties holds overriding min and max bounds for the
     * domain axis
     *
     * @param b
     */
    fun setDomainRange(b: Boolean) {
        if (!b) {
            remove(DOMAIN_AXIS_RANGE_MIN)
        }
    }

    /**
     * @param tickMarksVisible
     */
    fun setDomainTickMarks(tickMarksVisible: Boolean) {
        put(DOMAIN_TICKS, tickMarksVisible)
    }

    /**
     * @param title
     */
    fun setDomainTitle(title: String?) {
        if (title == null || title.isEmpty()) {
            remove(DOMAIN_TITLE)
        } else {
            put(DOMAIN_TITLE, title)
        }
    }

    /**
     * @param upperBound
     */
    fun setRangeMax(upperBound: Double) {
        put(RANGE_AXIS_RANGE_MAX, upperBound)
    }

    /**
     * @param lowerBound
     */
    fun setRangeMin(lowerBound: Double) {
        put(RANGE_AXIS_RANGE_MIN, lowerBound)
    }

    /**
     * Whether the plot properties holds overriding min and max bounds for the
     * range axis
     *
     * @param b
     */
    fun setRangeRange(b: Boolean) {
        if (!b) {
            remove(RANGE_AXIS_RANGE_MIN)
        }
    }

    /**
     * @param tickMarksVisible
     */
    fun setRangeTickMarks(tickMarksVisible: Boolean) {
        put(RANGE_TICKS, tickMarksVisible)
    }

    /**
     * @param title
     */
    fun setRangeTitle(title: String?) {
        if (title == null || title.isEmpty()) {
            remove(RANGE_TITLE)
        } else {
            put(RANGE_TITLE, title)
        }
    }

    /**
     * @param visible
     */
    fun setShowTitle(visible: Boolean) {
        put(CHART_TITLE_VISIBLE, visible.toString())
    }


    fun setTitle(t: String?) {
        t?.let { put(TITLE, it) } ?: remove(TITLE)
    }

    fun setTitleColor(c: Color) {
        put(CHART_TITLE_COLOR, c.rgb.toString())
    }

    /**
     */
    fun setTitleFont(font: Font) {
        put(CHART_TITLE_FONT_NAME, createFontString(font))
    }

    fun restoreChartProperties(chart: ChartPanel) {
        for (property in chartPanelProperties) {
            try {
                restoreChartProperty(chart, property, "ChartPanel")
            } catch (e: SecurityException) { // TODO Auto-generated catch block
                e.printStackTrace()
            } catch (e: IllegalArgumentException) { // TODO Auto-generated catch block
                e.printStackTrace()
            } catch (e: IllegalAccessException) { // TODO Auto-generated catch block
                e.printStackTrace()
            } catch (e: InvocationTargetException) { // TODO Auto-generated catch block
                e.printStackTrace()
            }
        }
        for (property in chartProperties) {
            try {
                restoreChartProperty(chart.chart, property, null)
            } catch (e: SecurityException) { // TODO Auto-generated catch block
                e.printStackTrace()
            } catch (e: IllegalArgumentException) { // TODO Auto-generated catch block
                e.printStackTrace()
            } catch (e: IllegalAccessException) { // TODO Auto-generated catch block
                e.printStackTrace()
            } catch (e: InvocationTargetException) { // TODO Auto-generated catch block
                e.printStackTrace()
            }
        }
        for (property in chartPlotProperties) {
            try {
                restoreChartProperty(
                    chart.chart.xyPlot, property,
                    "XYPlot"
                )
            } catch (e: SecurityException) { // TODO Auto-generated catch block
                e.printStackTrace()
            } catch (e: IllegalArgumentException) { // TODO Auto-generated catch block
                e.printStackTrace()
            } catch (e: IllegalAccessException) { // TODO Auto-generated catch block
                e.printStackTrace()
            } catch (e: InvocationTargetException) { // TODO Auto-generated catch block
                e.printStackTrace()
            }
        }
        for (property in axisProperties) {
            try {
                restoreChartProperty(
                    chart.chart.xyPlot
                        .domainAxis, property, "DomainAxis"
                )
                restoreChartProperty(
                    chart.chart.xyPlot
                        .rangeAxis, property, "RangeAxis"
                )
            } catch (e: SecurityException) { // TODO Auto-generated catch block
                e.printStackTrace()
            } catch (e: IllegalArgumentException) { // TODO Auto-generated catch block
                e.printStackTrace()
            } catch (e: IllegalAccessException) { // TODO Auto-generated catch block
                e.printStackTrace()
            } catch (e: InvocationTargetException) { // TODO Auto-generated catch block
                e.printStackTrace()
            }
        }
    }

    fun saveChartProperties(panel: ChartPanel) {
        for (property in chartPanelProperties) {
            try {
                saveChartProperty(panel, property, "ChartPanel")
            } catch (e: SecurityException) { // TODO Auto-generated catch block
                e.printStackTrace()
            } catch (e: NoSuchMethodException) { // TODO Auto-generated catch block
                e.printStackTrace()
            } catch (e: IllegalArgumentException) { // TODO Auto-generated catch block
                e.printStackTrace()
            } catch (e: IllegalAccessException) { // TODO Auto-generated catch block
                e.printStackTrace()
            } catch (e: InvocationTargetException) { // TODO Auto-generated catch block
                e.printStackTrace()
            }
        }
        saveChartProperties(panel.chart)
        saveChartProperties(
            panel.chart.xyPlot.domainAxis,
            "DomainAxis"
        )
        saveChartProperties(
            panel.chart.xyPlot.rangeAxis,
            "RangeAxis"
        )
    }

    fun saveChartProperties(axis: Axis, prefix: String?) {
        for (property in axisProperties) {
            try {
                saveChartProperty(axis, property, prefix)
            } catch (e: SecurityException) { // TODO Auto-generated catch block
                e.printStackTrace()
            } catch (e: NoSuchMethodException) { // TODO Auto-generated catch block
                e.printStackTrace()
            } catch (e: IllegalArgumentException) { // TODO Auto-generated catch block
                e.printStackTrace()
            } catch (e: IllegalAccessException) { // TODO Auto-generated catch block
                e.printStackTrace()
            } catch (e: InvocationTargetException) { // TODO Auto-generated catch block
                e.printStackTrace()
            }
        }
    }

    fun saveChartProperties(chart: JFreeChart) {
        for (property in chartProperties) {
            try {
                saveChartProperty(chart, property, null)
            } catch (e: SecurityException) { // TODO Auto-generated catch block
                e.printStackTrace()
            } catch (e: NoSuchMethodException) { // TODO Auto-generated catch block
                e.printStackTrace()
            } catch (e: IllegalArgumentException) { // TODO Auto-generated catch block
                e.printStackTrace()
            } catch (e: IllegalAccessException) { // TODO Auto-generated catch block
                e.printStackTrace()
            } catch (e: InvocationTargetException) { // TODO Auto-generated catch block
                e.printStackTrace()
            }
        }
        for (property in chartPlotProperties) {
            try {
                saveChartProperty(chart.xyPlot, property, "XYPlot")
            } catch (e: SecurityException) { // TODO Auto-generated catch block
                e.printStackTrace()
            } catch (e: NoSuchMethodException) { // TODO Auto-generated catch block
                e.printStackTrace()
            } catch (e: IllegalArgumentException) { // TODO Auto-generated catch block
                e.printStackTrace()
            } catch (e: IllegalAccessException) { // TODO Auto-generated catch block
                e.printStackTrace()
            } catch (e: InvocationTargetException) { // TODO Auto-generated catch block
                e.printStackTrace()
            }
        }
    }

    @Throws(
        IllegalArgumentException::class, IllegalAccessException::class,
        InvocationTargetException::class
    )
    private fun restoreChartProperty(
        chart: Any?, property: String,
        prefix: String?
    ) {
        if (chart == null) return
        for (m in chart.javaClass.methods) {
            if (m.name == "set$property" && m.parameterTypes.size == 1) {
                val argType = m.parameterTypes[0]
                val propValue: Any? = get(
                    (if (prefix != null) "$prefix." else "")
                            + property
                )
                if (propValue != null) {
                    if (argType.isAssignableFrom(Color::class.java)) {
                        m.invoke(chart, Color((propValue as Int)))
                    } else if (argType.isAssignableFrom(String::class.java)) {
                        m.invoke(chart, propValue as String)
                    } else if (argType.isAssignableFrom(Boolean::class.java)
                        || argType.isAssignableFrom(Boolean::class.javaPrimitiveType)
                    ) {
                        m.invoke(chart, propValue as Boolean)
                    } else if (argType.isAssignableFrom(Stroke::class.java)) {
                        m.invoke(chart, makeBasicStroke(propValue as String))
                    } else if (argType.isAssignableFrom(PlotOrientation::class.java)) {
                        m.invoke(
                            chart,
                            if (propValue as Boolean) PlotOrientation.HORIZONTAL else PlotOrientation.VERTICAL
                        )
                    } else if (argType.isAssignableFrom(Font::class.java)) {
                        m.invoke(chart, makeFont(propValue as String))
                    }
                }
            }
        }
    }

    /**
     */
    private fun makeBasicStroke(strokeVal: String): Stroke? {
        val vals = strokeVal.split(":").toTypedArray()
        return if (vals.size == 6) {
            BasicStroke(
                vals[0].toFloat(),
                vals[1].toInt(),
                vals[2].toInt(),
                vals[3].toFloat(),
                getFloatArray(vals[4]),
                vals[5].toFloat()
            )
        } else {
            BasicStroke(
                vals[0].toFloat(),
                vals[1].toInt(),
                vals[2].toInt(),
                vals[3].toFloat()
            )
        }
    }

    /**
     * @param string
     * @return
     */
    private fun getFloatArray(string: String): FloatArray? {
        val floats = string.split(",").toTypedArray()
        val values = FloatArray(floats.size)
        for (i in floats.indices) {
            values[i] = floats[i].toFloat()
        }
        return values
    }

    /**
     * @param chart
     * @param property
     * @throws NoSuchMethodException
     * @throws SecurityException
     * @throws InvocationTargetException
     * @throws IllegalAccessException
     * @throws IllegalArgumentException
     */
    @Throws(
        SecurityException::class,
        NoSuchMethodException::class,
        IllegalArgumentException::class,
        IllegalAccessException::class,
        InvocationTargetException::class
    )
    private fun saveChartProperty(
        chart: Any,
        property: String,
        prefix: String?
    ) {
        var getMethod: Method? = null
        getMethod = try {
            chart.javaClass.getMethod("get$property")
        } catch (e: NoSuchMethodException) {
            chart.javaClass.getMethod("is$property")
        }
        val method = getMethod!!.invoke(chart)
        if (method is Color) {
            println(
                "Value of " + property + " is "
                        + method.rgb + " class = " + method.javaClass
            )
            put(
                (if (prefix != null) "$prefix." else "") + property,
                method.rgb
            )
        } else if (method is String) {
            put((if (prefix != null) "$prefix." else "") + property, method)
        } else if (method is Boolean) {
            put((if (prefix != null) "$prefix." else "") + property, method)
        } else if (method is BasicStroke) {
            put(
                (if (prefix != null) "$prefix." else "") + property,
                makeBasicStrokeString(method)
            )
        } else if (method is PlotOrientation) {
            put(
                (if (prefix != null) "$prefix." else "") + property,
                method === PlotOrientation.HORIZONTAL
            )
        } else if (method is Font) {
            put(
                (if (prefix != null) "$prefix." else "") + property,
                createFontString(method)
            )
        }
    }

    /**
     */
    private fun createFontString(fontValueal: Font): String {
        val buf = StringBuffer()
        buf.append(fontValueal.name)
        buf.append(":")
        buf.append(fontValueal.style)
        buf.append(":")
        buf.append(fontValueal.size)
        return buf.toString()
    }

    private fun makeFont(fontString: String): Font? {
        val vals = fontString.split(":").toTypedArray()
        return if (vals.size == 3) {
            Font(vals[0], vals[1].toInt(), vals[2].toInt())
        } else {
            Font(vals[0], Font.PLAIN, 12)
        }
    }

    /**
     * @param basicStroke
     * @return
     */
    private fun makeBasicStrokeString(basicStroke: BasicStroke): String {
        val buf = StringBuilder()
        buf.append(basicStroke.lineWidth)
        buf.append(":")
        buf.append(basicStroke.endCap)
        buf.append(":")
        buf.append(basicStroke.lineJoin)
        buf.append(":")
        buf.append(basicStroke.miterLimit)
        buf.append(":")
        if (basicStroke.dashArray != null) {
            for (f in basicStroke.dashArray) {
                buf.append(f)
                buf.append(",")
            }
            buf.append(":")
            buf.append(basicStroke.dashPhase)
        }
        return buf.toString()
    }

    private val chartProperties: Set<String> = HashSet(
        Arrays.asList(
            *arrayOf(
                "BackgroundPaint", "BorderPaint",
                "AntiAlias", "BorderStroke", "TextAntiAlias"
            )
        )
    )
    private val chartPlotProperties: Set<String> = HashSet(
        Arrays.asList(
            *arrayOf(
                "BackgroundPaint", "OutlinePaint",
                "OutlineStroke", "Orientation"
            )
        )
    )
    private val chartPanelProperties: Set<String> = HashSet(
        Arrays.asList(*arrayOf("Background"))
    )
    private val axisProperties: Set<String> = HashSet(
        Arrays.asList(
            *arrayOf(
                "AxisLinePaint", "AxisLineStroke",
                "LabelFont", "LabelPaint", "TickLabelsVisible",
                "TickLabelFont"
            )
        )
    )


    companion object {
        val plotProperties = mutableMapOf<Pair<Int, String>, KPlotProperties>()

        fun getPlotProperties(modelID: Int, chartVar:String): KPlotProperties {
            val key = Pair(modelID, chartVar)
            if (!plotProperties.keys.contains(key)) {
                plotProperties[key] = instance.loadPlotPropertiesFromDB(modelID, chartVar)
            }
            return plotProperties[key]!!
        }
    }
}