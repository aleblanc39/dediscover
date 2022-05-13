#ifndef XDEEXCEPTION_H
#define XDEEXCEPTION_H

#include <exception>
#include <string>

/**
 * \brief Exception object for most of DEDiscover errors.
 */

class XDEException : public std::exception {
   public:
    XDEException() throw(){};
    XDEException(const XDEException& e) throw() { this->msg = e.getMsg(); }

    XDEException(std::string m) throw() { msg = m + "\n"; }

    inline std::string getMsg() const { return msg; }

    ~XDEException() throw() {}
    const char* what() const throw() { return msg.c_str(); }

   protected:
    std::string msg;
};

#endif
