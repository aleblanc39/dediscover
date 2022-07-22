#ifndef XDEALGORITHM_H_
#define XDEALGORITHM_H_

#include <algorithms/GenericParameter.h>
#include <base/ThreadInterface.h>
#include <base/XDEMessage.h>
#include <parameters/ParameterSet.h>

#include <boost/cast.hpp>
#include <string>



/**
 * \brief Base virtual class for all the algorithm classes.
 *
 * All the algorithm classes (solvers, estimators, optimizers) extend this
 * class.
 */

//namespace xde_algorithm {

class XDEAlgorithm : public ThreadInterface {
   public:
    XDEAlgorithm() {}
    virtual ~XDEAlgorithm() {}

   protected:
    void logAlgorithmInfo(XDEMessage::MsgLevel level = XDEMessage::INFO);
    XDEAlgorithm(const XDEAlgorithm &a);
    std::vector<std::string> methodAttributes;

   private:
};

GeneralParameterPtr getCtrlParameter(
    const std::vector<GeneralParameterPtr> &params, const std::string &name);

double getDoubleParameterValue(const std::vector<GeneralParameterPtr> &params,
                               const std::string &paramName);
int getIntParameterValue(const std::vector<GeneralParameterPtr> &params,
                         const std::string &paramName); 

std::string getStringParameterValue(
    const std::vector<GeneralParameterPtr> &params,
    const std::string &paramName);

bool getBoolParameterValue(const std::vector<GeneralParameterPtr> &params,
                           const std::string &paramName); 

template <typename T>
void setParameterValue(const std::vector<GeneralParameterPtr> &params,
                       std::string &paramName, const T &newValue) {
    GeneralParameterPtr param = getCtrlParameter(params, paramName);
    if (param == NULL)
        throw XDEException("Trying to set value to unknown parameter: " +
                           paramName);
    GenericParameter<T> *p =
        boost::polymorphic_cast<GenericParameter<T> *>(param.get());
    p->setValue(newValue);
}
typedef std::shared_ptr<XDEAlgorithm> AlgorithmPtr;

#endif /*XDEALGORITHM_H_*/
