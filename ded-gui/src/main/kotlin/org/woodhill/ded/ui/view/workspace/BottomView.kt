package org.woodhill.ded.ui.view.workspace

import org.woodhill.ded.application.Styles
import tornadofx.*

class BottomView : View("My View") {


    override val root = hbox {
        addClass(Styles.bottomView)
    }
}
