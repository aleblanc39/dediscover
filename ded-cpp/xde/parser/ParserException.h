#ifndef PARSEREXCEPTION_H
#define PARSEREXCEPTION_H

#include <boost/lexical_cast.hpp>
#include <string>

/**
 * \brief Exception thrown when an error is detected during the
 * parsing process.
 */
class ParserException {
   public:
    ParserException();
    ParserException(std::string m) { msg = m + "\n"; }

    ParserException(int l, std::string m) {
        lineNO = l;
        msg = m;
    }

    inline std::string getMsg() const { return msg; }
    inline int getLineNO(){return lineNO;}

   private:
    std::string msg;
    int lineNO = 0;
};

#endif
