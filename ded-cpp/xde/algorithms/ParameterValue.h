#ifndef PARAMETER_VALUE_H_
#define PARAMETER_VALUE_H_

#include <string>
#include <any>
#include <map>

class ParameterValue
{
public:
    ParameterValue() {}
    ParameterValue(const std::string &paramName, const std::any &value) : paramName(paramName), value(value) {}

    const std::string paramName;
    const std::any value;
};

class ParameterValueMap
{
public:

    ParameterValueMap(){}

    template <typename T>
    ParameterValueMap(const T &values)
    {
        for (auto value : values)
        {
            paramValues.insert(std::pair<std::string, std::any>(value.paramName, value.value));
        }
    }

    std::any getValue(const std::string &paramName) const
    {
        auto value = paramValues.find(paramName);
        if (value == paramValues.end())
        {
            return std::any{};
        }
        else
        {
            return value->second;
        }
    }


    // This will cover the most common cases. If the value is a vector or a set then the
    // conversion will be done at the calling site. 
    double getDoubleParameterValue(const std::string &paramName, double defaultValue = 0) const {

        auto val = getValue(paramName);
        if (!val.has_value()) {
            return defaultValue;
        }
        return std::any_cast<double>(val);
    }

    int getIntParameterValue(const std::string &paramName, const int defaultValue = 0) const
    {
        auto val = getValue(paramName);
        if (!val.has_value()) {
            return defaultValue;
        }
       return std::any_cast<int>(val);
    }

    std::string getStringParameterValue(const std::string &paramName, const std::string &defaultValue = "") const
    {
        auto val = getValue(paramName);
        if (!val.has_value()) {
            return defaultValue;
        }
        return std::any_cast<std::string&>(val);
    }

    bool getBoolParameterValue(const std::string &paramName, const  bool defaultValue = false) const
    {
        auto val = getValue(paramName);
        if (!val.has_value()) {
            return defaultValue;
        }
        return std::any_cast<bool>(val);
    }

private:
    std::map<std::string, std::any> paramValues;
};

#endif