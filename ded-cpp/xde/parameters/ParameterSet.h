#ifndef PARAMETERSET_H
#define PARAMETERSET_H

/**
 *
 *
 * Making SymbolAttributes a virtual class is another option.
 */

#include <base/XDEException.h>
#include <base/XDEBlas.h>



#include <map>
#include <string>

class AbstractParameterSet;

typedef std::shared_ptr<AbstractParameterSet> ParameterSetPtr;

/**
 * \brief Base class for the templated ParameterSet class.
 *
 * This base class is defined only to allow the creation of boost shared_ptr
 * with ParameterSet objects.
 *
 */

class AbstractParameterSet {
   public:
};

/**
 * \brief Templated class for handling parameter sets.
 *
 * It will store and manage pairs of <string, SymbolAttributes>
 * where SymbolAttributes is a template types that defines the
 * attributes used for the model symbol in this particular set.
 *
 * The ParameterSet will be given to the Algorithm objects (solver,
 * estimator, etc) to extract the information to be used in order to
 * call the core mathematical functions. So the SymbolAttribute object
 * will differ based on the algorithm for which it is used. The
 * modelSymbol can be a model parameter or a dependent variable
 * (initial condition).
 *
 */

template <class T>
class ParameterSet : public AbstractParameterSet {
   public:
    typedef std::pair<std::string, T> MapEntry;
    typedef std::map<std::string, T> MapType;
    ParameterSet() {}
    ~ParameterSet() {}

    TDoubleVector getDoubleFields(double (*getField)()) const {
        TDoubleVector ret(parameters.size());
        unsigned cntr = 0;
        for (auto t :  parameters) { ret[cntr++] = ((t.*getField())); }
        return ret;
    }

    T getParameter(std::string s) const {
        typename MapType::const_iterator iter = parameters.find(s);
        if (iter == parameters.end())
            throw XDEException(
                "ParameterSet::getParameter: cannot find symbol: " + s);
        return (*iter).second;
    }

    void addParameter(T element) {
        std::string name = element.getName();
        if (parameters.find(name) != parameters.end()) parameters.erase(name);

        parameters.insert(std::pair<std::string, T>(name, element));
    }

    /**
     * \brief Return a vector of parameters satisfying a certain
     * boolean function defined on the template.
     */

    std::vector<T> getParameters(bool (T::*f)() const) const {
        typename MapType::const_iterator it;
        std::vector<T> ret;
        for (it = parameters.begin(); it != parameters.end(); it++) {
            if (((it->second).*f)()) {
                ret.push_back(it->second);
            }
        }
        return ret;
    }

    unsigned countParameters(bool (T::*f)() const) const {
        typename MapType::const_iterator it;
        unsigned ret = 0;
        for (it = parameters.begin(); it != parameters.end(); it++) {
            if (((it->second).*f)()) {
                ret++;
            }
        }
        return ret;
    }

    /**
     * \brief  Get the names of the parameters for which the member function
     * passed as the f parameter return true.
     */
    std::vector<std::string> getParameterNames(bool (T::*f)() const) const {
        std::cerr << "Getting parameter names. There are: " << parameters.size()
                  << std::endl;

        typename MapType::const_iterator it;
        std::vector<std::string> ret;
        for (it = parameters.begin(); it != parameters.end(); it++) {
            if (((it->second).*f)()) {
                ret.push_back((it->second).getName());
            }
        }
        return ret;
    }

    int size() { return parameters.size(); }

    std::vector<T> getParameters() {
        typename MapType::const_iterator it;
        std::vector<T> ret;
        for (it = parameters.begin(); it != parameters.end(); it++) {
            ret.push_back(it->second);
        }
        return ret;
    }

   private:
    MapType parameters;
};

#endif
