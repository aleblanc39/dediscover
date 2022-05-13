package org.woodhill.ded.workspace

import javafx.stage.FileChooser
import org.woodhill.ded.models.KModel
import org.woodhill.ded.sbml.SBMLFileParser
import org.woodhill.ded.sbml.XDESessionBuilder
import org.woodhill.ded.workspace.popups.NewPackageUserQuery
import org.woodhill.ded.workspace.popups.NewSessionBaseValues
import org.woodhill.ded.workspace.popups.NewSessionUserQuery
import tornadofx.*
import java.lang.Exception


data class NewSessionInfo(val pkg:String, val sessionName:String, val sessionTitle:String)

class WorkspaceUtil {
    companion object {
        fun getNewSessionName(
            initialSessionValues: NewSessionBaseValues,
            validateFcn: (String, String, String) -> String
        ): NewSessionInfo {
            return NewSessionUserQuery.getSessionName(initialSessionValues, validateFcn)

        }

        fun getNewPackageName(currentPackage: String, validateFcn:(String)->String): String {
            return NewPackageUserQuery.getNewPackageName(currentPackage, validateFcn)
        }

        fun importSBML(
            topPackage: String,
            intermediatePackage: String = "",
            validateFcn: (String, String, String) -> String
        ): KModel? {
            val filenames = chooseFile(
                "Data File to Import",
                //owner = currentWindow,
                filters = arrayOf(FileChooser.ExtensionFilter("XML files", "*.xml", "*.smbl"), FileChooser.ExtensionFilter("All Files", "*.*")),
                mode = FileChooserMode.Single
            )
            if (filenames.isNotEmpty()) {
                try {
                    val parsedFile = SBMLFileParser(filenames[0].path)
                    val xdeBuilder = XDESessionBuilder(
                        sbmlParser = parsedFile,
                        packageName = "Sessions.User Sessions.sbml",
                        sessionName = "sbmlSession-2",
                        sessionTitle = "SBML Title"
                    )

                    val packageAndSession =
                        getNewSessionName(
                            NewSessionBaseValues(
                                topPackage = topPackage,
                                intermediatePackage = intermediatePackage
                            )
                        ) { a, b, c ->
                            validateFcn(a, b, c)
                        }

                    return if (packageAndSession.sessionName.isNotEmpty()) {
                        xdeBuilder.kmodel.packageName = packageAndSession.pkg
                        xdeBuilder.kmodel.sessionName = packageAndSession.sessionName
                        xdeBuilder.kmodel.title = packageAndSession.sessionTitle
                        // TODO Won't display correctly if not saved. Needs redisign.
                        xdeBuilder.saveModel()
                        xdeBuilder.kmodel
                    } else {
                        null
                    }

                } catch (ex: Exception) {
                    error(header = "Error importing SBML", content = "Message: ${ex.message}")
                    return null
                }
            } else {
                return null
            }
        }
    }
}