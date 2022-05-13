#ifndef GENERIC_PARAMETER_H
#define GENERIC_PARAMETER_H
#include <algorithms/GeneralParameter.h>
#include <base/XDEBase.h>
#include <base/XDEException.h>
#include <base/XDEUtil.h>
#include <parameters/ParameterRangeException.h>

#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <boost/type_traits/is_same.hpp>

/**
 * \brief Templated class extending the GeneralParameter class
 *
 * Defines a set of constructors and methods for control
 * parameters. The file ConcreteParameters.h includes type definitions
 * based on the templates that can be used through the code.
 */

template <class GP>
class GenericParameter : public GeneralParameter {
   public:
    GenericParameter();
    GenericParameter(const std::string &name, GP def, std::vector<GP> optionList,
                     GP min, GP max, bool hasMin, bool hasMax,
                     std::string desc);

    GenericParameter(const std::string &name, GP def, GP min, GP max, bool hasMin,
                     bool hasMax, std::string desc);

    GenericParameter(const std::string &name, std::vector<GP> def,
                     int nbOptionsRequired, GP min, GP max, bool hasMin,
                     bool hasMax, std::string desc);

    GenericParameter(const std::string &name, std::vector<GP> def,
                     int nbOptionsRequired, std::vector<GP> optionList, GP min,
                     GP max, bool hasMin, bool hasMax, std::string desc);

    template <class T>
    bool is_same() {
        return boost::is_same<T, GP>::value;
    }

    virtual ~GenericParameter();

    virtual GP getValue();
    virtual std::vector<GP> getValues() { return values; }
    std::string getStringValue() {
        return boost::lexical_cast<std::string>(values[0]);
    }
    void setValue(const GP &v);
    void setValueFromString(const std::string &v);
    void setValues(const std::vector<GP> &v);

    GP getMin() { return min; }
    GP getMax() { return max; }

    GP getDefault() { return defaultValues[0]; }
    const auto &getDefaults() { return defaultValues; }
    std::string getDefaultValue();
    std::vector<std::string> getDefaultValues();

    std::vector<GP> getOptions() { return options; }
    virtual std::vector<std::string> getStringOptions();

    void setMin(GP m) { min = m; }
    void setMax(GP m) { max = m; }
    void setDefault(const GP &d);
    void setDefaults(const std::vector<GP> &d);
    void setOptions(const std::vector<GP> &v) { options = v; }

    bool hasOptions();

    virtual void setStringValues(const std::vector<std::string> &values);
    virtual std::vector<std::string> getStringValues();

    std::string getType() { return "GP"; }

   protected:
    std::vector<GP> defaultValues;
    std::vector<GP> values;
    GP min;
    GP max;

    std::vector<GP> options;

    void setInitialValues(const std::string &n, GP def, GP min, GP max, bool hasMin,
                          bool hasMax, std::string desc);
    void setInitialValues(const std::string &n, std::vector<GP> def,
                          int nbRequiredOptions, GP min, GP max, bool hasMin,
                          bool hasMax, std::string desc);

    std::string typeName;
};

template <class GP>
GenericParameter<GP>::GenericParameter() {}

template <class GP>
GenericParameter<GP>::GenericParameter(const std::string &n, GP def,
                                       std::vector<GP> optionList, GP min,
                                       GP max, bool hasMin, bool hasMax,
                                       std::string desc) {
    setInitialValues(n, def, min, max, hasMin, hasMax, desc);
    setOptions(optionList);
}

template <class GP>
GenericParameter<GP>::GenericParameter(const std::string &n, GP def, GP min, GP max,
                                       bool hasMin, bool hasMax,
                                       std::string desc) {
    setInitialValues(n, def, min, max, hasMin, hasMax, desc);
}

template <class GP>
GenericParameter<GP>::GenericParameter(const std::string &name, std::vector<GP> def,
                                       int nbReqOpt, GP min, GP max,
                                       bool hasMin, bool hasMax,
                                       std::string desc) {
    setInitialValues(name, def, nbReqOpt, min, max, hasMin, hasMax, desc);
}

template <class GP>
GenericParameter<GP>::GenericParameter(const std::string &name, std::vector<GP> def,
                                       int nbReqOpt, std::vector<GP> optionList,
                                       GP min, GP max, bool hasMin, bool hasMax,
                                       std::string desc) {
    setOptions(optionList);
    setInitialValues(name, def, nbReqOpt, min, max, hasMin, hasMax, desc);
}

template <class GP>
GenericParameter<GP>::~GenericParameter() {}

