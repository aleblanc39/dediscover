package org.woodhill.ded.ui.view.workspace

import javafx.scene.control.MultipleSelectionModel
import javafx.scene.control.SelectionMode
import javafx.scene.control.TreeItem
import javafx.scene.layout.HBox
import org.woodhill.ded.application.Styles
import org.woodhill.ded.models.KModel
import org.woodhill.ded.ui.view.*
import org.woodhill.ded.util.DBLoadMethods
import org.woodhill.ded.util.DBSaveMethods
import tornadofx.*


private fun selectSession(
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
    private val sessionTree = SessionTree(DBLoadMethods.instance.getAllPackages(), DBLoadMethods.instance.getAllModelsWithPackage())

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