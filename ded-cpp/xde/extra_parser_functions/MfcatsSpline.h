#ifndef MFCATS_SPLINE_H
#define MFCATS_SPLINE_H
#include <b_spline.h>
#include <algorithms/ConcreteParameters.h>
#include <boost/math/interpolators/cubic_b_spline.hpp>

#include <extra_parser_functions/DerivableSpline.h>
#include <boost/make_shared.hpp>

/**
 * Wrapper around the LibSpline code:
 *  https://github.com/mfcats/SplineLib
 *
 * This links to an older version. The newest doesn't play well with
 * gcc-7.
 * */

class MfcatsSpline : public DerivableSpline {
   public:

    static const std::string orderParam;
    

    void init() {

        // TODO_ALGM
        // addToControlParameters(std::make_shared<IntParameter>(
        // orderParam, 3, 2, 5, true, true,
        // "The order. Currently set to 3. Fixed. Till I work it out."));
    }

    MfcatsSpline() {init();}
    MfcatsSpline(std::string functionString)
        : DerivableSpline(functionString) {init();}
    MfcatsSpline(std::string functionString, MemorySynchronizerPtr mem)
        : DerivableSpline(functionString, mem) {init();}

    ~MfcatsSpline(){}

    typedef std::pair<double, double> ControlPoint;
    typedef std::pair<double, double> Knot;

    // unsigned getOrder() { return boost::numeric_cast<unsigned>(spline.order);
    // }

    const auto &getControlPoints() { return controlPoints; }
    std::vector<Knot> getKnots();

    virtual double computeFunction(const double &x);
    std::string getOrderOption() { return "Order Option"; }

    // For now have a default for all splines just to compile, but throw an
    // exception is actually used.
    virtual double computeDerivative(double /*t*/) {
        throw XDEException(
            "Derivative function not yet implemented for specific spline");
    }

    void initializeSpecific();
    virtual double estimateResultingObjectSize() { return 0; }

    static std::string s_getMethodName() { return "Mfcats B-Spline"; }
    std::string getMethodName() { return s_getMethodName(); }

    std::string getShortMethodDescription() {
        return s_getShortMethodDescription();
    }
    static std::string s_getShortMethodDescription() {
        return "Spline package implemented by Frings, Markus; Müller, Corinna; Spahn, Max; Susen, Christoph; Behr, Marek; Elgeti, Stefanie.";
    }

    std::string getLongMethodDescription() {
        return s_getLongMethodDescription();
    }
    static std::string s_getLongMethodDescription() {
        return "Spline package implemented by Frings, Markus; Müller, Corinna; Spahn, Max; Susen, Christoph; Behr, Marek; Elgeti, Stefanie.";
    }

    static std::vector<std::string> s_getMethodAttributes() {return std::vector<std::string>();}

   private:
    double firstPoint;
    double lastPoint;

    std::shared_ptr<spl::BSpline<1>>
        b_spline_ptr;  //(knot_vector_ptr, degree, control_points);


    std::shared_ptr<boost::math::cubic_b_spline<double>> boost_spline;

   protected:
    std::vector<ControlPoint> controlPoints;
    std::vector<std::pair<double, double>> timeToCoord;
};

#endif  // MFCATS_SPLINE_H