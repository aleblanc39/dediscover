#include "GenericParameter.h"

/** Quick fix to handle booleans. **/
template <>
void GenericParameter<bool>::setStringValues(
    const std::vector<std::string> &newValues) {
    values.clear();
    for (const auto &s: newValues) {
        values.push_back((s[0] == 't' || s[0] == 'T' || s[0] == '1') ? true
                                                                     : false);
    }
}

/** And another quick fix to handle nan's in doubles. Getting less generic by
 * the minute. **/
template <>
void GenericParameter<double>::setStringValues(
    const std::vector<std::string> &newValues) {
    values.clear();
    for (const auto &s: newValues) {
        double d = XDEUtil::XDE_NAN;
        try {
            d = boost::lexical_cast<double>(s);
        } catch (...) {
            // Any exception  assign NaN
        }
        values.push_back(d);
    }
}
