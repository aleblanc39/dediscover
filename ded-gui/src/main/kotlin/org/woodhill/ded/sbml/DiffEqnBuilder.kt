package org.woodhill.ded.sbml

import org.sbml.jsbml.Reaction
import org.sbml.jsbml.Species


class DiffEqnBuilder(val species: Species, private val symbolsHandler: SymbolsHandler) {
    private val productReactions = mutableListOf<Reaction>()
    private val reactantReactions = mutableListOf<Reaction>()
    private val modifierReactions = mutableListOf<Reaction>()
    private val varNameUsed: String = species.name

    fun addReactionProduct(r: Reaction) {
        productReactions.add(r)
    }

    fun addReactionReactant(r: Reaction) {
        reactantReactions.add(r)
    }

    fun addReactionModifier(r: Reaction) {
        modifierReactions.add(r)
    }

    val diffEquation: String
        get() = "$variableTerm = $equationTerm"
    val variableTerm: String
        get() = "d$varNameUsed/dt"// Quick fix// We know that these are all reactions involving this species.
    //referencesSpeciesNames.add(species.getName());

    // Add all the species attached to reaction that did not contain math.
//    System.err.println("Adding reactant: ");
    //    System.err.println(s.getSpeciesInstance().getId());
    //    System.err.println(s.getSpeciesInstance().getName());

    // All terms for which this reaction is a reactant. Don't care about
    // product here,
    // just subtract reactant, which is likely the same species as the
    // object itself.
// First term. No need for parenthesis// Not first term.// String reaction = getReactantMath(r, r.getListOfReactants());// All reactions for which this object is a product. Want to add the
    // reactant to term
    /**
     * Construct the RHS of the differential equation.
     * @return
     */
    val equationTerm: String
        get() {
            val ret = StringBuffer()
            val referencesSpeciesNames: MutableSet<String> = HashSet()
            if (productReactions.size == 0 && reactantReactions.size == 0) return ""
            var keepPlusSign = false

            // All reactions for which this object is a product. Want to add the
            // reactant to term
            for (r in productReactions) {
                // String reaction = getReactantMath(r, r.getListOfReactants());
                if (r.kineticLaw != null) {
                    if (keepPlusSign) {
                        // Not first term.
                        ret.append(" + ")
                        ret.append(r.kineticLaw.math)
                    } else {
                        // First term. No need for parenthesis
                        keepPlusSign = true
                        ret.append(r.kineticLaw.math)
                    }
                } else {
                    for (s in r.listOfModifiers) {
                        referencesSpeciesNames.add(s.name)
                    }
                    for (s in r.listOfReactants) {
                        //    System.err.println("Adding reactant: ");
                        //    System.err.println(s.getSpeciesInstance().getId());
                        //    System.err.println(s.getSpeciesInstance().getName());
                        referencesSpeciesNames.add(s.speciesInstance.name)
                    }
                }
            }

            // All terms for which this reaction is a reactant. Don't care about
            // product here,
            // just subtract reactant, which is likely the same species as the
            // object itself.
            for (r in reactantReactions) {
                if (r.kineticLaw != null) {
                    ret.append(" - (" + r.kineticLaw.math + ")")
                } else {
                    // We know that these are all reactions involving this species.
                    //referencesSpeciesNames.add(species.getName());
                }
            }
            if (ret.length == 0) {
                ret.append(symbolsHandler.createUnique("__alpha")).append(" * ").append(species.name)
            } else {
                referencesSpeciesNames.remove(species.name)
            }

            // Add all the species attached to reaction that did not contain math.
            for (s in referencesSpeciesNames) {
                // Quick fix
                if (s.trim { it <= ' ' } !== "") ret.append(" + ").append(symbolsHandler.createUnique("__alpha"))
                    .append(" * ").append(s)
            }
            return SBMLUtil.convertEquationToMUParser(ret.toString())
        }

    init {
        // Assumes that correctVariableNames has already been called.
    }
}
