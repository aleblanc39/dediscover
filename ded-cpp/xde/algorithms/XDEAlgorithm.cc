#include "algorithms/XDEAlgorithm.h"

#include <parser/ModelParameter.h>

#include <boost/cast.hpp>
#include <iostream>

using namespace std;

namespace xde_algorithm {

GeneralParameterPtr getCtrlParameter(
    const std::vector<GeneralParameterPtr> &params, const std::string &name) {
    for (unsigned i = 0; i < params.size(); i++) {
        if (params[i]->getName() == name) {
            return params[i];
        }
    }
    return GeneralParameterPtr();
}



void XDEAlgorithm::logAlgorithmInfo(XDEMessage::MsgLevel level) {
    //     XDEMessage::log(level, getMethodName() + " called\n");

    //     for (const auto &p :  getControlParameters()) {
    //         XDEMessage::log(level, getMethodName() + " ControlParameter (" +
    //                                    p->getName() + "): " +
    //                                    p->getStringValue());
    //     }
}

// TODO_ALGM all static, add control parameters vector
// double XDEAlgorithm::getDoubleParameterValue(const std::string &paramName) {
//     return ((GenericParameter<double> *)getCtrlParameter(paramName).get())
//         ->getValue();
// }

// int XDEAlgorithm::getIntParameterValue(const std::string &paramName) {
//     return ((GenericParameter<int> *)getCtrlParameter(paramName).get())
//         ->getValue();
// }

// string XDEAlgorithm::getStringParameterValue(const std::string &paramName) {
//     return ((GenericParameter<string> *)getCtrlParameter(paramName).get())
//         ->getValue();
// }

// bool XDEAlgorithm::getBoolParameterValue(const std::string &paramName) {
//     return ((GenericParameter<bool> *)getCtrlParameter(paramName).get())
//         ->getValue();
// }

}  // namespace xde_algorithm