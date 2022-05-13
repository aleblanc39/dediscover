package org.woodhill.ded.util

import org.woodhill.ded.models.KParameterSet
import tornadofx.isInt
import java.io.StringReader
import java.util.*
import java.util.function.Consumer
import javax.json.Json
import javax.json.JsonObjectBuilder

/**
 * Split line based on commas, but don't split if comma inside double quote
 *
 * @param input
 * @return
 */
fun splitOnCommas(input: String): List<String>? {
    val result: MutableList<String> = LinkedList()
    var start = 0
    var inQuotes = false
    for (current in 0 until input.length) {
        if (input[current] == '\"') inQuotes = !inQuotes // toggle state
        if (input[current] == ',' && !inQuotes) {
            result.add(input.substring(start, current))
            start = current + 1
        }
        val atLastChar = current == input.length - 1
        if (atLastChar) {
            if (input[current] == ',') {
                result.add("")
            } else {
                result.add(input.substring(start))
            }
        }
    }
    return result
}

fun doubleArrayFromSpaceSeparated(input: String):DoubleArray {
    val ret = input.trim().split(" ").map { it.toDoubleOrNull() }

    return DoubleArray(ret.size) {
        i->
        ret[i]!!
    }
}

fun stringPairsToJson(params: Iterable<Pair<String, String>>): String {
    val jsonObject = Json.createObjectBuilder()
    params.forEach{
        jsonObject.add(it.first, it.second)
    }
    return jsonObject.build().toString()
}

/**
 * TODO Reusing method in DBLoadMethods. Won't need both. Need to consolidate
 */
fun jsonControlParamsToMap(controlParams: String): Map<String, String> {
    return parseControlParameters(controlParams)
}

fun parseControlParameters(controlParams: String?): Map<String, String> {
    val params = mutableMapOf<String, String>()
    if (controlParams == null || controlParams.isEmpty()) {
        return params
    }

    val jsonReader = Json.createReader(StringReader(controlParams))
    val jsonObj = jsonReader.readObject()

    jsonObj.keys.forEach(Consumer { n: String -> params.put(n, jsonObj.getString(n)) })
    return params
}


class DEDUtils {

    companion object {
            fun generateNewName(prefix: String, currentNames: Set<String>): String {
                val highestIndex = currentNames.filter {
                    it.startsWith(prefix) &&
                            it.substring(prefix.length).isInt()
                }.map { it.substring(prefix.length).toInt() }.sorted().lastOrNull()
                return if (highestIndex == null) prefix + 0 else prefix + (highestIndex + 1)
            }

            fun generateNewNames(nbToGenerate: Int, prefix: String, currentName: Set<String> = emptySet() ):List<String> {
                val names = mutableListOf<String>()
                repeat(nbToGenerate) {
                    names.add(generateNewName(prefix, names.toSet()))
                }
                return names
            }


            /**
             * Types S and T must have a name field. Will remove from original the ones not found in newParams.
             * This is used after parsing a model, we want to preserve the information from the old one, update
             * occurence index.
             *
             * fromStorage contains the symbols from the last saved model. Used so we don't create now objects for the case
             * where in a saved model we delete a symbol, parse, the put the symbol back.
             *
             * Expected workflow:
             *
             */
            fun <S, T> synchronizeNewModelSymbols(current:MutableList<S>, newParams:List<T>, fromStorage:List<S>, originalName:(S)->String, newName:(T)->String, copyOccurence:(S, T)->Unit, newObject:(T)->S ) {

                val newNames = newParams.map {newName(it)}
                fromStorage.forEach{
                    if (!current.contains(it))
                        current.add(it)
                }
                current.removeIf { !newNames.contains(originalName(it)) }


                current.forEach{ orig ->
                    val newRec = newParams.find { newName(it) == originalName(orig) }
                    if (newRec != null) {
                        println("Copyin occurence for $newRec")
                        copyOccurence(orig, newRec)
                    }
                }

                val oldNames:List<String> = current.map{originalName(it) }
                newParams.filter{!oldNames.contains(newName(it))}.forEach {
                    current.add(newObject(it))
                }
            }



        fun paramSetAsJsonBuilder(paramSet: KParameterSet): JsonObjectBuilder {

            val jsonObject = Json.createObjectBuilder()
            paramSet.allParameterValues().forEach {param->
                    jsonObject.add(param.name, param.defaultValue)
            }
            return jsonObject
        }

        fun controlParamsAsJsonBuilder(controlParams: Map<String, String>): JsonObjectBuilder {
            val jsonObject = Json.createObjectBuilder()
            controlParams.forEach {
                jsonObject.add(it.key, it.value)
            }
            return jsonObject
        }
    }
}
