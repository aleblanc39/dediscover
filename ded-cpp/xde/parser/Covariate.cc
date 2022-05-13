#include "Covariate.h"
using namespace std;


Covariate::Covariate(string name, unsigned lineNO, int occurenceIndex, TimeVaryingFunctionPtr tvf) : IntermediateSymbol(name, lineNO, occurenceIndex), defaultFunction(tvf)
{
    timeVaryingIndex = TimeVaryingFunctionList::getInstance()->addFunction(tvf);
    mapped = false;
}
