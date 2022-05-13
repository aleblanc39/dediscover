#include <expressions/MemoryBankSynchronizer.h>
#include <base/XDEUtil.h>

using namespace std;

MemoryBankSynchronizer::MemoryBankSynchronizer(){}
MemoryBankSynchronizer::~MemoryBankSynchronizer(){}

MemoryBankSynchronizer::MemoryBankSynchronizer(MemoryBank *memBank) {
    memoryBank = memBank;
}


double *MemoryBankSynchronizer::getLocation(std::string symbol) {
    XDE_ASSERT(memoryBank != NULL);
    return memoryBank -> getLocation(symbol);
}

void MemoryBankSynchronizer::updateMemory(string symbol, double value) {
    XDE_ASSERT(memoryBank != NULL);
    memoryBank -> setValue(symbol, value);
}

int MemoryBankSynchronizer::createBlock(const vector<string> &symbolNames) {
    XDE_ASSERT(memoryBank != NULL);
    return memoryBank -> createBlock(symbolNames);
}

void MemoryBankSynchronizer::releaseBlock(int blockID) {
    XDE_ASSERT(memoryBank != NULL);
    memoryBank -> releaseBlock(blockID);
}

void MemoryBankSynchronizer::setValues(int blockID, const double *values) {
    XDE_ASSERT(memoryBank != NULL);
    memoryBank ->setValues(blockID, values);
}


void MemoryBankSynchronizer::setValues(int blockID, const double *values, int nbToCopy) {
    XDE_ASSERT(memoryBank != NULL);
    memoryBank ->setValues(blockID, values, nbToCopy);
}

const double *MemoryBankSynchronizer::getValues(int blockID) {
    XDE_ASSERT(memoryBank != NULL);
    return memoryBank -> getValues(blockID);
}

double MemoryBankSynchronizer::getValue(string varName) {
    XDE_ASSERT(memoryBank != NULL);
    return memoryBank -> getValue(varName);
}


string MemoryBankSynchronizer::generateVariable() {
    XDE_ASSERT(memoryBank != NULL);
    return memoryBank -> generateVariable();
}

void MemoryBankSynchronizer::getValues(int blockID, double *buffer) const {
    XDE_ASSERT(memoryBank != NULL);
    memoryBank -> getValues(blockID, buffer);
}
