
#ifndef PARAMETERRANGEEXCEPTION_H_
#define PARAMETERRANGEEXCEPTION_H_

#include <string>
#include <boost/lexical_cast.hpp>

/**
 * \brief Virtual base class for the exception thrown to indicate an error in the range of values of a parameter.
 */

class ParameterRangeException {
 public:
    ParameterRangeException();
    virtual ~ParameterRangeException();

    std::string getParameterName() {return name;}
    bool hasMinValue() {return hasMin;}
    bool hasMaxValue() {return hasMax;}

    virtual std::string getMin() = 0;
    virtual std::string getMax() = 0;
    virtual std::string getValue() = 0;

 protected:
    std::string name;
    bool hasMin;
    bool hasMax;

};


/**
 * \brief Templated class for the parameter range exception. Allows any number type to be used.
 */

template <class T> class TParameterRangeException : public ParameterRangeException{

 public: 

    TParameterRangeException(std::string n, T value, T minValue, T maxValue, bool _hasMin, bool _hasMax): value(value), minValue(minValue), maxValue(maxValue) {
	name = n;
	hasMin = _hasMin;
	hasMax = _hasMax;
    }
	
    std::string getMin() {return boost::lexical_cast<std::string, T>(minValue);}
    std::string getMax() {return boost::lexical_cast<std::string, T>(maxValue);}
    std::string getValue() {return boost::lexical_cast<std::string, T>(value);}
	
    ~TParameterRangeException(){}

 private:
    T value;
    T minValue;
    T maxValue;
};





#endif /* PARAMETERRANGEEXCEPTION_H_ */
