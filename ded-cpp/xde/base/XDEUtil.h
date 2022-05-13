#ifndef XDEUTIL_H_
#define XDEUTIL_H_

#include <base/XDEBlas.h>
#include <base/XDEMessage.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

/**
 * \brief Collection of utility methods used by DEDiscover.
 *
 */

class XDEUtil {
   public:
    static TDoubleVector getTVectorDouble(
        const std::vector<double> &v);  // Used

    static std::vector<double> getDoubleVector(const TDoubleVector &td) {
        std::vector<double> ret(td.size());
        std::copy(td.begin(), td.end(), ret.begin());
        return ret;
    }

    static double getDouble(std::string str);
    static double strtod1(std::string s);  // Used
    static std::string itoa(int num);

    static double XDE_NAN;
    static double XDE_INF;
    static std::string XDE_NAN_STR;

    static TDoubleVector getColumn(const TDoubleMatrix &mat,
                                   unsigned col);  // Used
    static TDoubleVector getRow(const TDoubleMatrix &mat,
                                unsigned row);  // Used

     template <class T> static
    void printMatrix(T matrix) {
        for (int i = 0; i < matrix.size1(); i++) {
            for (int j = 0; j < matrix.size2(); j++) {
                std::cerr << matrix(i, j) << " ";
            }
            std::cerr << std::endl;
        }
    }
};

/**
 * Keep outside the class for now, till I figure out an easy way to
 * deal with templates.
 */

template <class T>
T *vectorToArray(const std::vector<T> &vec) {
    if (vec.size() == 0) return NULL;
    T *ret = new T[vec.size()];
    std::copy(vec.begin(), vec.end(), ret);
    return ret;
}

template <class T>
void xde_resize(T &arrayType, unsigned n) {
    if (arrayType.size() != n) arrayType.resize(n);
}

template <class T>
void xde_copy(T &copyTo, const T &copyFrom) {
    xde_resize(copyTo, copyFrom.size());
    copy(copyFrom.begin(), copyFrom.end(), copyTo.begin());
}

/**
 * Collect all names. When I have time, make function used a parameter
 *
 */
template <class T>
std::vector<std::string> getAllNames(const std::vector<T> &v) {
    std::vector<std::string> ret(v.size());
    std::transform(v.begin(), v.end(), ret.begin(),
                   [](T p) -> std::string { return p->getName(); });
    return ret;
}

/**
 * Define some macros to control how the asserts are handled. In the
 * code there sill be statements of the form XDE_ASSERT(cond). Flags
 * in the C++ compiler will control how they are used. It will be as follows.
 *
 * if XDE_DEBUG_ASSERT is defined, then XDE_ASSERTS are used as
 * assert() statements.
 *
 * if XDE_OPTIMIZE is used, then the XDE_ASSERT() statement are
 * removed
 *
 * If neither of the above is defined, the XDE_ASSERTs ar replaced
 * with throw(XDEProgrammingExceptions.)
 *
 * If both flags are defined: undefined.
 */

#ifdef XDE_DEBUG_ASSERT
#define XDE_ASSERT(x) assert(x)
#include <cassert>
#elif XDE_OPTIMIZE
#define XDE_ASSERT(x)
#else
#include <base/XDEMessage.h>
#include <base/XDEProgrammingException.h>
#define XDE_ASSERT(expr)                                   \
    {                                                      \
        if (!(expr)) {                                     \
            XDEMessage::log(XDEMessage::XDE_ERROR, #expr); \
            throw XDEProgrammingException(#expr);          \
        }                                                  \
    }
#endif

// Note: The #expr above is refered to as 'stringification'. Not tested with
// MSVC yet.

#endif
