#ifndef GENERIC_FUNCTION_LIST
#define GENERIC_FUNCTION_LIST

#include <base/XDEUtil.h>

#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/scoped_ptr.hpp>

using namespace boost::interprocess;

static boost::interprocess::interprocess_mutex mutex;

/**
 * \brief Template for a class to manage function which
 * can be added and removed in an asynchronous way.
 */

template <typename FunctionPtr>
class GenericFunctionList {
   public:
    virtual ~GenericFunctionList();

    unsigned addFunction(FunctionPtr function);
    void removeFunction(unsigned functionIndx);

    FunctionPtr getFunction(unsigned indx);
    void replaceFunction(unsigned index, FunctionPtr function);

    // Kind of a hack. Should be able to make more general.
    // Right now we know that these are the two possible signatures for the
    // classes implementing this template.
    double compute(unsigned indx);
    double compute(unsigned indx, double t);

    static GenericFunctionList *getInstance() {
        if (instance.get()) return instance.get();

        scoped_lock<interprocess_mutex> lock(mutex);
        if (!instance.get()) instance.reset(new GenericFunctionList());
        return instance.get();
    }

   private:
    GenericFunctionList();

    static boost::scoped_ptr<GenericFunctionList> instance;

    std::vector<FunctionPtr> functions;
    std::vector<unsigned> availableSlots;
    unsigned nextSlot;

    static const unsigned MaxEntries = 100000;

    // Make an entry in the functions vector available.
    void releaseEntry(unsigned u);
};

template <typename T>
boost::scoped_ptr<GenericFunctionList<T> > GenericFunctionList<T>::instance;

template <typename T>
GenericFunctionList<T>::GenericFunctionList() {
    functions.resize(MaxEntries);
    availableSlots.resize(MaxEntries);
    for (unsigned i = 0; i < MaxEntries; i++) availableSlots[i] = i;
    nextSlot = 0;
}

/**
 * Destructor: Need to release all the slots. Do it in critical region.
 */
template <typename T>
GenericFunctionList<T>::~GenericFunctionList() {}

template <typename T>
T GenericFunctionList<T>::getFunction(unsigned indx) {
    // cerr << "indx is: " << indx << endl;
    if (functions[indx].get() == NULL)
        throw XDEProgrammingException(
            "Trying to access a non-exist Time-Varying function.\n");
    return functions[indx];
}

/**
 * Release an entry in the functions array.
 *
 * This function MUST be called from inside a critical region.
 */
template <typename T>
void GenericFunctionList<T>::releaseEntry(unsigned u) {
    functions[u].reset();
    availableSlots[--nextSlot] = u;
}

/**
 * Add a TV function for this object. Executed in critical region.
 */
template <typename T>
unsigned GenericFunctionList<T>::addFunction(T function) {
    scoped_lock<interprocess_mutex> lock(mutex);
    unsigned nextAvailable = availableSlots[nextSlot++];
    functions[nextAvailable] = function;
    return nextAvailable;
}

template <typename T>
void GenericFunctionList<T>::removeFunction(unsigned functionIndx) {
    scoped_lock<interprocess_mutex> lock(mutex);
    if (functions[functionIndx].get() == NULL) {
        // std::cerr << "Removing non-existing function. Do nothing.\n";
    } else {
        XDE_ASSERT(nextSlot > 0);
        functions[functionIndx].reset();
        availableSlots[--nextSlot] = functionIndx;
    }
    //    std::cerr << "Function removed\n";
}

// TODO May want to check that the location is actually assigned.
template <typename T>
void GenericFunctionList<T>::replaceFunction(unsigned indx, T function) {
    functions[indx] = function;
}

template <typename T>
double GenericFunctionList<T>::compute(unsigned indx) {
    return functions[indx]->compute();
}

template <typename T>
double GenericFunctionList<T>::compute(unsigned indx, double t) {
    return functions[indx]->compute(t);
}

#endif
