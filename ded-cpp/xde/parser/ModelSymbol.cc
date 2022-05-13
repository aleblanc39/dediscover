

#include <boost/bind.hpp>

#include <base/XDEException.h>
#include <base/XDEUtil.h>
#include "ModelSymbol.h"


#include <base/XDEMessage.h>

using namespace std;

ModelSymbol::ModelSymbol(string name,
			 string description,
			 bool timeDependent) 
    : name(name),
      description(description),
      timeDependent(timeDependent){
    // TODO Use MAX_UNSIGNED, or whatever it is called
    firstOccurenceLine = 100000;

}


ModelSymbol::ModelSymbol(std::string name, unsigned lineNO, int occurenceIndex):name(name), definitionLine(lineNO), occurenceIndex(occurenceIndex),  firstOccurenceLine(occurenceIndex) {
}



ModelSymbol::~ModelSymbol() {
    XDEMessage::log(XDEMessage::DEBUG4,
		    string("Will delete a model symbol into destructor...").append(getName()));
}


