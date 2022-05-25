package org.woodhill.ded.ui.popups

import javafx.beans.property.SimpleObjectProperty
import javafx.beans.property.SimpleStringProperty
import org.woodhill.ded.session.SessionElements
import tornadofx.*
import java.io.File
import java.nio.file.Path
import java.nio.file.Paths

class ExportSingleFileDialog  constructor(val sessionElements: SessionElements, val thisTile:String, val filePrefix: String, val suffix:String = "csv"): Fragment(thisTile){


    fun coreFilename() = filePrefix + "-" + sessionElements.model.sessionName;

    fun filename() = outputDir.resolve(coreFilename() +  ".${suffix}").toString()

    val outputDirProperty = SimpleObjectProperty<Path>(Paths.get(System.getProperty("user.home")))
    var outputDir by outputDirProperty

    val fileProperty = SimpleStringProperty(filename())
    var file by fileProperty



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
                        val filename = chooseDirectory(
                            "Select output directory",
                            owner = currentWindow,
                            initialDirectory = File(file).parentFile
                        )

                        if (filename != null) {
                            outputDir = Paths.get(filename.toString())
                            val f = File(file).name
                            file = outputDir.resolve(f).toString()
                        }
                    }
                }

            }
        }
        hbox {
            button("Accept").action {
                close()
            }
            button("Cancel").action {
                file = ""
            }
        }
    }

}