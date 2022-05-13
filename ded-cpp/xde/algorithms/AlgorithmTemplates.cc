#include "AlgorithmTemplates.h"


std::string AlgorithmFactory::getShortMethodDescription(std::string method) {
    if (methodsMap.find(method) == methodsMap.end())
	throw XDEException("Unknown method: " + method);
    return boost::get<0>(methodsMap[method]);

}

std::string AlgorithmFactory::getLongMethodDescription(std::string method) {
    if (methodsMap.find(method) == methodsMap.end())
	throw XDEException("Unknown method: " + method);
    return boost::get<1>(methodsMap[method]);

}


std::vector<std::string> AlgorithmFactory::getMethodAttributes(std::string method){
    if (methodsMap.find(method) == methodsMap.end())
	throw XDEException("Unknown method: " + method);
    return boost::get<2>(methodsMap[method]);

}

bool AlgorithmFactory::isAttributeDefined(std::string methodName, std::string attributeName) {
    std::vector<std::string> attrs = getMethodAttributes(methodName);
    return find(attrs.begin(), attrs.end(), attributeName) != attrs.end();
}
