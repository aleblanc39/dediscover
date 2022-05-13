
#ifndef XDEMESSAGE_H_
#define XDEMESSAGE_H_

#include <iostream>
#include<string>
#include <vector>


/**
 * \brief Class to allow variable grain-level logging to arbitrary 
 * output stream.
 */


class XDEMessage {
 public:

#ifdef WIN32
#undef ERROR  // Windows defines an ERROR macro in wingdi.h.  Undefine here to avoid compilation errors
#endif
  enum MsgLevel {
		DEBUG5 = 9,
		DEBUG4 = 8,
		DEBUG3 = 7,
		DEBUG2 = 6,
		DEBUG1 = 5,
		DEBUG  = 4,
		INFO = 3,
		WARNING = 2,
		XDE_ERROR = 1,
		NONE = 0};

  typedef void (*logFcn) (MsgLevel level, const std::string &message);

  // The actual values are defined in XDEMessage.cc
  static std::string messages[]; /* = {
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
      };*/

  

  
  /* Static (Class) Variable) */
  static MsgLevel debugLevel;
  static std::ostream *outstream;
  
  

  /* Static (Class) Methods */
  static void setLogLevel(MsgLevel level);
  static void setOutputStream(std::ostream &s);
    
  static MsgLevel getLogLevel() {return debugLevel;}
  static void log(MsgLevel _code, std::string _message);

  static void setLogFunction(logFcn newFunction);
  static void defaultLogFcn(MsgLevel level, const std::string &message);
  static void resetFunction() {logFunction = &defaultLogFcn;}

 private:
  static logFcn logFunction;
  


};

#endif /*XDEMESSAGE_H_*/
