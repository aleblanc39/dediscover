#ifndef XDEPROGRAMMINGEXCEPTION_H
#define XDEPROGRAMMINGEXCEPTION_H


#include <string>
#include "XDEException.h"


/**
 * \brief PRogramming exception class
 *
 * Class to indicate an exception which is most likely the 
 * result of a programming error. Indicates a bug in the 
 * code.
 */


class XDEProgrammingException: public XDEException {
	

	
 public:
 XDEProgrammingException()throw (){};

 XDEProgrammingException(const XDEProgrammingException& e) throw() : XDEException(e){
 }

 XDEProgrammingException(std::string m) throw () {
     msg = m + "\n";
 }
    
 inline std::string getMsg() const {return msg;}

 ~XDEProgrammingException()  throw(){}
 const char* what() const  throw () {
     return msg.c_str();
 }
 protected:
 std::string msg;
};

#endif
