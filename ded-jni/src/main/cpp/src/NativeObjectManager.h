#ifndef NATIVE_OBJECT_MANAGER_H
#define NATIVE_OBJECT_MANAGER_H

/**
 * Class to handle the native objects.
 *
 * This class uses boost smart pointers to handle the pointers from the java
 * objects to their corresponding C++ counterparts. It will make memory
 * management much easier while allowing great flexibility in creating and
 * releasing objects from the java side.
 *
 * This class is implemented loosely following the singleton pattern.
 */

#include <vector>

#include <base/XDEBase.h>
#include <boost/any.hpp>
#include <boost/scoped_ptr.hpp>


class NativeObjectManager {

public:
     unsigned addPtr(boost::any sharedPtr);
     
     boost::any getPtr(unsigned index);
     void removePtr(unsigned index);
     static NativeObjectManager *getInstance();
     virtual ~NativeObjectManager();

private:
    NativeObjectManager();

    std::vector<boost::any> any_entries;
    std::vector<unsigned> availableSlots;
    int nextSlot;
    unsigned currentSize;
    static boost::scoped_ptr<NativeObjectManager> instance;
    static const unsigned initialSize = 10000;
};


#endif //NATIVE_OBJECT_MANAGER_H
