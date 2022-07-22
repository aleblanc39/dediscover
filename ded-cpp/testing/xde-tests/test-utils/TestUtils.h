#ifndef TESTUTILS_H
#define TESTUTILS_H

#include <string>
#include <vector>
#include <memory>

template<typename T> int findElementIndex(std::vector<std::shared_ptr<T>> params, const std::string &symbol) {
    auto param = find_if(params.begin(), params.end(), 
                [=](const std::shared_ptr<T> &p) {return p->getName() == symbol;});

    return param - params.begin();

}

template<typename T> int findElementIndex(std::vector<T> params, const T &value) {
    auto param = find_if(params.begin(), params.end(), 
                [=](const T &p) {return p == value;});

    return param - params.begin();

}


#endif
