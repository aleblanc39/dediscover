#ifndef TVFUNCTIONEXCEPTION_H
#define TVFUNCTIONEXCEPTION_H

#include <string>

/**
 * \brief Exception thrown by Time Varying functions.
 */

class TVFunctionException {
   public:
    TVFunctionException();
    TVFunctionException(std::string m) { msg = m + "\n"; }

    inline std::string getMsg() const { return msg; }

   private:
    std::string msg;
};

#endif
