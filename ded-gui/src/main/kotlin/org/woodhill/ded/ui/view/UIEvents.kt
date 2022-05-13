package org.woodhill.ded.ui.view

import org.woodhill.ded.models.KModel
import tornadofx.*

class CovariateMappingChanged(scope: Scope) : FXEvent(EventBus.RunOn.ApplicationThread, scope)

class DatatatableDataChangeEvent(scope: Scope) : FXEvent(scope = scope)
class DatatatableStructureChangeEvent(scope: Scope) : FXEvent(scope = scope)

class DataMappingChangeEvent(scope: Scope) : FXEvent(EventBus.RunOn.ApplicationThread, scope = scope)

class PlotsUpdatingEvent(scope: Scope) : FXEvent(EventBus.RunOn.ApplicationThread, scope)
class ResimulateEvent(scope: Scope) : FXEvent(EventBus.RunOn.ApplicationThread, scope)
class SimulationDoneEvent : FXEvent(EventBus.RunOn.ApplicationThread)
class SymbolDescChangeEvent(scope: Scope): FXEvent(EventBus.RunOn.ApplicationThread, scope)
class EquationsChangeEvent(scope: Scope) : FXEvent(EventBus.RunOn.ApplicationThread, scope)


class ModelAddedEvent(val model: KModel) : FXEvent()
class PackageAddedEvent(val newPackage: String) : FXEvent()
class PackageRemovedEvent(val packageName: String) : FXEvent()
class ModelDeletedEvent(val model: KModel) : FXEvent()
class ModelDeletedEventID(val modelID: Int) : FXEvent()
class ModelSavedEvent(val model: KModel) : FXEvent()

class MenuAffectingEvent(): FXEvent()