package org.woodhill.ded.sbml

class SymbolsHandler {
    var counter = 0
    val allSymbols = mutableSetOf<String>()

    fun addSymbol(s: String) {
        allSymbols.add(s)
    }

    fun addOrCreateUnique(s: String): String {
        if (!allSymbols.contains(s)) {
            allSymbols.add(s)
            return s
        }
        return createUnique(s)
    }

    fun createUnique(s: String): String {
        while (true) {
            val ret = s + "_" + counter++
            if (!allSymbols.contains(ret)) {
                allSymbols.add(ret)
                return ret
            }
        }
    }
}
