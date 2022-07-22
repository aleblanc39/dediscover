#ifndef PARAMETER_VALUE_H_
#define PARAMETER_VALUE_H_

#include <string>
#include <any>
#include <map>
#include <set>


struct ParameterValue {
    ParameterValue(const std::string &paramName, const std::any &value) : paramName(paramName), value(value) {}

    const std::string paramName;
    const std::any value;

};

class ParameterValueMap
{
public:

    ParameterValueMap(const std::set<ParameterValue> &values) {
        for (auto value: values) {
            paramValues.insert(std::pair<std::string,std::any>(value.paramName, value.value));
        }
    }

    std::any getValue(const std::string &paramName) {
        auto value = paramValues.find(paramName);
        if (value == paramValues.end()) {
            return std::any{};
        } else {
            return value->second;
        }

    }


private:
    std::map<std::string, std::any> paramValues;

};

#endif