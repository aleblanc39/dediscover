#ifndef CBIMDEFINITIONS_H
#define CBIMDEFINITIONS_H


/**
 * \brief Some defininion 
 */
#include <math.h>
#include <limits>
#include <utility>

#ifdef WINDOWS
#include <windows.h>
#define _SLEEP Sleep
#endif

#ifdef _WIN32
#include <windows.h>
#define _SLEEP Sleep
#endif

#ifdef LINUX
#include <time.h>
#include <unistd.h>
#define _SLEEP sleep
#endif

#ifdef __APPLE__
#include <time.h>
#define _SLEEP sleep
#endif

#ifdef __CYGWIN__
#include <time.h>
#define _SLEEP sleep
#endif



static double epsilon = std::numeric_limits<double>::epsilon();

inline bool double_equals(double a, double b) {
    if (fabs(a-b) < epsilon)
	return true;
    return false;
}

inline bool double_less_than(double a, double b) {
    if (fabs(a-b) < epsilon)
	return false;
    return (a < b);
}

inline bool double_greater_than(double a, double b) {
    if (fabs(a-b) < epsilon)
	return false;
    return (a > b);
}


#ifdef WINDOWS
// Microsoft does not provide an implementation of log2
// in its current C/C++ compilers.  Code is from
// http://www.tek-tips.com/viewthread.cfm?qid=1293237&page=8
inline double log2(double x)
{
  static const double xxx = 1.0/log(2.0);
  return log(x)*xxx;
}
#endif



#endif
