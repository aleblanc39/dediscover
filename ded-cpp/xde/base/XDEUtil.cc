#include "base/XDEUtil.h"
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/limits.hpp>
#include <boost/numeric/conversion/bounds.hpp>
#include <boost/lexical_cast.hpp>
#include <base/XDEException.h>
#include <iomanip>

using namespace boost;
using namespace std;

// TODO Use function, so we don't have to deal with nasty initialization bug?
double XDEUtil::XDE_NAN = std::numeric_limits<double>::quiet_NaN();
double XDEUtil::XDE_INF = std::numeric_limits<double>::infinity();
string XDEUtil::XDE_NAN_STR = "NaN";

TDoubleVector XDEUtil::getTVectorDouble(const vector<double> &v)
{

    TDoubleVector tv(v.size());
    for (unsigned i = 0; i < v.size(); i++)
        tv[i] = v[i];
    return tv;
}

string XDEUtil::itoa(int num)
{
    stringstream converter;
    converter << num;
    return converter.str();
}

// Can't remember why we did the inf part.

double XDEUtil::getDouble(string str)
{
    trim(str);

    // Having the toUpper here is a problem if we convert from the parser.
    // INF should return infinity, while inf should throw an exception.
    // But this was probably written to handle data file on which we have little
    // control.
    to_upper(str);

    if (str == "-INF")
        return -numeric_limits<double>::infinity();
    if (str == "+INF" || str == "INF")
        return numeric_limits<double>::infinity();
    if (str == "NAN" || str == "")
        return XDEUtil::XDE_NAN;
    // What windows recognizes. In case we did not save using double2str
    if (str == "1.#QNAN")
        return XDEUtil::XDE_NAN;
    //return strtod1(str);
    return boost::lexical_cast<double>(str);
}

double XDEUtil::strtod1(string s)
{
    trim(s);
    char *endptr;
    const char *str = s.c_str();
    //errno = 0;
    double ret = strtod(str, &endptr);
    // if (s.size() == 0 || endptr != str + s.size())
    // 	throw XDEException(string("Bad value to convert to double: ").append(s));

    return ret;
}

/**
 * Required instead of boost::lexical_cast in order to have the same
 * format for double across platforms. 
 */



TDoubleVector XDEUtil::getRow(const TDoubleMatrix &mat, unsigned row)
{
    XDE_ASSERT(row < mat.size1());
    unsigned nCols = mat.size2();
    TDoubleVector ret(nCols);
    for (unsigned i = 0; i < nCols; i++)
    {
        ret[i] = mat(row, i);
    }
    return ret;
}

TDoubleVector XDEUtil::getColumn(const TDoubleMatrix &mat, unsigned col)
{
    XDE_ASSERT(col < mat.size2());
    unsigned nRows = mat.size1();
    TDoubleVector ret(nRows);
    for (unsigned i = 0; i < nRows; i++)
    {
        ret[i] = mat(i, col);
    }
    return ret;
}
