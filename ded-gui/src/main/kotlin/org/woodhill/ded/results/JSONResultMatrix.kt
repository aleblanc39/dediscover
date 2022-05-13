package org.woodhill.ded.results

import org.woodhill.ded.util.doubleArrayFromSpaceSeparated
import java.io.StringReader
import javax.json.Json

/*
 Class to handle any type of results where we have timepoints, a set of variables with the results for each timepoint,
 and conversion to/from json string.

 Keep the data as a list of String and a double matrix.

 Class should be immutable.
 */

class JSONResultMatrix private constructor(val timepoints: List<Double>, val jsonData:String,  val matrixData: Pair<List<String>, List<DoubleArray>>) {

   constructor(timepoints: List<Double>, jsonData:String): this(timepoints, jsonData, extractJsonData(jsonData)) {
    }
}


private fun extractJsonData(jsonData:String): Pair<List<String>, List<DoubleArray>> {
    val returnedData = mutableListOf<DoubleArray>()
    val varNames = mutableListOf<String>()

    val jsonReader = Json.createReader(StringReader(jsonData))
    val jsonObj = jsonReader.readObject()

    for (k in jsonObj.keys) {
        varNames.add(k)
        returnedData.add(doubleArrayFromSpaceSeparated(jsonObj.getString(k)))
    }
    return Pair(varNames, returnedData)
}
