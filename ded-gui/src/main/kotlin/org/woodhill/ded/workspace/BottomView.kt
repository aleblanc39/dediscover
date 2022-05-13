package org.woodhill.ded.workspace

import org.woodhill.ded.application.Styles
import tornadofx.*

class BottomView : View("My View") {


    override val root = hbox {
        addClass(Styles.bottomView)
    }
}
