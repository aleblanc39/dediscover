#include "Macro.h"

#include <base/XDEUtil.h>
using namespace std;

Macro::Macro(const string &macroName): IntermediateSymbol(macroName) {
    usedInInitialCondition = false;
    setTimeDependent(false);
}

Macro::Macro(const string &name, unsigned lineNO, int occurenceIndex) : 
    IntermediateSymbol(name, lineNO, occurenceIndex) {
    usedInInitialCondition = false;
    setTimeDependent(false);
}


Macro::~Macro() {}




