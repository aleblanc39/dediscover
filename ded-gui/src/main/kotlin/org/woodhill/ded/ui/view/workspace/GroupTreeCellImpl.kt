package org.woodhill.ded.ui.view.workspace

import javafx.beans.binding.Bindings
import javafx.beans.property.SimpleBooleanProperty
import javafx.scene.control.Alert
import javafx.scene.control.ButtonType
import javafx.scene.control.TreeView
import javafx.scene.control.cell.TextFieldTreeCell
import javafx.scene.layout.Region
import javafx.scene.text.FontWeight
import org.woodhill.ded.models.KModel
import org.woodhill.ded.ui.popups.NewSessionBaseValues
import org.woodhill.ded.ui.view.ModelDeletedEventID
import org.woodhill.ded.ui.view.PackageRemovedEvent
import org.woodhill.ded.util.DBSaveMethods
import org.woodhill.ded.util.duplicateSession
import tornadofx.*

private fun newPackageEntry(p: Group): String =
    p.thisPackage.plus(if (p.thisPackage.isEmpty()) "" else ".").plus(p.name)

class GroupTreeCellImpl(private val tabbedModels: TabbedModelsView, treeView: TreeView<Group>) : TextFieldTreeCell<Group>() {

    // Avoiding conflict. Ref: https://stackoverflow.com/questions/44035263/accidental-override-the-following-declarations-have-the-same-jvm-signature
    private val thisTreeView = treeView

    private val allSelectedPackagesEditable = SimpleBooleanProperty(false)
    private val enablePackageAdd = SimpleBooleanProperty(true)
    private val allSelectedSessionDeletable = SimpleBooleanProperty(false)

    private val enableAllSessionOperations = SimpleBooleanProperty(true)


    // TODO Use corresponding meth
    private fun removeSessions(vararg ids: Int ) {
        ids.forEach {
            tabbedModels.closeSessionIfOpened(it)
            if (it> 0) {
                DBSaveMethods.instance.deleteSession(it)
            }
            tabbedModels.fire(ModelDeletedEventID(it))
        }
    }

    /**
     * Recursively remove a package with everything under.
     */
    private fun removePackage(pkgFullName: String) {
        val comps = tabbedModels.modelListView.getComponentsUnderPackage(pkgFullName)
        removeSessions(*comps.first.toIntArray())
        comps.second.forEach { _ ->
            DBSaveMethods.instance.deletePackage(pkgFullName)
            tabbedModels.fire(PackageRemovedEvent(pkgFullName))
        }
    }

