package org.woodhill.ded.workspace

import javafx.beans.binding.Bindings
import javafx.beans.property.SimpleBooleanProperty
import javafx.beans.value.ObservableValue
import javafx.scene.control.*
import javafx.scene.control.cell.TextFieldTreeCell
import javafx.scene.layout.HBox
import javafx.scene.layout.Region
import javafx.scene.text.FontWeight
import org.woodhill.ded.application.Styles
import org.woodhill.ded.data.Group
import org.woodhill.ded.data.SessionTree
import org.woodhill.ded.models.KModel
import org.woodhill.ded.ui.view.*
import org.woodhill.ded.util.DBSaveMethods
import org.woodhill.ded.util.duplicateSession
import org.woodhill.ded.workspace.popups.NewSessionBaseValues
import sun.java2d.pipe.SpanShapeRenderer
import tornadofx.*

private fun newPackageEntry(p: Group): String =
    p.thisPackage.plus(if (p.thisPackage.isEmpty()) "" else ".").plus(p.name)



//fun sessionIsOpened(sessionID:Int): SimpleBooleanProperty {
//    return SimpleBooleanProperty()
//}

class GroupTreeCellImpl(private val tabbedModels: TabbedModelsView, treeView: TreeView<Group>) : TextFieldTreeCell<Group>() {

    // Avoiding conflict. Ref: https://stackoverflow.com/questions/44035263/accidental-override-the-following-declarations-have-the-same-jvm-signature
    private val thisTreeView = treeView

    private val allSelectedPackagesEditable = SimpleBooleanProperty(false)
    private val enablePackageAdd = SimpleBooleanProperty(true)
    private val allSelectedSessionDeletable = SimpleBooleanProperty(false)

    val enablePackageDelete = SimpleBooleanProperty(true)
    val enableMultipleDelete = SimpleBooleanProperty(true) // Rule same for packages and files
    private val enableAllSessionOperations = SimpleBooleanProperty(true)


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
                                error("Please save the session before duplicating.\n(Programmer laziness)")
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


fun selectSession(
    sessionName: String,
    packageName: String,
    node: TreeItem<Group>,
    selectionModel: MultipleSelectionModel<TreeItem<Group>>
): Boolean {
    val sessionUsed = if (sessionName.equals(""))
        packageName.substringAfterLast(".") else sessionName

    val packageUsed = if (sessionName.equals(""))
        packageName.substringBeforeLast(".") else packageName

    if ((node.value.thisPackage == packageUsed).and(node.value.name == sessionUsed)) {
        selectionModel.select(node)
        return true
    } else {
        for (n in node.children) {
            val inserted = selectSession(sessionName, packageName, n, selectionModel)
            if (inserted)
                return true
        }
        return false
    }
}


class ModelListView : View() {
    override val root = HBox()
    private val tabbedModels: TabbedModelsView by inject()
    private val sessionTree = SessionTree()

    val topUserPackage = sessionTree.topUserEditablePackage()
    fun removeSession(sessionID: Int) = sessionTree.removeSession(sessionID)
    fun addSession(sessionID: Int, packageName: String, sessionName: String, editable:Boolean) =
        sessionTree.addSession(sessionID, packageName, sessionName, editable)

    fun addSession(model: KModel) = sessionTree.addSession(model)
    fun getComponentsUnderPackage(fullPkgName: String) = sessionTree.componentsUnderPackage(fullPkgName)

    init {
        with(root) {
            //addClass(Styles.modelTree)
            vbox {
                treeview<Group> {
                    addClass(Styles.modelTree)
                    prefHeight=800.0
                    selectionModel.selectionMode = SelectionMode.MULTIPLE
                    root = TreeItem(sessionTree.rootNode)
                    root.isExpanded = true

                    onDoubleClick {
                        val x = selectionModel.selectedItems
                        if (x[0].value.id > 0) {
                            tabbedModels.loadModelFromDB(x[0].value.id)
                        }
                    }

                    populate { parent ->
                        if (parent == root)
                            sessionTree.rootNode.children
                        else
                            parent.value.children
                    }

                    subscribe<ModelAddedEvent> { event ->
                        sessionTree.addSession(event.model)
                        selectionModel.clearSelection()
                        selectSession(event.model.sessionName, event.model.packageName, root, selectionModel)
                    }

                    subscribe<PackageAddedEvent> { event ->
                        sessionTree.insertPackage(event.newPackage)
                        DBSaveMethods.instance.getPackageID(event.newPackage, expandable = true, editable = true)
                        selectionModel.clearSelection()
                        selectSession("", event.newPackage, root, selectionModel)
                    }

                }.setCellFactory {
                    GroupTreeCellImpl(tabbedModels, it)
                }

            }
        }

        subscribe<ModelDeletedEvent> { event ->
            sessionTree.removeSession(event.model.modelID)
        }
        subscribe<ModelDeletedEventID> { event ->
            sessionTree.removeSession(event.modelID)
        }

        subscribe<PackageRemovedEvent> {
            event -> sessionTree.removePackage(event.packageName)
        }

    }

    private fun allSessions(node: Group = sessionTree.rootNode): Set<Pair<String, String>> {
        val sessionMames = mutableSetOf<Pair<String, String>>()
        node.children.forEach {
            if (it.id != 0) {
                sessionMames.add(Pair(it.thisPackage, it.name))
            } else {
                sessionMames.addAll(allSessions(it))
            }
        }
        return sessionMames
    }


    private fun allPackages(node: Group): Set<String> {
        val ret = mutableSetOf<String>()
        if (node.id == 0) {
            ret.add(node.thisPackage.plus(".").plus(node.name))
            node.children.forEach {
                ret.addAll(allPackages(it))
            }
        }
        return ret
    }


    fun updateModelID(oldValue: Int, newValue: Int) {
        val group = findGroupWithID(modelID = oldValue)
        if (group != null) {
            group.id = newValue
        }
    }

    private fun findGroupWithID(modelID: Int, top: Group = sessionTree.rootNode): Group? {
        top.children.forEach {
            if (it.id == modelID) {
                return it
            } else if (it.id == 0) {
                val ret = findGroupWithID(modelID, it)
                if (ret != null)
                    return ret
            }
        }
        return null
    }


    fun validateNewSessionName(currentPackage: String, newPackage: String, newSessionName: String): String {

        if (newSessionName.contains("."))
            return ("Session name can not contains a dot")

        val pckg = currentPackage + if (newPackage == "") "" else "."
        return if (allSessions().contains(
                Pair(
                    "$pckg$newPackage",
                    newSessionName
                )
            )
        ) "Session name already exists" else ""
    }


    fun validateNewPackageName(newPackage: String): String {
        if (allPackages(sessionTree.rootNode).contains(newPackage)) {
            return "Package $newPackage already exists"
        }
        return ""
    }

}