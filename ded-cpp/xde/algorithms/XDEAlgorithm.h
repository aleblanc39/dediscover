#ifndef XDEALGORITHM_H_
#define XDEALGORITHM_H_

#include <algorithms/GenericParameter.h>
#include <base/XDEMessage.h>
#include <parameters/ParameterSet.h>
#include <base/ThreadInterface.h>

#include <boost/cast.hpp>
#include <string>

class XDEAlgorithm;

typedef std::shared_ptr<XDEAlgorithm> AlgorithmPtr;

/**
 * \brief Base virtual class for all the algorithm classes.
 *
 * All the algorithm classes (solvers, estimators, optimizers) extend this
 * class.
 */

class XDEAlgorithm :  public ThreadInterface {
   public:
    XDEAlgorithm();
    virtual ~XDEAlgorithm();

    /** Method name to by used by the factories */
    virtual std::string getMethodName() = 0;

    /** Short method description for display */
    virtual std::string getShortMethodDescription() = 0;

    virtual std::string getLongMethodDescription() = 0;

    virtual std::vector<std::string> getMethodAttributes() {
        return methodAttributes;
    }

    std::vector<GeneralParameterPtr> getControlParameters();
    std::vector<GeneralParameterPtr> getControlParameters() const;

    GeneralParameterPtr getCtrlParameter(std::string name);
    static GeneralParameterPtr getCtrlParameter(
        const std::vector<GeneralParameterPtr> params, std::string name);

    template <typename T>
    void setParameterValue(std::string paramName, T newValue);

    double getDoubleParameterValue(const std::string &paramName);
    int getIntParameterValue(const std::string &paramName);
    std::string getStringParameterValue(const std::string &paramName);
    bool getBoolParameterValue(const std::string &paramName);
    
   protected:
    std::vector<GeneralParameterPtr> controlParameters;

    void logAlgorithmInfo(XDEMessage::MsgLevel level = XDEMessage::INFO);

    XDEAlgorithm(const XDEAlgorithm &a);
    std::vector<std::string> methodAttributes;


    void addToControlParameters( GeneralParameterPtr p) {
        controlParameters.push_back(p);
    }

   private:
};

template <typename T>
void XDEAlgorithm::setParameterValue(std::string paramName, T newValue) {
    GeneralParameterPtr param = getCtrlParameter(paramName);
    if (param == NULL)
        throw XDEException("Trying to set value to unknown parameter: " +
                           paramName);
    GenericParameter<T> *p =
        boost::polymorphic_cast<GenericParameter<T> *>(param.get());
    p->setValue(newValue);
}


#endif /*XDEALGORITHM_H_*/
