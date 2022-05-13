#ifndef MEMORYSYNCHRONIZER_H
#define MEMORYSYNCHRONIZER_H

#include <string>
#include <vector>

#include <memory>




class MemorySynchronizer;

typedef std::shared_ptr<MemorySynchronizer> MemorySynchronizerPtr;

/** 
 * \brief Virtual class to synchronize memory between expressions and model symbols.
 *
 * The main purpose of this class is to provide an convenient way to
 * synchronize the values between the model symbol objects and the
 * memory used by the different components using the expressions.
 */

class MemorySynchronizer {

 public:
    MemorySynchronizer();
    virtual ~MemorySynchronizer();

    virtual void updateMemory(std::string symbol, double value) = 0;

    virtual int createBlock(const std::vector<std::string> &symbolNames) = 0;
    virtual void releaseBlock(int blockID) = 0;

    virtual void setValues(int blockID, const double *values) = 0;
    virtual void setValues(int blockID, const double *values, int nbToCopy) = 0;
    virtual const double *getValues(int blockID) = 0;
    virtual double getValue(std::string varName) = 0;
    virtual void getValues(int blockID, double *buffer) const = 0;

    virtual double *getLocation(std::string symbol) = 0;

    virtual std::string generateVariable() = 0;

 private:


};




#endif
