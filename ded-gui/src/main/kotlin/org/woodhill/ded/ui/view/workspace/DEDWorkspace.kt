package org.woodhill.ded.ui.view.workspace

import javafx.application.Platform
import org.woodhill.ded.ui.view.util.notImplemented
import org.woodhill.ded.ui.popups.NewSessionBaseValues
import tornadofx.*

class DEDWorkspace : Workspace("DEDiscover") {
    val tabbedModels: TabbedModelsView by inject()
    val modelListView: ModelListView by inject()

    init {
        menubar {
            prefWidth = 800.0
            menu("File") {
                item("New Model").action {
                    val packageAndSession =
                        WorkspaceUtil.getNewSessionName(NewSessionBaseValues(topPackage = modelListView.topUserPackage)) { a, b, c ->
                            modelListView.validateNewSessionName(
                                a,
                                b,
                                c
                            )
                        }
                    if (packageAndSession.sessionName.trim().isNotEmpty()) {
                        tabbedModels.addNewModel(packageAndSession)
                        //fire(ModelAddedEvent(newModel))
                    }
                }
                item("Save") {
                    action {
                        tabbedModels.saveCurrentSession()
                    }
                    enableWhen(WorkspaceProperties.saveableSessionProperty)

                }
                separator()
                // @formatter:off
                item("Import SBML").action {
                    val kmodel = WorkspaceUtil.importSBML(
                        topPackage = modelListView.topUserPackage,
                        intermediatePackage = "sbml"
                    ) { a, b, c ->
                        modelListView.validateNewSessionName(a, b, c)
                    }
                    // @formatter:on

                    if (kmodel != null) {
                        tabbedModels.modelListView.addSession(kmodel)
                        tabbedModels.loadModelFromDB(kmodel.modelID)
                        //tabbedModels.addNewModel(packageAndSession)
                    }
//
//
//                    val packageAndSession =
//                        WorkspaceUtil.getNewSessionName(NewSessionBaseValues(topPackage = modelListView.topUserPackage, intermediatePackage = "sbml")) { a, b, c ->
//                            modelListView.validateNewSessionName(a,b,c)
//                        }
//                    if (packageAndSession.second.trim().isNotEmpty()) {


                    //   }
                }

                separator()
                item("Exit").action {
                    //tabbedModels.saveCurrentSession()
                    confirm(
                        header = "Exit Confirmation",
                        content = "Are you sure you want to exit?",
                        actionFn = { Platform.exit() })
                }

            }
            menu("Edit") {
                item("Undo").action {
                    notImplemented()
                }
                item("Redo").action {
                    notImplemented()
                }
                separator()
                item("Cut").action {
                    notImplemented()
                }
                item("Copy").action {
                    notImplemented()
                }
                item("Paste").action {
                    notImplemented()

                }
                separator()
                item("Select All").action {
                    notImplemented()
                }
                item("Find/Replace").action {
                    notImplemented()
                }
                enableWhen(WorkspaceProperties.hasOpenSessionProperty)

            }
            menu("ModelLib") {
                // notImplemented()
            }
            menu("Data") {
                item("Import").action {
                    tabbedModels.importDatatable()
                    tabbedModels.selectTab(SessionTopView.TABINDEX.DATATABLE)
                }
                item("Export").action {
                    tabbedModels.exportDatatable()
                }
                item("Clear Data").action {
                    notImplemented()

                }
                item("Clear Mapping").action {
                    notImplemented()

                }

                enableWhen(WorkspaceProperties.hasOpenSessionProperty)

            }
            menu("Run") {
                item("Parse").action {
                    notImplemented()

                }
                item("Simulate").action {
                    notImplemented()

                }
                item("Estimate").action {
                    notImplemented()

                }
                enableWhen(WorkspaceProperties.hasOpenSessionProperty)

            }
            menu("Export") {
                // TODO Move all implementation into tabbedModels? Would make it easier to export from a button in window.
                item("Simulation Results") { enableWhen(WorkspaceProperties.hasSimulationResultProperty) }.action {
                    val exportParams = tabbedModels.exportSimulationResultsDialog()
                    if (exportParams.jsonFile.trim().isNotEmpty()) {
                        tabbedModels.exportSimResultAsJson(exportParams.paramSetName, exportParams.jsonFile)
                    }
                    if (exportParams.csvFile.trim().isNotEmpty()) {
                        tabbedModels.exportSimResultAsCSV(exportParams.paramSetName, exportParams.csvFile)
                    }
                }

                item("Estimation Results") {
                    enableWhen(WorkspaceProperties.hasEstimationResultProperty)
                }.action {
                    val exportParams = tabbedModels.exportEstimationResultsDialog()
                    if (exportParams.jsonFile.trim().isNotEmpty()) {
                        tabbedModels.exportEstimationResult(exportParams.jsonFile, asCSV = false)
                    }
                    if (exportParams.csvFile.trim().isNotEmpty()) {
                        tabbedModels.exportEstimationResult(exportParams.csvFile, asCSV = true)
                    }
                }

                item("Plots") { enableWhen(WorkspaceProperties.hasSimulationResultProperty) }.action {
                    //val exportLoc =

                    tabbedModels.exportPlots()
                }
                enableWhen(WorkspaceProperties.hasOpenSessionProperty)


            }
            menu("Window") {
                //  notImplemented()
                enableWhen(WorkspaceProperties.hasOpenSessionProperty)

            }
            menu("Help") {
                //  notImplemented()
            }
        }
    }


    override fun onDock() {
        super.onDock()

        // Remove default buttons so they can be reordered.
        workspace.backButton.removeFromParent()
        workspace.forwardButton.removeFromParent()
        workspace.createButton.removeFromParent()
        workspace.saveButton.removeFromParent()
        workspace.deleteButton.removeFromParent()
        workspace.refreshButton.removeFromParent()

        workspace.add(workspace.createButton)
        workspace.add(workspace.saveButton)
        workspace.add(workspace.refreshButton)
        workspace.add(workspace.deleteButton)

        workspace.saveButton.enableWhen(WorkspaceProperties.saveableSessionProperty)
        workspace.deleteButton.enableWhen(WorkspaceProperties.deletableSessionProperty)
        // TODO Add more buttons.
    }
}