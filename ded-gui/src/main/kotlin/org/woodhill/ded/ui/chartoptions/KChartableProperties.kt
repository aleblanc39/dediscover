package org.woodhill.ded.ui.chartoptions

import org.woodhill.ded.models.KModel
import org.woodhill.ded.util.DBLoadMethods.Companion.instance


/**
 * Contains the properties that control which symbols are displayed, and how they will be. There can be entries that
 * do not correspond to any currently defined symbols as some may be removed after the model is parsed and they are
 * not removed.
 */
class KChartableProperties private constructor(val model:KModel) {

    private val chartingOptions = mutableMapOf<String, ParameterChartOptions>()
    // val parameterChartOptions:  List<ParameterChartOptions> get() = chartingOptions.values.toList()

    val props = mutableMapOf<String, Any>()
    var dirty = false

    operator fun get(key: String): Any? {
        return props[key]
    }

    val defaultsToDots = listOf(ParameterChartOptions.ParamType.DATA, ParameterChartOptions.ParamType.REPLICATE_MEAN)
    fun getORDefault(key: String, paramType: ParameterChartOptions.ParamType): ParameterChartOptions {
        if (!chartingOptions.containsKey(key) || !chartingOptions[key]?.paramType?.equals(paramType)!! ) {
            chartingOptions[key] = ParameterChartOptions(name=key, paramType = paramType, style =
                if (defaultsToDots.contains(paramType)) ParameterChartOptions.DisplayStyle.SHAPE else ParameterChartOptions.DisplayStyle.CURVE
            )
        }
        return chartingOptions[key]!!
    }

    fun addChartableProperty(key: String, parameterChartOptions: ParameterChartOptions) {
        chartingOptions[key] = parameterChartOptions
    }


    /**
     * Whether the parameter is currently visible
     *
     * @param paramName
     * @return
     */
    fun curveOrShape(
        paramName: String,
        def: ParameterChartOptions.DisplayStyle
    ): ParameterChartOptions.DisplayStyle {
        val b = get("$paramName.style")
        return if (b == null) def else  ParameterChartOptions.DisplayStyle.valueOf(b as String)
    }

    /**
     * Whether the parameter is currently log scale
     *
     * @param paramName
     * @return
     */
    fun log(paramName: String, def:Boolean): Boolean {
        //val prop = getORDefault(paramName)
        val b = get("$paramName.log")
        return if (b == null) false else b as Boolean
    }

    /**
     * Whether the parameter is currently visible
     *
     * @param paramName
     * @return
     */
    fun visible(paramName: String, def: Boolean): Boolean {
        val b = get("$paramName.visible")
        return  if (b==null) def else b as Boolean
    }

    fun equiv(a:Any?, b:Any?):Boolean {
        if ((a == null) && (b == null)) {
            return true;
        } else if ((a == null) || (b == null)) {
            return false;
        } else {
            return a.equals(b);
        }
    }

    companion object {
        val chartablePropertiesMap = mutableMapOf<KModel, KChartableProperties>()

        @JvmStatic
        fun getProperties(model:KModel): KChartableProperties {
            if (!chartablePropertiesMap.containsKey(model)) {
                val props = KChartableProperties(model)
                instance.loadChartableProperties(model, props)
                chartablePropertiesMap.put(model, props)
            }
            return chartablePropertiesMap[model]!!
        }
    }
}
