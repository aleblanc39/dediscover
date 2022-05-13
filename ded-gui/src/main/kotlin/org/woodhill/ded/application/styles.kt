package org.woodhill.ded.application

import javafx.geometry.Pos
import javafx.scene.paint.Color
import tornadofx.*

/**
 * Created by miguelius on 04/09/2017.
 */
class Styles : Stylesheet() {
    companion object {
        val wrapper by cssclass()
        val consola by cssclass()
        val modelTree by cssclass()
        val equationEditor by cssclass()
        val modelComponents by cssclass()
        val modelTables by cssclass()
        val mainWindow by cssclass()
        val tabbedModelsView by cssclass()
        val plotsTabView by cssclass()
        val bottomView by cssclass()
    }

    init {
        mainWindow {
            prefHeight = 800.px
            prefWidth = 1600.px
            backgroundColor += Color.LIGHTGRAY
        }

//
//        tabbedModelsView {
//            maxWidth = 5000.px
//        }
       menu {
            prefWidth = 85.px
            fillWidth = false
            alignment = Pos.CENTER


        }
        textArea and consola {
            baseColor= Color.BLACK
            fontFamily = "Consolas"
            textFill = Color.RED
        }

        bottomView {
            prefHeight = 10.px
        }

        modelTree {
            prefWidth = 200.px
            prefHeight = 900.px
            fillHeight = true
        }
        equationEditor {
            prefWidth = 700.px
            prefHeight = 600.px
            fillHeight = true
        }
        modelComponents {
            prefWidth = 700.px
            fillHeight = true

        }

        modelTables {

        }


        plotsTabView {

        }

    }
}
