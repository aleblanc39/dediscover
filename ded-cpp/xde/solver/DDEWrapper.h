#ifndef DDEWRAPPER_H_
#define DDEWRAPPER_H_


#include <solver/XDESolverWrapper.h>
#include <base/XDEUtil.h>
#include <base/XDEMessage.h>



/**
 * \brief Base for all the class defining delayed-differential equations methods.
 */

class DDEWrapper : virtual public XDESolverWrapper {
 public:
    DDEWrapper(){}
    virtual ~DDEWrapper(){}

    void setHistoryParas(TDoubleVector newHistoryParasValue) {historyParasValue = newHistoryParasValue;}
    TDoubleVector getHistoryParas() {return historyParasValue;}
    void setDelayParas(TDoubleVector newDelaysParasValue) {delaysParasValue = newDelaysParasValue;}
    TDoubleVector getDelayParas() {return delaysParasValue;}

    static auto s_getMethodAttributes() {
        return std::vector<std::string>{"DDE"};
    }

    
 protected:

    TDoubleVector historyJump;
    TDoubleVector historyParasValue;
    TDoubleVector delaysParasValue;


};

#endif /*DDEWRAPPER_H_*/
