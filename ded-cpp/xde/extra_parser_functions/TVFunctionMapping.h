#ifndef TVFUNCTIONMAPPING_H
#define TVFUNCTIONMAPPING_H

#include <extra_parser_functions/TimeVaryingFunction.h>


#include <set>
#include <string>
#include <vector>
#include <utility>

class TVFunctionMapping;

typedef std::shared_ptr<TVFunctionMapping> TVFunctionMappingPtr;
typedef std::vector<std::pair<double, double> > Vec;
/**
 * \brief Create a mapping between a time-varying functionand and a symbol
 *
 */

class TVFunctionMapping {
   public:
    TVFunctionMapping(std::string symbol);
    virtual ~TVFunctionMapping();

    auto getMappedSymbol() { return mappedSymbol; }

    void assignControlPoints(TimeVaryingFunctionPtr p, const Vec &points);

    auto isFunctionUsed() { return functionUsed; }

    auto isDirty() { return dirty; }
    void setDirty(bool b) { dirty = b; }

   private:
    TVFunctionMapping() {}

    bool functionUsed;
    std::string mappedSymbol;

    /**
     * Indicates if a changed occured that should force the symbol owning this
     * mapping to reinitialize the function. Set to true when columns are
     * modified. Can be set externally as well. Must be reset externally.
     */

    bool dirty;
};

#endif  // #ifndef TVFUNCTIONMAPPING_H
