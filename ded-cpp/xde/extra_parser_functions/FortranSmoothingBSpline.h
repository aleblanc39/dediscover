#ifndef FORTRANSMOOTHINGBSPLINE_H
#define FORTRANSMOOTHINGBSPLINE_H

#include "DerivableSpline.h"

/**
 * \brief Wrapper around the Smoothing B-Spline method implemented
 * in Fortran.
 */




class FSmoothingBSpline : public DerivableSpline {
    const static size_t max_size;

   public:
    typedef struct {
        double *knots;
        double *coefs;
        int order, sizeknots, sizecoefs;
    } sp;


    const static std::string gcvOption;
    const static std::string toleranceOption;
    const static std::string orderOption;



    FSmoothingBSpline();
    FSmoothingBSpline(std::string functionString);
    FSmoothingBSpline(std::string functionString, MemorySynchronizerPtr mem);
    virtual ~FSmoothingBSpline();

    std::string getShortMethodDescription() {
        return s_getShortMethodDescription();
    }
    static std::string s_getShortMethodDescription() {
        return "Creates a basis spline (Fortran version).";
    }


    std::string getLongMethodDescription() {
        return s_getLongMethodDescription();
    }

    static std::string s_getLongMethodDescription() {
        return "Creates a basis-spline of the specified order by smoothing the "
               "provided (time, value) data. Replicates are allowed, meaning "
               "time points need not be unique.";
    }

    static std::vector<std::string> s_getMethodAttributes() {
        return std::vector<std::string>();
    }

    double estimateResultingObjectSize() { return 0; }
    double computeDerivative(double t);
    double computeFunction(const double &x);

    static std::string s_getMethodName() { return "FSmoothing B-Spline"; }
    std::string getMethodName() { return s_getMethodName(); }

   protected:
    virtual void initializeSpecific();
    sp spline;

 
};

#endif  // FORTRANSMOOTHINGBSPLINE_H
