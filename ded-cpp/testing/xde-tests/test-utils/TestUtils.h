#ifndef TESTUTILS_H
#define TESTUTILS_H

#include <string>
#include <iostream>
#include <base/XDEUtil.h>
#include <objectfunction/LeastSquare.h>
#include <extra_parser_functions/TimeVaryingFunction.h>


extern void displayErrorsCount(std::string pgmName, int nbErrors);
extern void getControlPoints(TDoubleVector &x, TDoubleVector &y);


/**
 * Create a basic data table to use for testing.
 */

extern ModelPtr getBasicModel();

//TODO: Use a sstream to allow other data types and and class with the
//     << operator defines.
template<class T> std::string vec2string(T vec) {
    std::ostringstream sstr;
    for (unsigned i = 0; i < vec.size(); i++) {
	sstr << vec[i];
	if (i < vec.size()-1)
	    sstr << " ";
    }
    return sstr.str();
}


template <class T> void printMatrix(T matrix) {
    for (int i=0; i<matrix.size1(); i++) {
        for (int j=0; j<matrix.size2(); j++) {
            std::cerr << matrix(i,j) << " ";
        }
        std::cerr << std::endl;
    }
}
 



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


extern void reportError(int no, std::string progName, std::string procName, std::string msg="");

#endif
