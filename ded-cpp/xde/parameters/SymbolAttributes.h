#ifndef SYMBOLATTRIBUTES_H
#define SYMBOLATTRIBUTES_H




#include <string>

/**
 * \brief Base class for all the classes that will contain information for a
 * ModelSymbol.
 *
 * The base class only defines the name. The extending class
 * can define any field. Will be used mostly for the
 * parameter sets.
 */


class SymbolAttributes {

 public:
    SymbolAttributes(){}
    virtual ~SymbolAttributes(){}

    void setName(std::string s) {name = s;}
    std::string getName() const {return name;}

 protected:
    std::string name;

};



#endif
