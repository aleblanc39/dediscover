package org.woodhill.ded.ui.chartoptions

import javafx.beans.property.SimpleBooleanProperty
import javafx.beans.property.SimpleObjectProperty
import tornadofx.getValue
import tornadofx.setValue

open class ParameterChartOptions(
    var name: String,
    visible: Boolean = true, useLog: Boolean = false,
    style: DisplayStyle = DisplayStyle.CURVE, paramType: ParamType
) {

    // IDEA won't automatically import tornadofx.*

    val isVisisbleProperty = SimpleBooleanProperty(visible)
    var isVisible by isVisisbleProperty

    val logProperty = SimpleBooleanProperty(useLog)
    var useLog by logProperty

    val displayStyleProperty = SimpleObjectProperty<DisplayStyle>(style)
    var displayStyle by displayStyleProperty

    // TODO Currently not saved in DB. Don't know why.
    val paramTypeProperty = SimpleObjectProperty<ParamType>(paramType)
    val paramType by paramTypeProperty

    enum class DisplayStyle( val thisStyle: String) {
        CURVE("Curve"), SHAPE("Point"), BOTH("Both");

        override fun toString(): String {
            return thisStyle
        }
    }


    // Put other for now since the information is not, as yet, saved in database.
    enum class ParamType(val thisType: String) {
        MACRO("Macro"), DATA("Data"),
        VARIABLE("Variable"),
        COVARIATE("Covariate"),
        FORMULA("Formula"),
        REPLICATE_MEAN("Replicate Mean"),
        OTHER("Other");

        override fun toString(): String {
            return thisType
        }
    }

    /*
     * (non-Javadoc)
     *
     * @see java.lang.Object#toString()
     */
    override fun toString(): String {
        return ("ParameterChartOptions [visible=" + isVisible + ", log=" + useLog
                + ", style=" + displayStyle + ", paramType=" + paramType + "]")
    }

}