template <class GP>
void GenericParameter<GP>::setInitialValues(const std::string &n, std::vector<GP> def,
                                            int nbReqOpt, GP min, GP max,
                                            bool hasMin, bool hasMax,
                                            std::string desc) {
    values.resize(def.size());
    defaultValues.resize(def.size());
    nbRequiredOptions = nbReqOpt;
    setDefaults(def);
    setValues(def);
    setName(n);
    setDescription(desc);
    setHasMin(hasMin);
    setHasMax(hasMax);
    setMin(min);
    setMax(max);
}

template <class GP>
void GenericParameter<GP>::setInitialValues(const std::string &n, GP def, GP min,
                                            GP max, bool hasMin, bool hasMax,
                                            std::string desc) {
    values.resize(1);
    defaultValues.resize(1);
    nbRequiredOptions = 1;
    setName(n);
    setDescription(desc);
    setDefault(def);
    setValue(def);
    setMin(min);
    setHasMin(hasMin);
    setHasMax(hasMax);
    setMax(max);
}

template <class GP>
void GenericParameter<GP>::setValue(const GP &v) {
    if (nbRequiredOptions > 1) {
        throw XDEException("Wrong number of values assigned");
    }
    if ((hasMin && (v < min)) || (hasMax && (v > max)))
        throw new TParameterRangeException<GP>(name, min, max, v, hasMin,
                                               hasMax);
    if (values.size() != 1) values.resize(1);
    values[0] = v;
}

template <class GP>
GP GenericParameter<GP>::getValue() {
    return values[0];
}

template <class GP>
void GenericParameter<GP>::setValues(const std::vector<GP> &v) {
    if (values.size() != (unsigned)nbRequiredOptions && nbRequiredOptions > 0) {
        throw XDEException("Parameter expects more than one valuen");
    }
    for (unsigned i = 0; i < v.size(); i++) {
        if ((hasMin && (v[i] < min)) || (hasMax && (v[i] > max)))
            throw new TParameterRangeException<GP>(name, min, max, v[i], hasMin,
                                                   hasMax);
    }
    values = v;
}

template <class GP>
void GenericParameter<GP>::setDefault(const GP &d) {
    if (values.size() != 1) {
        throw XDEException("Parameter expects more than one value");
    }
    defaultValues[0] = d;
}

template <class GP>
void GenericParameter<GP>::setDefaults(const std::vector<GP> &d) {
    if (values.size() != d.size()) {
        throw XDEException("Parameter expects more than one value");
    }
    defaultValues = d;
}

template <class GP>
bool GenericParameter<GP>::hasOptions() {
    if (options.size() > 0) return true;
    return false;
}

template <class GP>
void GenericParameter<GP>::setStringValues(
    const std::vector<std::string> &newValues) {
    values.resize(newValues.size());
    typedef GP (*f2)(const std::string &);
    f2 p2 = &boost::lexical_cast<GP, std::string>;

    transform(newValues.begin(), newValues.end(), values.begin(), p2);
}

template <class GP>
std::vector<std::string> GenericParameter<GP>::getStringValues() {
    std::vector<std::string> ret(values.size());
    // Bug in boost 1.53, so replace the following with code below
    //    transform(values.begin(), values.end(), ret.begin(),
    //    boost::lexical_cast<std::string, GP>);

    typedef std::string (*f2)(const GP &);
    f2 p2 = &boost::lexical_cast<std::string, GP>;
    transform(values.begin(), values.end(), ret.begin(), p2);

    return ret;
}

template <class GP>
std::vector<std::string> GenericParameter<GP>::getDefaultValues() {
    XDE_ASSERT(defaultValues.size() >= 1);
    std::vector<std::string> ret(defaultValues.size());

    // Also for boost 1.53 bug
    //    transform(defaultValues.begin(), defaultValues.end(), ret.begin(),
    //    boost::lexical_cast<std::string, GP>);

    typedef std::string (*f2)(const GP &);
    f2 p2 = &boost::lexical_cast<std::string, GP>;
    transform(defaultValues.begin(), defaultValues.end(), ret.begin(), p2);

    return ret;
}

template <class GP>
std::string GenericParameter<GP>::getDefaultValue() {
    XDE_ASSERT(defaultValues.size() == 1);
    return boost::lexical_cast<std::string, GP>(defaultValues[0]);
}

template <class GP>
std::vector<std::string> GenericParameter<GP>::getStringOptions() {
    std::vector<std::string> ret(options.size());
    typedef std::string (*f2)(const GP &);
    f2 p2 = &boost::lexical_cast<std::string, GP>;

    transform(options.begin(), options.end(), ret.begin(), p2);
    return ret;
}

template <>
void GenericParameter<bool>::setStringValues(
    const std::vector<std::string> &newValues);
template <>
void GenericParameter<double>::setStringValues(
    const std::vector<std::string> &newValues);





#endif
