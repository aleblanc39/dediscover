package org.woodhill.ded.ui.popups

import javafx.beans.property.SimpleBooleanProperty
import javafx.beans.property.SimpleObjectProperty
import javafx.beans.property.SimpleStringProperty
import org.woodhill.ded.session.SessionElements
import tornadofx.*
import java.io.File
import java.nio.file.Path
import java.nio.file.Paths

data class ExportFilenames(val paramSetName:String = "", val csvFile: String = "", val jsonFile: String = "")


class CSVJsonExportQuery(val sessionElements: SessionElements, val thisTile:String, val filePrefix: String): Fragment(thisTile) {

    fun coreFilename() = filePrefix + "-" + sessionElements.model.sessionName + "-" + paramSetProperty.value;

    fun csvFileName() = csvOutputDir.resolve(coreFilename() +  ".csv").toString()
    fun jsonFileName() = jsonOutputDir.resolve(coreFilename() +  ".json").toString()

    val simtabProperties = sessionElements.simTabProperties

    val csvOutputDirProperty = SimpleObjectProperty<Path>(Paths.get(System.getProperty("user.home")))
    var csvOutputDir by csvOutputDirProperty

    val jsonOutputDirProperty = SimpleObjectProperty<Path>(Paths.get(System.getProperty("user.home")))
    var jsonOutputDir by jsonOutputDirProperty

    val paramSetProperty = SimpleStringProperty(simtabProperties.currentParameterSetProperty.value)

    val csvFileProperty = SimpleStringProperty(csvFileName())
    val jsonFileProperty = SimpleStringProperty(jsonFileName())

    val exportCSVProperty = SimpleBooleanProperty(true)
    val exportJSONProperty = SimpleBooleanProperty(true)

    var csvFile by csvFileProperty
    var jsonFile by jsonFileProperty

    val availableParamSetsList = sessionElements.solverResults.map {
        it.parameterSet.name
    }


    var exportConfig = ExportFilenames()

    override val root = vbox {
        prefWidth = 750.0

        form {
            fieldset {
                field("Parameter Set") {
                    combobox<String>(paramSetProperty) {
                        items = availableParamSetsList.asObservable()
                    }
                }
                hbox {
                    prefWidth = 100.0
                    checkbox(property = exportCSVProperty)
                    field("CSV file name:") {
                        textfield(csvFileProperty) {
                            prefWidth = 450.0
                        }
                    }
                    button("Change").action {
                        val filename = chooseDirectory(
                            "Select output directory",
                            owner = currentWindow,
                            initialDirectory = File(csvFile).parentFile
                        )

                        if (filename != null) {
                            csvOutputDir = Paths.get(filename.toString())
                            val f = File(csvFile).name
                            csvFile = csvOutputDir.resolve(f).toString()
                        }
                    }
                }
                hbox {
                    checkbox(property = exportJSONProperty)
                    field("JSON file name:") {
                        textfield(jsonFileProperty){
                            prefWidth = 450.0
                        }
                    }
                    button("Change").action {
                        val filename = chooseDirectory ("Select output directory",
                        owner = currentWindow,
                        initialDirectory = File(jsonFile).parentFile
                        )

                        if (filename != null) {
                            jsonOutputDir= Paths.get(filename.toString())
                            val f = File(jsonFile).name
                            jsonFile = jsonOutputDir.resolve(f).toString()
                        }
                    }
                }
            }
        }
        hbox {
            button("Accept").action {
                exportConfig = ExportFilenames(paramSetProperty.value,
                    if (exportCSVProperty.value) csvFile else "",
                    if (exportJSONProperty.value) jsonFile else "")
                close()
            }
            button("Cancel").action {
                close()
            }
        }
    }

    init {
        paramSetProperty.onChange {
            csvFile = csvFileName()
            jsonFile = jsonFileName()
        }
    }

    companion object {
        fun getSimulationResultFilenames(sessionElemnts: SessionElements):ExportFilenames? {
            val selectionWindows = CSVJsonExportQuery(sessionElemnts, "Export Simulation Results", "simResults")
            selectionWindows.openModal(block = true)
            return selectionWindows.exportConfig

        }
        fun getEstimationResultFilenames(sessionElemnts: SessionElements):ExportFilenames? {
            val selectionWindows = CSVJsonExportQuery(sessionElemnts, "Export Estimation Results", "estResults")
            selectionWindows.openModal(block = true)
            return selectionWindows.exportConfig

        }
    }
}



