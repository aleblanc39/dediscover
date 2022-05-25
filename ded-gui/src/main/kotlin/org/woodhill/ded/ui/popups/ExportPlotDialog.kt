package org.woodhill.ded.ui.popups

import javafx.beans.property.SimpleStringProperty
import javafx.scene.paint.Color
import org.woodhill.ded.session.SessionElements
import org.woodhill.ded.ui.view.util.setBackgroundcolor
import tornadofx.*
import java.io.File
import java.nio.file.Files
import java.nio.file.Path
import java.nio.file.Paths

/**
 * Code for export dialog of plots. Right only defaults to jpg files, may add options for different format
 */


data class PlotsFileInfo(val filename:String, val fileType:String)

class ExportPlotDialog constructor(
    val sessionElements: SessionElements,
    val thisTile: String,
    val filePrefix: String,
    val availableTypes: List<String>
) : Fragment(thisTile) {


    fun coreFilename() = filePrefix + "-" + sessionElements.model.sessionName;

    var outputDir: Path = Paths.get(System.getProperty("user.home"))
    fun filename() = outputDir.resolve(coreFilename()).toString()

    private val fileTypeProperty = SimpleStringProperty(availableTypes[0])
    var fileType by fileTypeProperty

    private val errorMessageProperty = SimpleStringProperty("")
    private var errorMsg: String by errorMessageProperty

    private val fileProperty = SimpleStringProperty(filename())
    var fileValue by fileProperty

    private var finalFileValue: String = ""


    private fun currentFullPath():String {
        val fileParts = Paths.get(fileValue)
        val parentDir = fileParts.parent
        val fname = fileParts.fileName
        val fileValue = parentDir.resolve(fname).toString()

        if (!Files.exists(parentDir)) {
            try {
                Files.createDirectory(parentDir)
            } catch (e: Exception) {
                errorMsg = "Can not create directory $parentDir"
                return ""
            }
        }
        return fileValue
    }

    fun plotFileInfo() = PlotsFileInfo(finalFileValue, fileType )

    override val root = vbox {
        prefWidth = 750.0

        form {
            fieldset {

                hbox {
                    prefWidth = 100.0
                    field("Filename:") {
                        textfield(fileProperty) {
                            prefWidth = 450.0
                        }
                    }
                    button("Change").action {
                        val dirname = chooseDirectory(
                            "Select output directory",
                            owner = currentWindow,
                            initialDirectory = File(fileValue).parentFile
                        )

                        if (dirname != null) {
                            outputDir = Paths.get(dirname.toString())
                            val f = File(fileValue).name
                            fileValue = outputDir.resolve(f).toString()
                        }
                    }
                }

                field("File Type") {
                        combobox(
                            fileTypeProperty,
                            availableTypes
                        ) {
                        }
                }

            }
        }
        text(errorMessageProperty) {
            setBackgroundcolor(this, Color.RED)
            isVisible = false
            errorMessageProperty.onChange {
                this.isVisible = errorMessageProperty.value != ""
            }
        }
        hbox {
            button("Accept").action {
                finalFileValue = currentFullPath()
                if (finalFileValue.isNotEmpty()) {
                    close()
                }
            }
            button("Cancel").action {
                finalFileValue = ""
                close()
            }
        }
    }


    companion object {
        fun getExportFilenames(sessionElements: SessionElements, typeOptions:List<String>): PlotsFileInfo {
            val dialog = ExportPlotDialog(sessionElements, "Names For Plots", "Plot-", typeOptions)
            dialog.openModal(block = true)
            return dialog.plotFileInfo()

        }
    }
}