
#ifndef MODELSYMBOL_H_
#define MODELSYMBOL_H_

#include <base/XDEUtil.h>
#include <base/XDEBlas.h>



#include <limits>
#include <map>
#include <string>

class ModelSymbol;

typedef std::shared_ptr<ModelSymbol> ModelSymbolPtr;

/**
 * \brief Base class for the symbols used in the model.
 */

class ModelSymbol{
   public:
    ModelSymbol(std::string name, 
                std::string description = "", bool timeDependent = false);

    ModelSymbol(std::string name, unsigned lineNO, int occurenceIndex);

    virtual ~ModelSymbol();

    void setDefinitionLine(int lineNO) { definitionLine = lineNO; }

    void setOccurenceIndex(int lineNO) { occurenceIndex = lineNO; }

    void updateFirstOccurenceLine(int lineNO) {
        if (lineNO < firstOccurenceLine || firstOccurenceLine < 0)
            firstOccurenceLine = lineNO;
    }

    int getDefinitionLine() { return definitionLine; }

    int getOccurenceIndex() { return occurenceIndex; }

    int getFirstOccurenceLine() { return firstOccurenceLine; }

    std::string getName() { return name; }

    std::string getDescription() { return description; }
    void setDescription(std::string s) { description = s; }

    unsigned getEquationVersion();

    bool isTimeDependent() { return timeDependent; }
    void setTimeDependent(bool b) { timeDependent = b; }




   protected:
   private:
    std::string name;
    std::string description;
 
    int definitionLine;
    int occurenceIndex;
    int firstOccurenceLine;

    // Disable copy assignment and copy constructors
    ModelSymbol(const ModelSymbol&);
    ModelSymbol& operator=(const ModelSymbol&);

    // bool hasMin;
    // bool hasMax;
    bool timeDependent;

    void initializeSymbol();
};

#endif /*MODELSYMBOL_H_*/
