#include "NativeObjectManager.h"

#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <iostream>
boost::interprocess::interprocess_mutex mutex;

boost::scoped_ptr<NativeObjectManager> NativeObjectManager::instance;

using namespace boost::interprocess;

NativeObjectManager::NativeObjectManager() {
    currentSize = initialSize;
    any_entries.resize(currentSize);
    availableSlots.resize(currentSize);
    for (unsigned i = 0; i < currentSize; i++) availableSlots[i] = i;
    nextSlot = 0;
}

NativeObjectManager::~NativeObjectManager() {}

NativeObjectManager *NativeObjectManager::getInstance() {
    if (instance.get()) return instance.get();

    scoped_lock<interprocess_mutex> lock(mutex);
    // Check again that it has not been created.
    if (!instance.get()) instance.reset(new NativeObjectManager());
    return instance.get();
}

unsigned NativeObjectManager::addPtr(boost::any sharedPtr) {
    scoped_lock<interprocess_mutex> lock(mutex);

    // Check for size. IF exceeds current size, resize
    // by adding equal to initial size
    if (nextSlot == currentSize) {
        std::cerr << "Reached maximum size in NativeObjectManager. Resizing."
                  << std::endl;
        any_entries.resize(currentSize + initialSize);
        availableSlots.resize(currentSize + initialSize);
        for (unsigned i = currentSize; i < currentSize + initialSize; i++)
            availableSlots[i] = i;
        currentSize += initialSize;
    }

    any_entries[availableSlots[nextSlot]] = sharedPtr;
    unsigned ret = availableSlots[nextSlot++];
    return ret;
}

boost::any NativeObjectManager::getPtr(unsigned index) {
    return any_entries[index];
}

void NativeObjectManager::removePtr(unsigned index) {
    scoped_lock<interprocess_mutex> lock(mutex);
    assert(nextSlot > 0);
    any_entries[index] = NULL;
    availableSlots[--nextSlot] = index;
}
