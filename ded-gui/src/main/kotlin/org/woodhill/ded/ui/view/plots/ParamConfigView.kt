package org.woodhill.ded.ui.view.plots

import edu.rochester.urmc.cbim.jni.JSolver
import javafx.beans.property.DoubleProperty
import javafx.beans.property.SimpleBooleanProperty
import javafx.beans.property.SimpleObjectProperty
import javafx.collections.ObservableList
import javafx.event.EventHandler
import javafx.scene.control.ScrollPane
import javafx.scene.control.Slider
import javafx.util.StringConverter
import org.woodhill.ded.models.EstimableParameterValue
import org.woodhill.ded.models.KModel
import org.woodhill.ded.models.KParameterSet
import org.woodhill.ded.ui.ProgressController
import org.woodhill.ded.ui.threading.performSimulationOnSets
import org.woodhill.ded.ui.view.EquationsChangeEvent
import org.woodhill.ded.ui.view.PlotsUpdatingEvent
import org.woodhill.ded.ui.view.ResimulateEvent
import org.woodhill.ded.ui.view.plots.popups.NewParameterSetPopup
import org.woodhill.ded.ui.view.util.createDoubletextfield
import org.woodhill.ded.ui.view.util.validateDouble
import org.woodhill.ded.workspace.DEDTabView
import org.woodhill.ded.workspace.popups.expandParameterRangeDialog
import tornadofx.*

// TODO Use titledpane
class ParamConfigView : DEDTabView("Parameter Sets") {
    private val solverTabProperties = model.item.solverTabProperties
//    fun pg() = ProgressController.getProgressControler(scope, {},{},{})
    private fun displayCurrentParamSet() =
        simtabProperties.plotVisible(simtabProperties.currentParameterSetProperty.value, true)

    private val displayCurrentParamSetProperty = SimpleBooleanProperty(displayCurrentParamSet())

    private val copyFromList = observableListOf<KParameterSet>()
    private val availableParamSetsList = observableListOf<String>()


    private fun availableParamSetObjs() = availableParamSetsList.map {
        model.item.parameterSetByName(it)
    }.toList()

    private fun updateCopyFromList() {
        copyFromList.clear()
        model.parameterSets.value.filter { it.name != currentParamSet() }.forEach {
            copyFromList.add(it)
        }
    }

    private fun updateAvailableParamSetsList() {

        val oldCurrentParamSet = currentParamSet()
        availableParamSetsList.removeIf {
            !model.parameterSets.value.map { it.name }.contains(it)
        }

        model.parameterSets.value.filter { !availableParamSetsList.contains(it.name) }.forEach {
            availableParamSetsList.add(it.name)
        }

        if (!availableParamSetsList.contains(oldCurrentParamSet)) {
            simtabProperties.currentParameterSetProperty.value = model.parameterSets.value[0].name
        }
    }

    override val root = vbox()
    private val scrollPrefHeight = 400.0
    private val copyFromProperty = SimpleObjectProperty<KParameterSet>()

    private val isCurrentParamSetDefaultProperty =
        SimpleBooleanProperty(currentParamSet() == sessionElements.defaulParameterSetName)


    // TODO Move to a different file. It is used in more than one location
    class KParameterSetStringConverter(val model: KModel) : StringConverter<KParameterSet>() {
        override fun toString(p0: KParameterSet?): String {
            return p0!!.name
        }

        override fun fromString(p0: String?): KParameterSet {
            return KParameterSet.getParameterSet(model, p0!!)!!
        }
    }

    // Is there a better, more robust way to do this? Changing the combobox<KParameterSet> entry below
    // with a function that returns it breaks the logic.
    private fun refreshView() {
        // TODO This is very fragile. Need to understand how it really works.
        root.children[0].getChildList()!!.removeAt(3)
        root.children[0].getChildList()!!.add(3, modelParametersPane())
        root.children[0].getChildList()!!.removeAt(5)
        root.children[0].getChildList()!!.add(5, depVarsScrollPane())
        displayCurrentParamSetProperty.value = simtabProperties.plotVisible(currentParamSet(), true)
    }

