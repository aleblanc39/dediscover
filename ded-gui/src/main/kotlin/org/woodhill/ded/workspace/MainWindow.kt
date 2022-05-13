package org.woodhill.ded.workspace

import javafx.scene.control.Alert
import javafx.scene.control.ButtonType
import javafx.scene.layout.Region
import org.woodhill.ded.application.Styles
import org.woodhill.ded.ui.view.ModelDeletedEvent
import org.woodhill.ded.util.DBSaveMethods
import org.woodhill.ded.workspace.popups.NewSessionBaseValues
import tornadofx.*

class MainWindow : View("DEDiscover") {
    private val tabbedModels: TabbedModelsView by inject()
    private val modelListView: ModelListView by inject()


    override val root = borderpane {
        left<ModelListView>()
        center<TabbedModelsView>()
        bottom<BottomView>()

    }

    override fun onDelete() {
        super.onDelete()
        val session = tabbedModels.currentSession()
        if (session != null) {
            val modelID = session.item.model.modelID

            val alert = Alert(
                Alert.AlertType.CONFIRMATION,
                "Are you sure you want to delete session ${session.item.model.sessionName}",
                ButtonType.YES, ButtonType.NO
            )

            alert.title = "Delete Confirmation"
            alert.headerText = "Delete confirmation"
            alert.dialogPane.minHeight = Region.USE_PREF_SIZE
            alert.showAndWait()
            println("Result from alert: ${alert.result}")

            if (alert.result == ButtonType.YES) {
                tabbedModels.closeSessionIfOpened(modelID)
                if (modelID > 0) {
                    DBSaveMethods.instance.deleteSession(modelID)
                }
                fire(ModelDeletedEvent(session.item.model))
            }
        }
    }

    override fun onSave() {
        super.onSave()
        tabbedModels.saveCurrentSession()
    }


    override fun onCreate() {
        super.onCreate()
        // TODO Same code also used from menu. Consolidate.
        val packageAndSession = WorkspaceUtil.getNewSessionName(NewSessionBaseValues(topPackage = modelListView.topUserPackage)){ a, b, c -> modelListView.validateNewSessionName(a,b, c)}
        if (packageAndSession.sessionName.trim().isNotEmpty()) {
            tabbedModels.addNewModel(packageAndSession)
        }
    }

    init {
        root.addClass(Styles.mainWindow)

    }

}