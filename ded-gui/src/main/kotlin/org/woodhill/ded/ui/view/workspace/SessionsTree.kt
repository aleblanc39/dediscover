package org.woodhill.ded.ui.view.workspace

import javafx.beans.property.SimpleBooleanProperty
import javafx.collections.ObservableList
import org.woodhill.ded.models.KModel
import org.woodhill.ded.models.ModelPackage
import org.woodhill.ded.util.DBModel
import tornadofx.observableListOf

/**
 * Will read the models from the SQLite database and return them in form that
 * simulates a file tree, even if there is no such tree in db. Each model has a name and a package
 * that consists of dot ('.') separated tokens.
 */


// Group can hold both packages and models. An id==0 means this node is a package, positive will be a saved session
// negative a session that has never been saved.
data class Group(val name: String, val thisPackage:String = "", val children: ObservableList<Group> = observableListOf(), var id: Int,
                 val expandable:SimpleBooleanProperty, val editable:SimpleBooleanProperty) {
    fun fullName() = "$thisPackage.$name"
}

class SessionTree {

    val rootNode: Group

    fun addSession(modelID: Int, packageName: String, sessionName: String, editable:Boolean) {
        insertPackage(packageName, rootNode, expandable = true, editable = true)
        addModels(listOf(DBModel(modelID, sessionName, packageName, editable)), rootNode)
    }

    fun addSession(model: KModel) = addSession(model.modelID, model.packageName, model.sessionName, model.editable)
    fun insertPackage(name:String) {
        insertPackage(name, rootNode, expandable = true, editable = true)
    }

    private fun insertPackage(name: String, topGroup: Group, expandable:Boolean, editable:Boolean) {
        if (name == topGroup.name) {
            topGroup.editable.value = editable
            topGroup.expandable.value = expandable
        } else {
            val parts = name.split(".")
            if (parts[0] != topGroup.name)
                throw Exception("Bad package structure: ")
            var child = topGroup.children.find { g -> g.name == parts[1] }
            if (child == null) {
                child = Group(name = parts[1], thisPackage = topGroup.thisPackage.plus(if (topGroup.thisPackage.equals("")) "" else '.').plus(parts[0]), id = 0,
                        editable = SimpleBooleanProperty(true),
                        expandable = SimpleBooleanProperty(true))
                topGroup.children.add(child)

            }
            insertPackage(parts.subList(1,parts.size).joinToString(separator = "."), child, expandable, editable)
        }
    }

    /**
     * At this point the hierarchy structure (packages) is created. Need to add models.
     */
    private fun addModels(models: List<DBModel>, topGroup: Group) {
        for (model in models) {
            var thisGroup = topGroup
            for (x in model.pkg.split('.')) {
                val child = thisGroup.children.find { g -> g.name == x }
                if (child != null) {
                    thisGroup = child
                }
            }
            thisGroup.children.add(
                Group(model.name, model.pkg, id = model.modelID,
                    expandable = SimpleBooleanProperty(false), editable = SimpleBooleanProperty(model.editable))
            )
        }
    }


    fun removeSession(modelID: Int, currentPackage: Group = rootNode): Boolean {

        //TODO This is called often. Why???
        println("Remove session for model $modelID")
        val node = currentPackage.children.find { it.id == modelID }
        if (node != null) {
            currentPackage.children.remove(node)
            return true
        }
        for (x in currentPackage.children.filter {it.id == 0 }) {
            val deleted = removeSession(modelID, x)
            if (deleted) {
                return true
            }
        }
        return false
    }


    fun removePackage(packageName: String) {
        val parentPackage = findPackageWithName(packageName.substringBeforeLast('.'))
        parentPackage?.children?.removeIf {
            it.id == 0 && it.fullName() == packageName
        }
    }

    /**
     * TODO
     */
    private  fun getSessionsFromDB(allPackages: List<ModelPackage>, allModels: List<DBModel>): Group {

        val rootPackage = allPackages.find {
            !it.name.contains('.')
        } ?: throw Exception("Bad package structure in database -- no root found")

        val returnedGroup = Group(name = rootPackage.name, id = 0, expandable = SimpleBooleanProperty(false), editable = SimpleBooleanProperty(false))
        allPackages.forEach { insertPackage(it.name, returnedGroup, expandable = it.expandable, editable = it.editable) }
        addModels(allModels, returnedGroup)

        return returnedGroup // assigned to sessions
    }

    /**
     * Returns the highest package that can be expanded by a user.
     * Execute a BFS on the tree
     */
    fun topUserEditablePackage():String {
        val candidates = mutableListOf(rootNode)

        while (candidates.size > 0) {
            val sess = candidates.removeAt(0) // Use removeFirst when moving to 1.4
            for (x in sess.children) {
                if (x.expandable.value) {
                    return x.thisPackage.plus(".").plus(x.name)
                }
                candidates.add(x)
            }
        }

        // Actually should throw exception
        return "NoSuch Package"
    }


    private fun findPackageWithName(fullPkgName: String, node: Group = rootNode): Group? {
        if (fullPkgName == node.fullName()) {
            return node
        }
        node.children.filter { it.id == 0 }.forEach {
            val n = findPackageWithName(fullPkgName, it)
            if (n != null)
                return n
        }
        return null
    }


    /**
     * Returns a pair with a set of all session IDs and a set of all packages under the package
     * name given as input. It may not exist, in which case it returns two empty sets.
     */
    fun componentsUnderPackage(pkgName:String): Pair<Set<Int>, Set<String>> {
        val node = findPackageWithName(pkgName) ?: return Pair(setOf(), setOf())
        return componentsUnderPackage(node)
    }

    private fun componentsUnderPackage(node: Group): Pair<Set<Int>, Set<String>> {
        val ret = Pair(mutableSetOf<Int>(), mutableSetOf(node.fullName()))

        node.children.forEach{
            if (it.id != 0) { // Have a session
                ret.first.add(it.id)
            } else {
                val fromChild = componentsUnderPackage(it)
                ret.first.addAll(fromChild.first)
                ret.second.addAll(fromChild.second)
            }
        }
        return ret
    }

    constructor (allPackages: List<ModelPackage>, allModels: List<DBModel>) {
        this.rootNode = getSessionsFromDB(allPackages, allModels)
    }
}