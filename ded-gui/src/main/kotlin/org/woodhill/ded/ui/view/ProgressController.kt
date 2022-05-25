package org.woodhill.ded.ui.view

import javafx.beans.property.SimpleBooleanProperty
import javafx.beans.property.SimpleStringProperty
import javafx.scene.control.ButtonType
import tornadofx.*

/**
 * TODO May make abstract, then create 2 subclasses for simulation, estimation, or just one class with more parameters (progress bar status, etc.)
 */
//class ProgressController(val cancelFunction: ()->Unit, val pauseFunction: ()->Unit,val resumeFunction: ()->Unit) : Fragment() {
class ProgressController() : Fragment() {
    val status: TaskStatus by inject()
    val messageProperty = SimpleStringProperty("")
    val waitMessageProperty = SimpleStringProperty("")

    val cancelInProgressProperty = SimpleBooleanProperty(false)

    var cancelFunction: ()->Unit = {}
    var pauseFunction: ()->Unit = {}
    var resumeFunction: ()->Unit = {}


    enum class ProcessStatus {RUNNING, PAUSED}
    var processStatus: ProcessStatus = ProcessStatus.RUNNING
    var isCancelled = false
    override val root=vbox {
        prefHeight = 100.0
        prefWidth = 200.0

        text("This is the displayed message").bind(messageProperty)
        label(status.message)
        progressbar(status.progress)
        hbox {
            togglebutton ("Pause") {
                enableWhen(!cancelInProgressProperty)

                action {
                    text = if (text == "Pause") {
                        pauseFunction()
                        "Resume"
                    } else {
                        resumeFunction()
                        "Pause"
                    }
                }
                processStatus = ProcessStatus.PAUSED

            }
            button("Cancel").apply {
                enableWhen(!cancelInProgressProperty)
                action  {
                    val alert =
                        confirmation(
                            "Cancel Confirmation",
                            content = "Are you sure you want to cancel?",
                            buttons = *arrayOf(ButtonType.YES, ButtonType.NO)
                        )
                    if (alert.result == ButtonType.NO) {
                        println("Won't cancel")
                        resumeFunction()
                    } else {
                        isCancelled = true
                        cancelInProgressProperty.value = true
                        cancelFunction()
                        waitMessageProperty.value = "Cancelling... please wait"
                    }
                }
            }
        }
        text("").bind(waitMessageProperty)
    }

    fun changeMessage(newMessage:String) {
       messageProperty.value = newMessage
    }


    companion object {
        fun getProgressControler(
            scope: Scope,
            cancelFunction: () -> Unit,
            pauseFunction: () -> Unit,
            resumeFunction: () -> Unit
        ): ProgressController {
            val pg = find(ProgressController::class, scope).apply {
                                       this.cancelFunction = cancelFunction
                this.resumeFunction = resumeFunction
                this.pauseFunction = pauseFunction


            }
            return pg
        }

    }
}






