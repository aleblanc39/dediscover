package org.woodhill.ded.sbml

import org.sbml.jsbml.*

private val DEDiscoverReservedWords = listOf("pi")


 fun getSpeciesName(species: Species): String? {
    return if (species.name != null && species.name.length > 0 && species.name != "unknown") {
        // Replace all non_alnum with underscores
        species.name.replace("\\W".toRegex(), "_")
    } else {
        species.id
    }
}

 fun correctVariableNames(sbmlDoc: SBMLDocument): SymbolsHandler {
    val model: Model = sbmlDoc.model
    val symbolsHandler = SymbolsHandler()

    fun rename(s: String): String? = if (DEDiscoverReservedWords.contains(s)) {
        symbolsHandler.addOrCreateUnique(s)
    } else s


    // Put all symbols in a list first, to reduce amount of renaming
    val species = model.listOfSpecies
    for (s in species) {
        // Making assumption that no two species can have the same name.
        // Should check
        // if valid.
        // allSymbols.add(getSpeciesName(s));
        getSpeciesName(s)?.let { symbolsHandler.addSymbol(it) }
    }
    val parameters: ListOf<Parameter> = model.listOfParameters
    for (p in parameters) {
        symbolsHandler.addSymbol(p.name)
    }

    // Macros are defined as rules in SBML
    val rules = model.listOfRules
    for (r in rules) {
        if (r.isAlgebraic) {
            throw RuntimeException("Can not handle algebraic rules yet.")
        } else if (r.isAssignment) {
            val ass = r as AssignmentRule
            // allSymbols.add(ass.getVariable());
            symbolsHandler.addSymbol(ass.variable)
        } else {
            System.err.println("Some other rule!!!")
            throw RuntimeException("Unhandled type of rule")
        }
    }

    // Go again through all the species/parameters/rules, rename variables
    // as necessage
    for (s in species) {
        s.name = getSpeciesName(s)?.let { rename(it) }
    }
    for (p in parameters) {
        p.setName(rename(p.getName()))
    }
    // Now we know we just have valid rules. Don't need to check.
    // Using only assignment rules for now
    for (r in rules) {
        (r as AssignmentRule).variable = rename(r.variable)
    }

    return symbolsHandler
}

