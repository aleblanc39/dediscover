package org.woodhill.ded.sbml

import org.sbml.jsbml.*
import org.woodhill.ded.sbml.SBMLUtil.getCellDesignerClass
import java.io.File
import java.util.regex.Matcher
import java.util.regex.Pattern


class SBMLFileParser(filename: String) {
    private val sbmlDoc: SBMLDocument = SBMLReader.read(File(filename).readText(Charsets.UTF_8))
    private val symbolsHandler = correctVariableNames(sbmlDoc)
    private val equationsMap = sbmlDoc.model.listOfSpecies.map { Pair(it.id, DiffEqnBuilder(it, symbolsHandler)) }.toMap()

    fun createEquations(): String {
        val equations =
            StringBuffer(
                """
                // Equations translated from SBML session name: ${sbmlDoc.model.name}
                
                """.trimIndent()
            )

        val model = sbmlDoc.model
        for (r in model.listOfReactions) {
            for (s in r.listOfProducts) {
                if (s != null) {
                    equationsMap[s.species]!!.addReactionProduct(r)
                    equationsMap[s.species]!!.addReactionModifier(r)
                }
            }
            for (s in r.listOfReactants) {
                if (s != null) {
                    equationsMap[s.species]!!.addReactionReactant(r)
                }
            }
        }

        // Add the differential equations. Will filter out those for which the
        // protein type is "UNKNOWN"

        // Add the differential equations. Will filter out those for which the
        // protein type is "UNKNOWN"

        if (equationsMap.values == null) {
            System.err.println("Warning: no equations defined.")
        } else {
            for (eqn in equationsMap.values) {
                if (getCellDesignerClass(eqn.species) == "UNKNOWN") {
                    System.err.println("Skipping over " + eqn.species.name)
                } else if (eqn.equationTerm.isNotEmpty()) {
                    System.err.println(eqn.diffEquation)
                    equations.append(eqn.diffEquation + "\n")
                } else {
                    System.err.println("No equation for " + eqn.species.name)
                }
            }
        }
        equations.append(getMacros())
        return updateNamesInEquations(equations.toString())
    }

    private fun getMacros(): String {
        val macros = StringBuffer()
        for (r in sbmlDoc.model.listOfRules) {
            if (r.isAlgebraic) {
                throw  RuntimeException("Unhandled type of rule");
            } else if (r.isAssignment) {
                val ass = r as AssignmentRule
                macros.append(
                    ass.getVariable() + " = " + SBMLUtil.convertEquationToMUParser(ass.math.toString())
                            + "\n"
                );
            } else {
                println("Some other rule!!!");
                throw  RuntimeException("Unhandled type of rule");
            }
        }
        return macros.toString();
    }

    private fun updateNamesInEquations(equations: String): String {
        val species = sbmlDoc.model.listOfSpecies
        var returnedEqns = equations
        for (sp in species) {
            val key = sp.id
            val nameUsed = getSpeciesName(sp)
            // Given pair key-value, want to replace $1 in all
            // [^alnum]+([alnum]+)[^alnum]+|"\n" with value
            // The fact that we require at least one non-alnum before the match
            // will prevent var definitions (macro or deriv) from
            // being transformed.
            val patternToMatch = "(\\W+)($key)([\\W+|\\n])"
            val pattern: Pattern = Pattern.compile(patternToMatch)
            val m: Matcher = pattern.matcher(returnedEqns)
            if (m.find()) {
                returnedEqns = m.replaceAll("$1$nameUsed$3")
            }
        }
        return returnedEqns
    }

    fun getModelParamValues():Map<String, Double> {
        val values = mutableMapOf<String, Double>()
        /** Local parameters and global parameters are very different concepts in SBML,
         * without a common interface that has both name and value. But both translate to
         * model parameters in DEDiscover.
         */

        sbmlDoc.model.listOfParameters.forEach {
            println("Adding model param value: ${it.name} -- ${it.value} -- ${it.id}")
            values[it.id] = it.value
        }

        sbmlDoc.model.listOfCompartments.forEach {
            println("COMPARTMENT: ${it.name} -- ${it.id} -- ${it.value}")
            values[it.id] = it.value

        }
        for (r in sbmlDoc.model.listOfReactions) {
            for (p in r.kineticLaw.listOfLocalParameters) {
                println("In local parameter: ${p.name} -- ${p.value} -- ${p.id}")
                val name: String = if (p.name != null && p.name !== "") p.name else p.id
                values[name] = p.value
            }
        }

        return values
    }

    private fun getValue(s:Species):Double {
        val v =  if (s.initialAmount == Double.NaN) s.initialConcentration else s.initialAmount
        return if (v == Double.NaN) 0.0 else v
    }

    fun getInitialConditionValues(): Map<String, Double> {
        println("List of species")
        sbmlDoc.model.listOfSpecies.forEach {
            println("${it.name} -- ${it.id} -- ${it.value} -- ${it.initialAmount} -- ${it.initialConcentration}")
        }
        return sbmlDoc.model.listOfSpecies.map{Pair(it.name, getValue(it))}.toMap()
    }
}

