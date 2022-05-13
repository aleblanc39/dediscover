#include "DelayedVariable.h"
#include <base/XDEUtil.h>

using namespace std;

int DelayedVariable::varCounter = 0;
string DelayedVariable::separatorString = "___";

DelayedVariable::DelayedVariable(const std::string &name, const unsigned &exprID)
    :varName(name), expressionID(exprID)  {
    internalName = name;
    internalName.append(separatorString).append(XDEUtil::itoa(varCounter++));
}


DelayedVariable::~DelayedVariable() {
    XDEMessage::log(XDEMessage::DEBUG2, "Into destructor for delayed variable");

}


