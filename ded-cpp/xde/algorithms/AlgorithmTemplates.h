#ifndef ALGORITHMTEMPLATES_H
#define ALGORITHMTEMPLATES_H

#include <boost/cast.hpp>
#include <boost/tuple/tuple.hpp>
#include <iostream>
#include <map>
#include <string>

#include "XDEAlgorithm.h"

class AlgorithmFactory;
typedef std::shared_ptr<AlgorithmFactory> AlgorithmFactoryPtr;

/**
 * Series of templates used by the various Algorithm factories. All
 * these factories need to define a function that takes a string as
 * argument, and create an object based on that string.
 *
 * Class will be implemented as singletons.
 */

/**
 * Main template: Will define a maximum of 8 algorithms. Then will
 * create templates with 1, 2, etc that will used this template.
 */

/**
 * \brief Dummy class for use by the AlgorithmFactoryTemplate.
 *
 * This class is used by the AlgorithmFactoryTemplate to cope with the fact
 * tha variadic template were not available in all C++ compilers.
 */

class NoClass {
   public:
    NoClass() {}
};

typedef boost::tuple<std::string, std::string, std::vector<std::string> >
    AlgorithmInfo;
typedef std::map<std::string, AlgorithmInfo> AlgorithmInfoMap;

namespace xde_detail {
template <class T>
inline static XDEAlgorithm *getAlgorithm() {
    return boost::polymorphic_cast<XDEAlgorithm *>(new T);
}

template <>
inline XDEAlgorithm *getAlgorithm<NoClass>() {
    // Just to make compiler happy
    return NULL;
}


// TODO AL2021  This is where we create the methods when instantiating a factory.
// Both methodMap and knownMethods are populated here. Template is the actual algorithm type.

// TODO All thew getMethodNane(), getShortDescription ... can be rewritted as T::
// and made static.
template <class T>
inline void addKnownMethod(AlgorithmInfoMap &map,
                           std::vector<std::string> &knownMethods) {
    map[T::s_getMethodName()] = boost::make_tuple(
        T::s_getShortMethodDescription(), T::s_getLongMethodDescription(),
        T::s_getMethodAttributes());

    knownMethods.push_back(T::s_getMethodName());
}
template <>
void inline addKnownMethod<NoClass>(AlgorithmInfoMap &,
                                    std::vector<std::string> &) {
    // Do nothing, just want compiler to be happy
}

}  // namespace xde_detail

/**
 * \brief Base class for the algorithm factories
 *
 * Implementing classes only need to implement the createMethod method with
 * the algorithm relevant for this factory. For an implementing class, such as
 * SolverFactory, a new solver is created with the call to
 *
 *  SolverFactory solverFactory;
 *  XDESolverWrapper *w = solverFactory.createMethod(solverType)
 *
 * where one way to obtain a solver type is with:
 * string solverType = MyAlgmClass::s_getMethodName();
 */

class AlgorithmFactory {
   public:
    virtual XDEAlgorithm *createMethod(std::string methodName) = 0;

    std::string getShortMethodDescription(std::string method);
    std::string getLongMethodDescription(std::string method);
    std::vector<std::string> getMethodAttributes(std::string method);

    std::vector<std::string> getKnownMethods() { return knownMethods; }
    bool isAttributeDefined(std::string methodName, std::string attributeName);

   protected:
    AlgorithmInfoMap methodsMap;
    std::vector<std::string> knownMethods;
};

/**
 * \brief Templated base class for the algorithm factories.
 *
 * The first template is the type of algorithm (solver, estimator, ...). The
 * next 8 templates are the algorithms that are part of this factory. Requires
 * at least one. Unused templates default to the NoClass class.
 */

template <class AlgorithmType, class T1, class T2 = NoClass, class T3 = NoClass,
          class T4 = NoClass, class T5 = NoClass, class T6 = NoClass,
          class T7 = NoClass, class T8 = NoClass>
class AlgorithmFactoryTemplate : public AlgorithmFactory {
   public:
    AlgorithmFactoryTemplate();

    AlgorithmType *createMethod(std::string method) {
        std::cerr << "Creating method: " << method << std::endl;
        XDEAlgorithm *ret = NULL;

        // Allow for empty String, which will create the first.

        if (method == "") method = knownMethods[0];

        std::vector<std::string>::iterator it =
            std::find(knownMethods.begin(), knownMethods.end(), method);
        if (it == knownMethods.end())
            throw XDEException("Unknown method to create: " + method);

        unsigned position = it - knownMethods.begin();

        switch (position) {
            case 0:
                ret = xde_detail::getAlgorithm<T1>();
                break;

            case 1:
                ret = xde_detail::getAlgorithm<T2>();
                break;
            case 2:
                ret = xde_detail::getAlgorithm<T3>();
                break;
            case 3:
                ret = xde_detail::getAlgorithm<T4>();
                break;
            case 4:
                ret = xde_detail::getAlgorithm<T5>();
                break;
            case 5:
                ret = xde_detail::getAlgorithm<T6>();
                break;
            case 6:
                ret = xde_detail::getAlgorithm<T7>();
                break;
            case 7:
                ret = xde_detail::getAlgorithm<T8>();
                break;
        }

        return boost::polymorphic_cast<AlgorithmType *>(ret);
    }

    std::vector<std::string> getKnownMethods() { return knownMethods; }

   private:
};

/**
 * \brief Templated class extending the AlgorithmFactory
 *
 * As of the time where this program was used the varadic templates were not
 * implemented in all the C++ compilers we use. The way we wanted to create a
 * algorithm factory was with a statement of the form:
 *   typedef AlgorithmFactoryTemplate<XDESolverWrapper,
     SambineDDESolver,
 *   SundialsSolverWrapper> SolverFactory;
 *
 * however the number of algorithms will change depending on the factory.
 * We resolved the problem by defining the NoClass class, and put it as
 * default for all but the first template.
 *
 */

// Constructor. 
template <class AlgorithmType, class T1, class T2, class T3, class T4, class T5,
          class T6, class T7, class T8>
AlgorithmFactoryTemplate<AlgorithmType, T1, T2, T3, T4, T5, T6, T7,
                         T8>::AlgorithmFactoryTemplate() {
    // Populate known methods vector

    xde_detail::addKnownMethod<T1>(methodsMap, knownMethods);
    xde_detail::addKnownMethod<T2>(methodsMap, knownMethods);
    xde_detail::addKnownMethod<T3>(methodsMap, knownMethods);
    xde_detail::addKnownMethod<T4>(methodsMap, knownMethods);
    xde_detail::addKnownMethod<T5>(methodsMap, knownMethods);
    xde_detail::addKnownMethod<T6>(methodsMap, knownMethods);
    xde_detail::addKnownMethod<T7>(methodsMap, knownMethods);
    xde_detail::addKnownMethod<T8>(methodsMap, knownMethods);
}

#endif  // ALGORITHMTEMPLATES_H
