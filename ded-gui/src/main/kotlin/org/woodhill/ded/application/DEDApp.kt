package org.woodhill.ded.application

import javafx.scene.control.Alert
import org.woodhill.ded.workspace.DEDWorkspace
import org.woodhill.ded.workspace.MainWindow
import tornadofx.*
import java.io.File
import java.sql.DriverManager
import kotlin.system.exitProcess


private const val DED_CONFIG_DIR = ".dediscover_2"
private const val DED_SESSION_FILE = "ded-models.db"
private val dedDir = File(System.getProperty("user.home"), DED_CONFIG_DIR)
private val dbFile = File(dedDir, DED_SESSION_FILE)
private val DB_URL = "jdbc:sqlite:" + dbFile.absolutePath

private fun createSessionFile() {
    try {
        val sqlURL = DEDApp::class.java.classLoader.getResource("ded.sql")
        val sql = File(sqlURL.path).readText()
        DriverManager.getConnection(DB_URL)

        DriverManager.getConnection(DB_URL).use { conn ->
            val stmt = conn.createStatement()
            stmt.executeUpdate(sql)
        }
    } catch (e: Exception) {
        alert(Alert.AlertType.INFORMATION, "Something went wrong when initializing the database: ${e.message}")
        exitProcess(0)
    }
}

private fun initializeSession() {
    try {

        if (dedDir.exists() && !dedDir.isDirectory) {
            alert(Alert.AlertType.INFORMATION, "A file named ${dedDir.absolutePath} exists " +
                    "and is not a directory. Exiting...")
            exitProcess(0)
        }

        if (dedDir.exists() && !dbFile.exists()) {
            confirm("The model file is missing from the directory." +
                    "Would you like to create a new one?", actionFn = { createSessionFile() })

            // Kotlin doesn't have a yes/no. If file still doesn't exit exit
            if (!dbFile.exists()) exitProcess(0)
        }

        if (!dedDir.exists()) dedDir.mkdir()

        if (!dedDir.isDirectory) {
            alert(Alert.AlertType.INFORMATION, "Error creating the user directory")
            exitProcess(0)
        }

        if (!dbFile.exists())
            createSessionFile()

    } catch (e: Exception) {
        val msg = "Can not initialize the session. This is the message received: ${e.message}"
        alert(Alert.AlertType.INFORMATION, msg)
        exitProcess(0)
    }
}


fun main(args: Array<String>) {
    launch<DEDApp>(args)
}

class DEDApp : App(DEDWorkspace::class) {

    override fun onBeforeShow(view: UIComponent) {
        workspace.dock<MainWindow>()
    }


    init {
        initializeSession()
        importStylesheet(Styles::class)
        try {
            System.loadLibrary("DEDiscover")
        } catch (ex: Throwable) {
            ex.printStackTrace()
        }
    }
}