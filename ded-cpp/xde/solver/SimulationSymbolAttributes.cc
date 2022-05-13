#include "SimulationSymbolAttributes.h"


// TODO: Why can we put name in initialization list, next to value?
SimulationSymbolAttributes::SimulationSymbolAttributes(std::string name, double v) :  value(v) {
    setName(name);
}


SimulationSymbolAttributes::~SimulationSymbolAttributes() {

}
