#ifndef MEMORYBANK_H_
#define MEMORYBANK_H_

#include <vector>
#include <string>
#include <map>



/**
 * \brief Class to handle the values assigned to symbols in DEDiscover 
 * equations.
 *
 * This class creates a mapping between a symbol in an equation of
 * DEDiscover, say alpha, to a unique memory location, so that the
 * symbol value can be assigned and retrieved by different components
 * of the model without further need to synchronize.
 *
 * It also support the creation of blocks of memory which can be all
 * assigned or retrieved in one call. For instance, all the model
 * parameters could be part of one block.
 *
 */

class MemoryBank {
public:
    MemoryBank(int size=1000);
    virtual ~MemoryBank();


    /** Returns a double* to the location assigned to variable varName */
    double *getLocation(std::string varName);
    double getValue(std::string varName);

    /**
     * Assign a new value to varName. A new entry is created if one
     * doesn't exist yet.
    */

    void setValue(std::string varName, double value);

    /** Create a new block with the variable names. The return value
     * is the block id.
    */
    int createBlock(std::vector<std::string> symbolNames);
    void releaseBlock(int blockID);

    /** Copy values to a block. The size of the block is the number of
     * values copied.
     */
    void setValues(int blockID, const double *values);

    /** Copy nbToCopy values to a block. nbToCopy must be no greater
     *	than the block size.
     */
    
    void setValues(int blockID, const double *values, int nbToCopy);
    const double *getValues(int blockID);

    /**
     * Assign the values from block blockID to a buffer.
     */

    void getValues(int blockID, double *buffer) const;


    /**
     * Generate a new variable.
     *
     * Will generate a new variable not currently in the list. For now will be
     * __X_<id>
     * where if is a counter.
     *
     */


    std::string generateVariable();

private:
    int nbAllocated;
    int maxSize;
    MemoryBank(const MemoryBank &);
    std::map<std::string, double *> varmap;

    // TODO Could use smart pointers for all of those, or smart arrays to ptr.
    std::vector<double *> memory;
    unsigned memoryPtr;

    std::vector<int> blockSize;

    std::vector<double *> activeBlocks;
    std::vector<double *> inactiveBlocks;

    int generatedVariableCounter;

};

#endif /*MEMORYBANK_H_*/
