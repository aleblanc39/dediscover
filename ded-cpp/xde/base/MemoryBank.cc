#include "base/MemoryBank.h"
#include <base/XDEUtil.h>
#include <boost/lexical_cast.hpp>
#include "base/XDEException.h"

using namespace std;

MemoryBank::MemoryBank(int size) {
    nbAllocated = 0;
    maxSize = size;
    generatedVariableCounter = 0;

    memoryPtr = 0;
    memory.push_back(new double[size]);
}

MemoryBank::~MemoryBank() {
    for (double *p : memory) { delete[] p; }

    for (auto b:  activeBlocks) {
        delete[] b;
    }
    for (auto ib:  inactiveBlocks) {
        delete[] ib;
    }
}

double *MemoryBank::getLocation(string varName) {
    //map<string, double *>::const_iterator it;
    auto it = varmap.find(varName);
    if (it == varmap.end()) {
        if (nbAllocated == maxSize) {
            // Memory is full. Allocate another block.
            memory.push_back(new double[maxSize]);
            memoryPtr = 0;
            nbAllocated = 0;
            // throw XDEException("No more space in memory bank.");
        }
        double *ptr = (memory[memoryPtr] + nbAllocated++);
        varmap.insert(pair<string, double *>(varName, ptr));
        return ptr;
    } else {
        return it->second;
    }
}

// Add option for non-existing variable?
void MemoryBank::setValue(string varName, double value) {
    double *loc = getLocation(varName);
    *loc = value;
}

double MemoryBank::getValue(string varName) {
    double *loc = getLocation(varName);
    return *loc;
}

// Methods to handle blocks

/**
 * Create a new block
 *
 * Create a new block containing all the model symbols listed in
 * symbolNames. To make things easier for the caller will allow for
 * blocks of 0 size.
 * 
 * AL 2021: A variable can not be in more than one block. Should validate.
 */

int MemoryBank::createBlock(vector<string> symbolNames) {
    int nbSymbols = symbolNames.size();
    double *block = NULL;

    if (nbSymbols > 0) {
        block = new double[nbSymbols];
        for (int i = 0; i < nbSymbols; i++) {
            double *loc = block + i;
            auto it = varmap.find(symbolNames[i]);
            if (it == varmap.end()) {
                varmap.insert(pair<string, double *>(symbolNames[i], loc));
            } else {
                it->second = loc;
            }
        }
    }

    activeBlocks.push_back(block);
    blockSize.push_back(nbSymbols);
    return activeBlocks.size() - 1;
}

void MemoryBank::releaseBlock(int blockID) {
    XDE_ASSERT((unsigned)blockID < activeBlocks.size());
    inactiveBlocks.push_back(activeBlocks[blockID]);
    activeBlocks[blockID] = NULL;
}

/**
 * Set the values pointed to by *values in the block.
 */

void MemoryBank::setValues(int blockID, const double *values) {
    // XDE_ASSERT((unsigned)blockID < activeBlocks.size() &&
    // activeBlocks[blockID] != NULL);
    auto size = blockSize[blockID];

    double *block = activeBlocks[blockID];
    for (int i = 0; i < size; i++) {
        block[i] = *values++;
    }
}

void MemoryBank::setValues(int blockID, const double *values, int nbToCopy) {
    XDE_ASSERT((unsigned)blockID < activeBlocks.size() &&
               activeBlocks[blockID] != NULL);
    auto size = blockSize[blockID];
    XDE_ASSERT(nbToCopy <= size);
    auto *block = activeBlocks[blockID];
    for (int i = 0; i < nbToCopy; i++) block[i] = *values++;
}

const double *MemoryBank::getValues(int blockID) {
    XDE_ASSERT((unsigned)blockID < activeBlocks.size() &&
               activeBlocks[blockID] != NULL);
    return activeBlocks[blockID];
}

void MemoryBank::getValues(int blockID, double *buffer) const {
    int size = blockSize[blockID];
    double *block = activeBlocks[blockID];
    for (int i = 0; i < size; i++) *buffer++ = block[i];
}

/**
 * Generate the next variable.
 *
 * Will generate the next variable. Will still check that the variable
 * does not already exists, just in case the programmer has been
 * sloppy.
 */

string MemoryBank::generateVariable() {
    while (true) {
        string newvar("____MEMORY_BANK_");
        newvar.append(
            boost::lexical_cast<std::string, int>(generatedVariableCounter++));
        if (varmap.find(newvar) == varmap.end()) {
            (void *)getLocation(newvar);  // Will allocate.
            return newvar;
        }
    }
}
