#ifndef MEMORYBANKSYNCHRONIZER_H
#define MEMORYBANKSYNCHRONIZER_H

#include <expressions/MemorySynchronizer.h>
#include <base/MemoryBank.h>
#include <string>



/** \brief Implementation of MemorySynchronizer for the MemoryBank class.
 *
 */

class MemoryBankSynchronizer : public MemorySynchronizer {

 public:
    MemoryBankSynchronizer();
    MemoryBankSynchronizer(MemoryBank *memBank);
    virtual ~MemoryBankSynchronizer();

    void updateMemory(std::string symbol, double value);

    int createBlock(const std::vector<std::string> &symbolNames);
    void releaseBlock(int blockID);

    void setValues(int blockID, const double *values);
    void setValues(int blockID, const double *values, int nbToCopy);
    const double *getValues(int blockID);
    double getValue(std::string varName);
    void getValues(int blockID, double *buffer) const;

    MemoryBank *getMemoryBank(){return memoryBank;}

    virtual std::string generateVariable();
    virtual double *getLocation(std::string symbol);

 private:
    MemoryBank *memoryBank;

};



#endif
