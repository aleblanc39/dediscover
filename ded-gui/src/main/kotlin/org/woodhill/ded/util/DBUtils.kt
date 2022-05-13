package org.woodhill.ded.util

import org.sqlite.SQLiteConfig
import java.io.File
import java.sql.Connection
import java.sql.DriverManager
import java.util.*

/**
 * Handles the DB Connection for the DEDiscover database file. Defaults to
 *  $HOME/.dediscover_2/ded-models.db.
 *
 *  Default can be overridden with environment variable DEDISCOVER_DB_FILE
 */

private val DED_CONFIG_DIR = ".dediscover_2"
private val DED_SESSION_FILE = "ded-models.db"
private val dedDir = File(System.getProperty("user.home"), DED_CONFIG_DIR)
private val dbFile = File(dedDir, DED_SESSION_FILE)
private val DEFAULT_DB_URL =  dbFile.absolutePath

private val DB_URL_ENV_VAR = "DEDISCOVER_DB_FILE"
class DBConnection() {
    val DB_URL = "jdbc:sqlite:" + if (System.getenv()[DB_URL_ENV_VAR] != null) System.getenv()[DB_URL_ENV_VAR] else DEFAULT_DB_URL

    fun  getConnection(): Connection {
        val connectionProperties: Properties
        val config = SQLiteConfig()
        config.enforceForeignKeys(true)
        connectionProperties = config.toProperties()

        return DriverManager.getConnection( DB_URL, connectionProperties)
    }
}