    override fun updateItem(thisGroup: Group?, p1: Boolean) {
        super.updateItem(thisGroup, p1)
        if (thisGroup != null) {
            text = thisGroup.name

            if (thisGroup.id == 0) { // Package selected
                if (contextMenu != null)
                    contextMenu.items.clear()
                contextmenu {
                    menu("New") {
                        item("Package") {
                            action {
                                println(thisTreeView.selectionModel.selectedItems.size)
                                val newPackage = WorkspaceUtil.getNewPackageName(newPackageEntry(thisGroup)) { a ->
                                    tabbedModels.modelListView.validateNewPackageName(a)
                                }
                                if (newPackage.trim().isNotEmpty()) {
                                    tabbedModels.addPackage(newPackage)

                                }
                            }
                        }
                        item("Session").action {
                            // Return a triple: package, session name, session title. (Should create class)
                            val packageAndSession = WorkspaceUtil.getNewSessionName(
                                NewSessionBaseValues(
                                    topPackage = newPackageEntry(thisGroup)
                                )
                            ) { a, b, c -> tabbedModels.modelListView.validateNewSessionName(a, b, c) }
                            if (packageAndSession.sessionName.trim().isNotEmpty()) {
                                tabbedModels.addNewModel(packageAndSession)
                            }
                        }
                        enableWhen(Bindings.and(thisGroup.expandable, enablePackageAdd))
                    }


                    fun deleteConfirmation(msg:String) {
                        val alert = Alert(
                            Alert.AlertType.CONFIRMATION,
                            msg,
                            ButtonType.YES, ButtonType.NO
                        )

                        alert.title = "Delete Confirmation"
                        alert.headerText = "Delete confirmation"
                        alert.dialogPane.minHeight = Region.USE_PREF_SIZE
                        alert.showAndWait()

                        if (alert.result == ButtonType.YES) {
                            thisTreeView.selectionModel.selectedItems.map { it.value.fullName() }.forEach {
                                removePackage(it)
                            }

                        }
                    }

                    item("Delete") {
                        action {
                            /**
                             * User may select a package and its subpackage for deletion. May optimize later, but for now:
                             * 1 - Get all the full package names
                             * 2 - Call delete function with the package name, which will locate the node and delete
                             *     the package. If the node was previously deleted then will do nothing.
                             *
                             * Could optimize by detecting subpackages here.
                             */
                            if (thisTreeView.selectionModel.selectedIndices.size == 1) {
                                deleteConfirmation("Are you sure you want to delete everything under ${thisGroup.fullName()}?")
                            } else {
                                deleteConfirmation( "Are you sure you want to delete everything under the selected packages?")
                            }
                        }
                        enableWhen(allSelectedPackagesEditable)
                    }
                }
                //font = Font.font(Font.getFontNames()[0])

                style {
                    fontWeight = FontWeight.BOLD
                }

            } else {
                // font = Font.font(Font.getFontNames()[8])
                style {
                    fontWeight = FontWeight.NORMAL
                }
                if (contextMenu != null)
                    contextMenu.items.clear()
                contextmenu {
                    item("Open") {
                        action {
                            tabbedModels.loadModelFromDB(thisGroup.id)
                        }
                        // TODO Want to disable when the session is already opened.
                        // Figure out a clean flow
                        //enableWhen(sessionIsOpened(thisGroup.id))
                    }
                    item("Duplicate") {
                        action {
                            if (thisGroup.id < 0) {
                                tornadofx.error("Please save the session before duplicating.\n(Programmer laziness)")
                                return@action
                            }
                            val model = KModel.getKModel(thisGroup.id)

                            val thisPackage = newPackageEntry(thisGroup).substringBeforeLast('.')
                            val tailPackage = thisPackage.removePrefix(tabbedModels.modelListView.topUserPackage)
                            val intermediate = if (tailPackage == thisPackage) "" else tailPackage.removePrefix(".")
                            val packageAndSession = WorkspaceUtil.getNewSessionName(
                                NewSessionBaseValues(
                                    topPackage = tabbedModels.modelListView.topUserPackage,
                                    intermediatePackage = intermediate,
                                    sessionName = model.sessionName,
                                    sessionTitle = model.title
                                )
                            ) { a, b, c -> tabbedModels.modelListView.validateNewSessionName(a, b, c) }

                            println("Returned from getNewSessionName with $packageAndSession")

                            if (packageAndSession.sessionName.trim().isNotEmpty()) {
                                val newSession = duplicateSession(
                                    model.modelID,
                                    packageName = packageAndSession.pkg,
                                    newSessionName = packageAndSession.sessionName,
                                    newSessionTitle = packageAndSession.sessionTitle
                                )
                                tabbedModels.modelListView.addSession(
                                    sessionID = newSession.modelID,
                                    sessionName = newSession.sessionName,
                                    packageName = newSession.packageName,
                                    editable = true
                                )
                            }
                        }
                        enableWhen { enableAllSessionOperations }

                    }

                    item("Delete") {
                        action {
                            val alert = Alert(
                                Alert.AlertType.CONFIRMATION,
                                if (thisTreeView.selectionModel.selectedIndices.size == 1) "Are you sure you want to delete session ${thisGroup.name}?"
                                else  "Are you sure you want to delete these ${thisTreeView.selectionModel.selectedIndices.size} sessions?",
                                ButtonType.YES, ButtonType.NO
                            )
                            alert.title = "Delete Confirmation"
                            alert.headerText = "Delete confirmation"
                            alert.dialogPane.minHeight = Region.USE_PREF_SIZE
                            alert.showAndWait()
                            println("Result from alert: ${alert.result}")

                            if (alert.result == ButtonType.YES) {
                                removeSessions(*thisTreeView.selectionModel.selectedItems.map{it.value.id}.toIntArray())
                            }

                        }
                        enableWhen(allSelectedSessionDeletable)
                    }

                    item("Reload").action {
                        confirm(header = "Reload Confirmation",
                            content = "Are you sure you want to reload session ${thisGroup.name}. All your chenged will be deleted.",
                            actionFn = {

                            })
                        information("Not implemented")

                    }
                    item("Rename") {
                        action {
                            val model = KModel.getKModel(thisGroup.id)

                            val thisPackage = newPackageEntry(thisGroup).substringBeforeLast('.')
                            val tailPackage = thisPackage.removePrefix(tabbedModels.modelListView.topUserPackage)

                            val intermediate = if (tailPackage.isEmpty()) "" else tailPackage.removePrefix(".")
                            val packageAndSession = WorkspaceUtil.getNewSessionName(
                                NewSessionBaseValues(
                                    topPackage = tabbedModels.modelListView.topUserPackage,
                                    intermediatePackage = intermediate,
                                    sessionTitle = model.title,
                                    sessionName = model.sessionName
                                )
                            ) { a, b, c -> tabbedModels.modelListView.validateNewSessionName(a, b, c) }

                            if (packageAndSession.sessionName.trim().isNotEmpty()) {
                                tabbedModels.renameSession(
                                    thisGroup.id,
                                    packageAndSession.pkg,
                                    packageAndSession.sessionName,
                                    packageAndSession.sessionTitle
                                )
                            }
                        }
                        enableWhen {
                            Bindings.and(
                                enableAllSessionOperations,
                                KModel.getKModel(thisGroup.id).editableProperty
                            )
                        }
                    }
                }
            }
        }
    }

    init {
        thisTreeView.selectionModel.selectedIndices.onChange {

            val selectedPackages = thisTreeView.selectionModel.selectedItems.filter {
                it.value.id == 0
            }

            allSelectedPackagesEditable.value = selectedPackages.find { !it.value.editable.value } == null

            if (selectedPackages.isNotEmpty()  && selectedPackages.size != thisTreeView.selectionModel.selectedIndices.size) {
                val alert = Alert(
                    Alert.AlertType.WARNING,
                    "No operation will be available when both packages and session are selected"
                )
                alert.dialogPane.minHeight = Region.USE_PREF_SIZE
                alert.headerText = "Multiple Selection Warning"
                alert.showAndWait()
                thisTreeView.selectionModel.clearSelection()
            }

            enablePackageAdd.value = thisTreeView.selectionModel.selectedIndices.size <= 1
            enableAllSessionOperations.value = thisTreeView.selectionModel.selectedIndices.size <= 1



            allSelectedSessionDeletable.value = thisTreeView.selectionModel.selectedItems.filter {
                it.value.id != 0 && it.value.editable.value == false
            }.isEmpty()
        }
    }
}
