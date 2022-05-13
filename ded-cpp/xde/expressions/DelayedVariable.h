#ifndef DELAYEDVARIABLE_H_
#define DELAYEDVARIABLE_H_
#include<memory>
#include <string>



/**
 * \brief Class to handle dependent variables with a delay.
 *
 * Each object is associated a variable name and an expressionID. The
 * expressionID is assigned by the caller for reference later, as
 * there can be more than one delay associated with a variable.
 *
 * The internal name is of the form 
 *  varName.append(separatorString).append(varCounter++).
 */


class DelayedVariable {

public:
    virtual ~DelayedVariable();

    DelayedVariable(const std::string &name, const unsigned &expressionID);

    auto getInternalName() const { return internalName;}
    auto getVarName() const {return varName;}

    auto  getExpressionID() const {return expressionID;}

private:
    static std::string separatorString;
    static int varCounter;
    std::string varName;
    std::string internalName;

    /** ID attached to this variable in the container for
	this object. In our case this is MuParserExpressionHandler.
    */
    unsigned expressionID;
};

typedef std::shared_ptr<DelayedVariable> DelayedVariablePtr;

#endif /*DELAYEDVARIABLE_H_*/
