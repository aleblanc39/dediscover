package org.woodhill.ded.workspace

import javafx.scene.Node
import javafx.scene.control.Tab
import javafx.scene.control.TabPane
import javafx.scene.control.TitledPane
import javafx.scene.layout.VBox
import org.jfree.chart.util.ExportUtils
import org.woodhill.ded.application.Styles
import org.woodhill.ded.models.KModel
import org.woodhill.ded.session.SessionElements
import org.woodhill.ded.session.SessionViewModel
import org.woodhill.ded.ui.view.*
import org.woodhill.ded.ui.view.datatable.DataPane
import org.woodhill.ded.util.DBLoadMethods
import org.woodhill.ded.util.DBSaveMethods
import org.woodhill.ded.workspace.popups.CSVJsonExportQuery
import org.woodhill.ded.workspace.popups.ExportFilenames
import org.woodhill.ded.workspace.popups.ExportPlotDialog
import org.woodhill.ded.workspace.popups.getDatatableExportFileName
import tornadofx.*
import java.io.File

class TabbedModelsView : View() {

    val modelListView: ModelListView by inject()

    private val openedModels = mutableSetOf<KModel>()
    private val modelsMap = hashMapOf<Node, SessionViewModel>()
    private val currentTabs = mutableMapOf<KModel, Tab>()

    fun content() = root.selectionModel.selectedItem.content
    fun currentSession() = modelsMap[content()]

    override val root = tabpane() {
        prefWidth = 600.0
        addClass(Styles.tabbedModelsView)
        selectionModel.selectedItemProperty().onChange {
            updateMenuProperties()
        }

    }


    private fun removeSession(model: KModel) {
        openedModels.remove(model)
        currentTabs.remove(model)
        openedModels.remove(model)
        DBLoadMethods.instance.removeModelInfo(model)

    }

    private fun initializeLoadedModel(modelObj: KModel) {
        openedModels.add(modelObj)
        val scope: SessionScope = setScope(modelObj)
        val sessionViewModel = scope.model
        val newTab = WorkspaceTopView()
        scope.tabPane = newTab.root

        root.add(newTab)
        root.selectionModel.selectLast()
        root.selectionModel.selectedItem.text = sessionViewModel.item.model.sessionName
        root.tabs.last().setOnClosed {
            removeSession(modelObj)

        }

        currentTabs[modelObj] = root.tabs.last()
        modelsMap[root.selectionModel.selectedItem.content] = sessionViewModel
        updateMenuProperties()
    }

    fun addNewModel(newSessionParams: NewSessionInfo) {
        val newModel: KModel = KModel.createNewModel(
            newSessionParams.pkg,
            newSessionParams.sessionName,
            title = newSessionParams.sessionTitle
        )
        initializeLoadedModel(newModel)
        fire(ModelAddedEvent(newModel))
    }

    fun addPackage(newPackage: String) {
        fire(PackageAddedEvent(newPackage))
    }

    fun loadModelFromDB(modelID: Int) {
        val modelObj = openedModels.find { it.modelID == modelID }
        if (modelObj != null) {
            root.selectionModel.select(currentTabs[modelObj])
        } else {
            initializeLoadedModel(KModel.getKModel(modelID))
        }
    }

    class SessionScope : Scope() {
        val model = SessionViewModel()
        lateinit var tabPane: TabPane
    }

    fun setScope(modelObj: KModel): SessionScope {
        val editScope = SessionScope()
        val sessionElements = SessionElements(modelObj)
        editScope.model.item = sessionElements
        //find(ModelTabView::class, editScope)
        find(WorkspaceTopView::class, editScope)
        editScope.model.editScope = editScope
        //return editScope.model
        return editScope
    }


    fun saveCurrentSession() {
        performSave(modelsMap[content()]!!)
    }

    fun currentPackageName(): String {
        val session = currentSession()
        if (session != null)
            return session.item.model.packageName

        return ""
    }

    fun exportSimulationResultsDialog(): ExportFilenames {
        // TODO Lots of assumptions about non-nulls here. Make sure they can be satisfied.
        val content = content()!!
        return modelsMap[content]?.let { CSVJsonExportQuery.getSimulationResultFilenames(it.item) }!!
    }

    fun exportEstimationResultsDialog(): ExportFilenames =
        modelsMap[content()!!]?.let { CSVJsonExportQuery.getEstimationResultFilenames(it.item) }!!

