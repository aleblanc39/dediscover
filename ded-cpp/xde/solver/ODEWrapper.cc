#include "ODEWrapper.h"

ODEWrapper::ODEWrapper()
{
}

ODEWrapper::~ODEWrapper()
{
}


std::vector<std::string> ODEWrapper::s_getMethodAttributes() {
    std::vector<std::string> attrs = XDESolverWrapper::getAttributes();
    attrs.push_back("ODE");
    return attrs;
}


