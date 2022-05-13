#ifndef INTERMEDIATESYMBOL_H
#define INTERMEDIATESYMBOL_H


#include "ModelSymbol.h"
#include "ProcessedModelExpression.h"




/**
 * \brief Base class for the macro and covariate classes.
 *
 * Most of the operations will be the same for both types of symbols.
 * They are intermediate symbols because they are used for
 * computations in the model and don't (generally) have a value of
 * interest on their own, unlike model parameters or dependent
 * variables.
 */


class IntermediateSymbol : public ModelSymbol {

 public:
    IntermediateSymbol(std::string symbolName);
    IntermediateSymbol(std::string symbolName, unsigned lineNO, int occurenceIndex);

    void assignExpression(ProcessedModelExpressionPtr e);

    auto getExpressionString() {return expression -> getOriginalExpression();}
    virtual std::string getDisplayExpression(){return expression -> getDisplayExpression();}
    ProcessedModelExpressionPtr getExpressionObject() {return expression;}


 protected:
    ProcessedModelExpressionPtr expression;
 private:
    

};

typedef std::shared_ptr<IntermediateSymbol> IntermediateSymbolPtr;

#endif //INTERMEDIATESYMBOL_H
