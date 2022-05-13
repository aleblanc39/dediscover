#ifndef MACRO_H
#define MACRO_H

#include "IntermediateSymbol.h"

#include <string>

class Macro;
typedef std::shared_ptr<Macro> MacroPtr;

/**
 * \brief Class to handle the macros from the model.
 *
 * It contains information on whether it is used in initial conditions
 * in order to detect invalid use.
 */

class Macro : public IntermediateSymbol {

public:

    Macro(std::string macroName);
    Macro(std::string macroName, unsigned lineNO, int occurenceIndex);
    virtual ~Macro();

    void setUsedInInitialCondition(bool b) {usedInInitialCondition = b;}
    bool isUsedInInitialCondition() {return usedInInitialCondition;}

 private:
    bool usedInInitialCondition;

};

#endif /*MACRO_H*/
