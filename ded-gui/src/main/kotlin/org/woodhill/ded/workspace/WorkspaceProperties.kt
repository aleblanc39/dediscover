package org.woodhill.ded.workspace

import javafx.beans.property.SimpleBooleanProperty
import org.woodhill.ded.session.SessionViewModel
import tornadofx.getValue
import tornadofx.setValue

/**
 * Purpose of this class is to define/update all the properties used in the GUI to enable/disable global
 * menu items and buttons
 */

class WorkspaceProperties {
    companion object {

        val hasEstimationResultProperty = SimpleBooleanProperty(false)
        var hasEstimationResult by hasEstimationResultProperty

        val hasSimulationResultProperty = SimpleBooleanProperty(false)
        var hasSimulationResult by hasSimulationResultProperty

        val hasOpenSessionProperty = SimpleBooleanProperty(false)
        var openSession by hasOpenSessionProperty

        val saveableSessionProperty = SimpleBooleanProperty(false)
        var saveableSession by saveableSessionProperty

        val deletableSessionProperty = SimpleBooleanProperty(false)
        var deletableSession by deletableSessionProperty



        fun updateMenuProperties(sess:SessionViewModel?)  {
            if (sess == null) {
                hasEstimationResult = false
                hasSimulationResult = false
                openSession = false
                saveableSession = false
                deletableSession = false
            } else {
                hasEstimationResult = sess.item.estimationResults.estimatedRows.size > 0
                hasSimulationResult = sess.item.solverResults.isNotEmpty()
                saveableSession = sess.item.model.editable
                deletableSession = sess.item.model.editable
                openSession = true

            }
        }
    }
}