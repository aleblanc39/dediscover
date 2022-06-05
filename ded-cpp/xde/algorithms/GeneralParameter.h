#ifndef GENERAL_PARAMETER_H_
#define GENERAL_PARAMETER_H_
#include <memory>
#include <base/XDEException.h>

#include <string>
#include <vector>

class GeneralParameter;

typedef std::shared_ptr<GeneralParameter> GeneralParameterPtr;

/**
 * \brief Virtual base class used for the control parameters.
 *
 * Control parameter used by the algorithm. All parameters are given a
 * name and one or many value. The implementation will always store in
 * vector of values, even if only one is needed. A parameter can also
 * include a list of options to handle the case of control vocabulary.
 */

class GeneralParameter  {
   public:
    GeneralParameter();
    virtual ~GeneralParameter();

    void setName(std::string n) { name = n; }
    void setDescription(std::string d) { description = d; }

    std::string getName() const { return name; }
    std::string getDescription() const { return description; }

    bool hasMinValue() { return hasMin; }
    bool hasMaxValue() { return hasMax; }

    void setHasMin(bool b) { hasMin = b; }
    void setHasMax(bool b) { hasMax = b; }

    /** Indicates a control vocabulary. */
    virtual bool hasOptions() = 0;
    /** Indicates the number of values to assign the parameter. */
    int getNbRequiredOptions() { return nbRequiredOptions; }

    /** Return the value as string, regardless of underlying type. */
    virtual std::string getStringValue() = 0;
    /** Return the values as strings, regardless of underlying type. */
    virtual std::vector<std::string> getStringValues() = 0;
    /** Return the options as strings, regardless of underlying type. */
    virtual std::vector<std::string> getStringOptions() = 0;

    void setStringValue(const std::string &s) {
        std::vector<std::string> strValues(1, s);
        setStringValues(strValues);
    }
    virtual void setStringValues(const std::vector<std::string> &values) = 0;
    virtual std::vector<std::string> getDefaultValues() = 0;

   protected:
    int nbRequiredOptions;

    std::string name;
    std::string description;

    bool hasMin;
    bool hasMax;

    bool assignedValidValue;
};

#endif
