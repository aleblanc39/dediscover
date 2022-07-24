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
    void logAlgorithmInfo(XDEMessage::MsgLevel level = XDEMessage::INFO){}

   private:
};




typedef std::shared_ptr<XDEAlgorithm> AlgorithmPtr;

#endif /*XDEALGORITHM_H_*/
