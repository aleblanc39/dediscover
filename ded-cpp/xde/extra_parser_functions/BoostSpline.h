#ifndef Boost_SPLINE_H
#define Boost_SPLINE_H
#include <algorithms/ConcreteParameters.h>
#include <boost/math/interpolators/cubic_b_spline.hpp>
#include <extra_parser_functions/DerivableSpline.h>


class BoostSpline : public DerivableSpline {
   public:

    static const std::string orderParam;
    

    BoostSpline() {}
    BoostSpline(std::string functionString)
        : DerivableSpline(functionString) {}
    BoostSpline(std::string functionString, MemorySynchronizerPtr mem)
        : DerivableSpline(functionString, mem) {}

    ~BoostSpline(){}

    typedef std::pair<double, double> ControlPoint;
    typedef std::pair<double, double> Knot;


    const auto &getControlPoints() { return controlPoints; }
    std::vector<Knot> getKnots();

    virtual double computeFunction(const double &x);

    // For now have a default for all splines just to compile, but throw an
    // exception is actually used.
    virtual double computeDerivative(double /*t*/) {
        throw XDEException(
            "Derivative function not yet implemented for specific spline");
    }

    void initializeSpecific();
    virtual double estimateResultingObjectSize() { return 0; }

    static std::string s_getMethodName() { return "Boost Cubic Spline"; }
    std::string getMethodName() { return s_getMethodName(); }

    std::string getShortMethodDescription() {
        return s_getShortMethodDescription();
    }
    static std::string s_getShortMethodDescription() {
        return "Cubic interpolation spline method that is part of the Boost library.";
    }

    std::string getLongMethodDescription() {
        return s_getLongMethodDescription();
    }
    static std::string s_getLongMethodDescription() {
        return "Cubic interpolation spline method that is part of the Boost library.";
    }

    static std::vector<std::string> s_getMethodAttributes() {return std::vector<std::string>();}

   private:
    std::shared_ptr<boost::math::cubic_b_spline<double>> boost_spline;

   protected:
};

#endif  // Boost_SPLINE_H