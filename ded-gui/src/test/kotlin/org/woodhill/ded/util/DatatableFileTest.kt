package org.woodhill.ded.util

import org.junit.Test

import org.junit.Assert.*

class DatatableFileTest {

    @Test
    fun getColumns() {
        val filepath = "src/test/resources/datatable.csv"
        val datatableFile = DatatableFile(filepath)
        println(datatableFile.columns)

    }

    @Test
    fun datatableNoHeaders() {
        val filepath = "src/test/resources/datatable-no-headers.csv"
        val datatableFile = DatatableFile(filepath)
        println(datatableFile.columns)
    }

    @Test
    fun datatableWithComments() {
        val filepath = "src/test/resources/datatable-with-comments.csv"
        val datatableFile = DatatableFile(filepath)
        println(datatableFile.columns)
    }
}