    fun exportSimResultAsJson(parameterSet: String, filename: String) =
        currentSession()!!.item.exportJsonSimResults(parameterSet, filename)

    fun exportSimResultAsCSV(parameterSet: String, filename: String) =
        currentSession()!!.item.exportCSVSimResults(parameterSet, filename)

    fun exportEstimationResult(filename: String, asCSV: Boolean) =
        currentSession()!!.item.exportEstimationResult(filename, asCSV)

    fun exportPlots() {
        val JPG = "jpg"
        val PDF = "pdf" //
        val PNG = "png"
        val plotFileInfo = ExportPlotDialog.getExportFilenames(currentSession()!!.item, listOf(JPG, PNG))


        if (plotFileInfo.filename != "") {

            currentSession()?.item?.plots?.forEachIndexed { indx, plot ->
                if (plotFileInfo.fileType == JPG) {
                    ExportUtils.writeAsJPEG(plot, 1000, 500, File("${plotFileInfo.filename}-$indx.jpg"))
                }
                if (plotFileInfo.fileType == PDF) {
                    ExportUtils.writeAsPDF(plot, 1000, 500, File("${plotFileInfo.filename}-$indx.pdf"))
                }
                if (plotFileInfo.fileType == PNG) {
                    ExportUtils.writeAsPNG(plot, 1000, 500, File("${plotFileInfo.filename}-$indx.png"))
                }
            }
        }
    }



    fun importDatatable() {
        val datatab = (content() as TabPane).tabs[WorkspaceTopView.TABINDEX.DATATABLE.index].content as VBox
        // TODO Should this be moved to DatatableView? Do we need this level of details here?
        val datapane = datatab.children[0] as TitledPane
        datapane.uiComponent<DataPane>()?.importData()
    }

    fun exportDatatable() {
        val content = content()!!
        val filename =  modelsMap[content]?.let { getDatatableExportFileName(it.item) }!!
        if (filename.trim().isEmpty()) {
            return
        }
        currentSession()!!.item.exportDatable(filename)


    }

    fun selectTab(tab: WorkspaceTopView.TABINDEX) {
        (content() as TabPane).selectionModel.select(tab.index)
    }



    fun deleteCurrentSession() {
        println("Will delete session:")
        val content = root.selectionModel.selectedItem.content
        DBSaveMethods.instance.deleteSession(modelsMap[content]!!.item.model.modelID)
    }


    fun saveSession(modelID: Int) {
        val modelToSave = openedModels.find {
            it.modelID == modelID
        }
        val tabForModel = currentTabs[modelToSave]
        performSave(modelsMap[tabForModel?.content]!!)
    }


    private fun performSave(sessionViewModel: SessionViewModel) {
        val oldID = sessionViewModel.item.model.modelID
        sessionViewModel.saveSession()
        if (oldID < 0) {
            // TODO Use event here?
            modelListView.updateModelID(oldID, sessionViewModel.item.model.modelID)
        }
    }


    fun saveAllSessions() {

    }

    /**
     * Close a session if it is opened. Invoked from context menu.
     */
    fun closeSessionIfOpened(modelID: Int) {
        val model = openedModels.find { it.modelID == modelID } ?: return
        root.tabs.remove(currentTabs[model])
        removeSession(model)
    }

    fun renameSession(sessionID: Int, newPackage: String, newSessionName: String, sessionTitle: String) {

        DBSaveMethods.instance.updateSessionName(sessionID, newPackage, newSessionName, sessionTitle)
        modelListView.removeSession(sessionID)
        modelListView.addSession(sessionID, newPackage, newSessionName, true)

        val kmodel = KModel.getKModel(sessionID)
        if (openedModels.contains(kmodel)) {
            val modelTab = currentTabs[kmodel]
            val sessionView: SessionViewModel? = modelsMap[modelTab?.content]
            if (sessionView != null) {
                sessionView.item.model.packageName = newPackage
                sessionView.item.model.sessionName = newSessionName
                sessionView.item.model.title = sessionTitle
            }
            if (modelTab != null) {
                modelTab.text = newSessionName
            }
        }
    }


    private fun updateMenuProperties() {
        WorkspaceProperties.updateMenuProperties(if (root.selectionModel.isEmpty) null else currentSession())
    }

    init {
        subscribe<MenuAffectingEvent> {
            updateMenuProperties()
        }

        subscribe<SimulationDoneEvent> {
            print("Into simulation done event")
            selectTab(WorkspaceTopView.TABINDEX.PLOTS)
        }
    }
}