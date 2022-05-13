#ifndef SIMULATIONSYMBOLATTRIBUTES_H
#define SIMULATIONSYMBOLATTRIBUTES_H

#include <parameters/SymbolAttributes.h>


/**
 * \brief Class to hold the values required for the simulation.
 *
 * This class will be the template argument for the SimulationParameterSet
 * class.
 *
 */

class SimulationSymbolAttributes : public SymbolAttributes {

 public:
    SimulationSymbolAttributes(){}
    SimulationSymbolAttributes(std::string name, double v);
    virtual ~SimulationSymbolAttributes(){}

    double getValue() const {return value;}
    void setValue(double d) {value = d;}
 private:
    double value;

};

#endif

