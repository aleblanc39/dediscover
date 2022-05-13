package org.woodhill.ded.util

import org.woodhill.ded.models.*
import org.woodhill.ded.properties.GlobalProperties
import org.woodhill.ded.properties.KSimTabProperties
import org.woodhill.ded.properties.KTabProperties
import org.woodhill.ded.results.EstimationCIResults

/**
 * Collection of methods to duplicate a session. Lots in common with SessionElements.save(). Should extract
 * common functionality.
 */

fun duplicateSession(sessionID: Int, newSessionName: String, packageName:String, newSessionTitle:String):KModel {
    val fromSession = KModel.getKModel(sessionID)
    val toSession = KModel(if (newSessionTitle=="") fromSession.title else newSessionTitle, newSessionName, packageName, fromSession.equations)
    toSession.editable = true
    toSession.save()

    duplicateSymbols(fromSession, toSession)
    copyProperties(fromSession, toSession)
    duplicateMapping(fromSession, toSession)
    return toSession
}


private fun duplicateMapping(fromSession:KModel, newSession:KModel) {
    val datatable = KDatatable(KMappingInfoSet.getMappingInfoSet(fromSession).cloneForModel(newSession))
    DBSaveMethods.instance.saveDatatable(datatable)
}



val estimableSymbolMap = mutableMapOf<EstimableSymbol, EstimableSymbol>()

private fun copyProperties(fromSession: KModel, toSession:KModel) {
    val fromGlobal = KTabProperties.loadTabProperties(fromSession, GlobalProperties::class)
    val toGlobal = GlobalProperties(toSession)
    fromGlobal.copyPropertiesTo(toGlobal)
    toGlobal.saveCurrentMap()

    val toSimtab = KSimTabProperties(toSession)
    KTabProperties.loadTabProperties(fromSession, KSimTabProperties::class).copyPropertiesTo(toSimtab)
    toSimtab.saveCurrentMap()

}

private fun copyEstimableSymbol(es:EstimableSymbol, newSession:KModel): EstimableSymbol {

    estimableSymbolMap[es] = EstimableSymbol(-1, newSession, es.name, es.occurenceIndex, es.description)
    return estimableSymbolMap[es]!!
}


private fun duplicateSymbols(fromSession:KModel, newSession:KModel) {

    DBSaveMethods.instance.saveModelParameters(newSession,
        EstimableSymbol.loadModelParameters(fromSession).map {
            copyEstimableSymbol(it, newSession)
        }
    )
    DBSaveMethods.instance.saveDependentVariables(newSession,
        EstimableSymbol.loadInitialConditions(fromSession).map {copyEstimableSymbol(it, newSession)!!})

    println("Saving the macros in duplicate symbols")
    KMacro.getMacros(fromSession).forEach {
        println("saving new macro for $it")
        DBSaveMethods.instance.saveMacro(KMacro(newSession, it.name, it.description, it.occurenceIndex))
    }
    KCovariate.getCovariates(fromSession).forEach {
        DBSaveMethods.instance.saveCovariate(KCovariate(newSession, it.name, it.timeExpression, it.description, it.occurenceIndex))
    }

    KParameterSet.parameterSetsForModel(fromSession).forEach {
        duplicateParameterSet(it.value, newSession, estimableSymbolMap)
    }
}

/**
 * Duplicate a parameter set.
 */
fun duplicateParameterSet(oldParameterSet: KParameterSet, newModel:KModel, estimableSymbolMap:Map<EstimableSymbol, EstimableSymbol>) {
    val newParameterSet = KParameterSet(oldParameterSet.name,
        newModel,
        modelParams = EstimableSymbol.loadModelParameters(newModel),
        initialConditions = EstimableSymbol.loadInitialConditions(newModel))

    DBSaveMethods.instance.saveParameterSet(newParameterSet)

    val newEstimationCIResults = duplicateEstimationResult(oldParameterSet, newParameterSet)
    DBSaveMethods.instance.saveEstimationResult(newEstimationCIResults)

    val initCondValues = oldParameterSet.initialConditionsValues.map{it.duplicate(newParameterSet, estimableSymbolMap[it.modelSymbol]!!)}
    val modelParamValues = oldParameterSet.modelParameterValues.map{it.duplicate(newParameterSet, estimableSymbolMap[it.modelSymbol]!!)}
    newParameterSet.replaceValues(newInitialConditionValues = initCondValues, newModelParameterValues = modelParamValues)

    // TODO Try to improve this. ParameterSet is saved twice. First the parameter set itself so it exists,
    // then again fo the init conds, model params values are saved along.
    DBSaveMethods.instance.saveParameterSet(newParameterSet, deleteOld = true)

    val solverResult = KSolverResult.getSolverResults(oldParameterSet.model).firstOrNull { it.parameterSet == oldParameterSet }
    if (solverResult != null) {
        DBSaveMethods.instance.saveSolverResult(solverResult.duplicate(newParameterSet))
    }
}

fun duplicateEstimationResult(oldParameterSet: KParameterSet, newParameterSet: KParameterSet): EstimationCIResults {
    val oldEstResult = EstimationCIResults.loadEstimationResult(oldParameterSet)
    return  EstimationCIResults(
        newParameterSet,
        solver = oldEstResult.solverStr,
        objectiveFunction = oldEstResult.objectiveFunctionStr,
        optimizer = oldEstResult.optimizerStr,
        optimalValue = oldEstResult.optimalValue,
        ci = oldEstResult.ciStr,
        aic = oldEstResult.aic,
        bic = oldEstResult.bic,
        aicc = oldEstResult.aicc,
        estimatedRows = oldEstResult.estimatedRows,
        fixedRows =  oldEstResult.fixedRows,
        intermediateResults = oldEstResult.intermediateResults,
        nbEstimates = oldEstResult.nbEstimates,
        nbObservations = oldEstResult.nbObservations,
        rss = oldEstResult.rss
    )
}