    init {

        updateCopyFromList()
        updateAvailableParamSetsList()

        simtabProperties.currentParameterSetProperty.onChange() {
            refreshView()
        }
        subscribe<EquationsChangeEvent> {
            refreshView()
        }

        with(root) {
            vbox {
                hbox {
                    label("Parameter Set:")
                    combobox<String>(simtabProperties.currentParameterSetProperty) {
                        items = availableParamSetsList
                        //converter = KParameterSetStringConverter(model.item.model)
                    }
                }
                checkbox("Display", displayCurrentParamSetProperty).action {
                    simtabProperties.setPlotVisibility(
                        currentParamSet(),
                        displayCurrentParamSetProperty.value
                    )
                    fire(PlotsUpdatingEvent(scope))
                }
                label("Model Parameters")
                add(modelParametersPane())
                label("Initial Conditions")
                add(depVarsScrollPane())
                gridpane {
                    row {
                        label("Copy Values From")
                        combobox<KParameterSet>(copyFromProperty) {
                            items = copyFromList
                            converter = KParameterSetStringConverter(model.item.model)
                        }
                        button("Create").apply {
                            action {
                                val nameAndDisplay = NewParameterSetPopup.getNewParameterSet(model)
                                if (!nameAndDisplay.first.equals("")) {
                                    val ps = sessionElements.copyParameterSet(nameAndDisplay.first)
                                    if (nameAndDisplay.second) {
                                        simtabProperties.currentParameterSetProperty.value = ps.name
                                    }
                                }
                            }
                        }
                        button("Delete").apply {
                            enableWhen(!isCurrentParamSetDefaultProperty)
                            action {
                                confirm(
                                    header = "Delete Confirmation",
                                    content = "Remove ${currentParamSet()}?",
                                    actionFn = {
                                        val loc =
                                            model.parameterSets.value.map { it.name }.indexOf(currentParamSet())
                                        val indxToReplace = if (loc > 0) loc - 1 else loc + 1
                                        val toRemove = model.item.parameterSetByName(currentParamSet())
                                        simtabProperties.currentParameterSetProperty.value =
                                            model.parameterSets.value[indxToReplace].name
                                        model.parameterSets.value.remove(toRemove)
                                        availableParamSetsList.remove(toRemove.name)
                                        simtabProperties.setPlotVisibility(toRemove.name, false)
                                        //sessionElements.deletedParameterSets.add(toRemove)
                                        sessionElements.deleteParameterSet(toRemove)
                                        fire(PlotsUpdatingEvent(scope))
                                    })
                            }
                        }
                    }
                    row {
                        button("Simulate").apply {
                            action {
                                performSimulationOnSets(
                                    //find(),
                                    pg(),
                                    { fireOnSimulationSuccess() },
                                    model,
                                    solverTabProperties.algmProperties.algmInstance!! as JSolver,
                                    model.item.parameterSetByName(currentParamSet())
                                )
                            }
                        }
                        button("Simulate All").apply {
                            action {
                                performSimulationOnSets(
                                    //find(),
                                    pg(),
                                    { fireOnSimulationSuccess() },
                                    model,
                                    solverTabProperties.algmProperties.algmInstance!! as JSolver,
                                    *availableParamSetObjs().toTypedArray()
                                )
                            }
                        }
                    }
                    row {
                        button("Display All").action {
                            displayCurrentParamSetProperty.value = true
                            availableParamSetObjs().forEach {
                                simtabProperties.setPlotVisibility(
                                    it.name,
                                    true
                                )
                            }
                            fire(PlotsUpdatingEvent(scope))
                        }
                        button("Display Only").action {
                            simtabProperties.setPlotVisibility(
                                currentParamSet(),
                                true
                            )
                            displayCurrentParamSetProperty.value = true
                            availableParamSetObjs().filter { it != currentParamSetObj() }.forEach {
                                simtabProperties.setPlotVisibility(
                                    it.name,
                                    false
                                )
                            }
                            fire(PlotsUpdatingEvent(scope))
                        }
                    }
                }
            }
        }

        copyFromProperty.onChange {
            val cps = currentParamSetObj()
            it?.allParameterValues()?.forEach { fromParam ->
                cps.getParamWithName(fromParam.name).defaultValueProperty.value = fromParam.defaultValue
                cps.getParamWithName(fromParam.name).maxValueProperty.value = fromParam.maxValue
                cps.getParamWithName(fromParam.name).minValueProperty.value = fromParam.minValue
            }
            copyFromProperty.value = null

        }

        model.parameterSets.onChange {
            updateCopyFromList()
            updateAvailableParamSetsList()
        }

        simtabProperties.currentParameterSetProperty.onChange {
            updateCopyFromList()
            isCurrentParamSetDefaultProperty.value =
                (simtabProperties.currentParameterSetProperty.value == sessionElements.defaulParameterSetName)
        }
    }


    private fun modelParametersPane(): ScrollPane {
        return createScrollpane(currentParamSetObj().modelParameterValues.asObservable())
    }

    private fun depVarsScrollPane(): ScrollPane {
        return createScrollpane(currentParamSetObj().initialConditionsValues.asObservable())
    }


    private fun simulateIfAutoRefresh() = if (simtabProperties.autoRefreshPlots.value) {
        performSimulationOnSets(
            pg(),
            { fireOnSimulationSuccess() },
            model,
            solverTabProperties.algmProperties.algmInstance!! as JSolver,
            currentParamSetObj()
        )
    } else {
    }

    private fun createScrollpane(variables: ObservableList<EstimableParameterValue>): ScrollPane {
        return scrollpane {
            vbox {
                variables.forEach { thisVar ->
                    val prop = thisVar.defaultValueProperty
                    val slider = getSlider(thisVar, prop)
                    val tf = createDoubletextfield(this, thisVar.defaultValueProperty) {
                        if (thisVar.expandParameterRangeDialog(thisVar.defaultValue)) {
                            println("New values?")
                            println(thisVar.defaultValue)
                            slider.value = thisVar.defaultValue
                            simulateIfAutoRefresh()
                        }
                        true
                    }

                    tf.setOnKeyReleased {
                        validateDouble(tf)
                    }

                    thisVar.defaultValueProperty.onChange {
                        slider.value = thisVar.defaultValue
                        fire(ResimulateEvent(scope))
                    }

                    thisVar.minValueProperty.onChange {
                        slider.min = thisVar.minValue
                    }
                    thisVar.maxValueProperty.onChange {
                        slider.max = thisVar.maxValue
                    }
                    hbox {
                        label(thisVar.modelSymbol.name)
                        add(slider)
                        add(tf)
                    }
                }
            }
            prefHeight = scrollPrefHeight
        }
    }


    private fun getSlider(estimableSymbol: EstimableParameterValue, prop: DoubleProperty): Slider {
        return slider {
            min = estimableSymbol.minValue
            max = estimableSymbol.maxValue
            value = estimableSymbol.defaultValue
            isShowTickMarks = true
            isShowTickLabels = true
            minorTickCount = 3
            majorTickUnit = ((estimableSymbol.maxValue - estimableSymbol.minValue) / 2.0).coerceAtLeast(0.1)

            onMouseReleased = EventHandler {
                if (prop.value != this.value) {
                    prop.value = this.value
                    simulateIfAutoRefresh()
                }
            }
        }
    }
}



