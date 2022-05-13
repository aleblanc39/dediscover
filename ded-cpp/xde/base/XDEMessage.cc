#include "XDEMessage.h"

using namespace std;

XDEMessage::logFcn XDEMessage::logFunction = &XDEMessage::defaultLogFcn;

string XDEMessage::messages[] = {
      "NONE",
      "ERROR",
      "WARNING",
      "INFO",
      "DEBUG" ,
      "DEBUG1",
      "DEBUG2",
      "DEBUG3",
      "DEBUG4",
      "DEBUG5"
};


void XDEMessage::setLogFunction(logFcn newFunction) {
    logFunction = newFunction;
}





// Set default value for log level and output stream.
XDEMessage::MsgLevel XDEMessage::debugLevel = XDEMessage::INFO;
ostream *XDEMessage::outstream = &cout;

void XDEMessage::setLogLevel(MsgLevel _level) {
    debugLevel = _level;
}

void XDEMessage::setOutputStream(ostream &s) {
    outstream = &s;
}


void XDEMessage::defaultLogFcn(MsgLevel _code, const string &_message) {
    *XDEMessage::outstream << "[" << XDEMessage::messages[(int)_code] << "] \n" << _message << endl;
}


void XDEMessage::log(MsgLevel _code, string _message) {
    if (debugLevel >= _code) {
	(*logFunction) (_code, _message);
    }
}
