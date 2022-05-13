#include "algorithms/XDEAlgorithm.h"

#include <parser/ModelParameter.h>

#include <boost/cast.hpp>
#include <iostream>

using namespace std;

XDEAlgorithm::XDEAlgorithm() { }

XDEAlgorithm::~XDEAlgorithm() {}

/**
 * Copy constructor. Will only copy control parameters.
 */

XDEAlgorithm::XDEAlgorithm(const XDEAlgorithm &a) {
 
}

GeneralParameterPtr XDEAlgorithm::getCtrlParameter(string name) {
    return getCtrlParameter(controlParameters, name);
}

// TODO Ust std::find
GeneralParameterPtr XDEAlgorithm::getCtrlParameter(
    const std::vector<GeneralParameterPtr> params, string name) {
    for (unsigned i = 0; i < params.size(); i++) {
        if (params[i]->getName() == name) {
            return params[i];
        }
    }
    return GeneralParameterPtr();
}

vector<GeneralParameterPtr> XDEAlgorithm::getControlParameters() {
    return controlParameters;
}

vector<GeneralParameterPtr> XDEAlgorithm::getControlParameters() const {
    return controlParameters;
}

void XDEAlgorithm::logAlgorithmInfo(XDEMessage::MsgLevel level) {
    XDEMessage::log(level, getMethodName() + " called\n");

    for (const auto &p :  getControlParameters()) {
        XDEMessage::log(level, getMethodName() + " ControlParameter (" +
                                   p->getName() + "): " + p->getStringValue());
    }
}


double XDEAlgorithm::getDoubleParameterValue(const std::string &paramName) {
    return ((GenericParameter<double> *)getCtrlParameter(paramName).get())->getValue();
}

int XDEAlgorithm::getIntParameterValue(const std::string &paramName) {
    return ((GenericParameter<int> *)getCtrlParameter(paramName).get())->getValue();
}

string XDEAlgorithm::getStringParameterValue(const std::string &paramName) {
    return ((GenericParameter<string> *)getCtrlParameter(paramName).get())->getValue();
}

bool XDEAlgorithm::getBoolParameterValue(const std::string &paramName) {
    return ((GenericParameter<bool> *)getCtrlParameter(paramName).get())->getValue();
}

