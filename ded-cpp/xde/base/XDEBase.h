#ifndef XDEBASE_H_
#define XDEBASE_H_

#include <base/XDEProgrammingException.h>
#include <memory>
#include <string>


/** The following is copied from the mathstat package. Since these are
 * not related to the rest of mathstat and can be used in situations
 * that are not related to mathstat it makes sense to define them
 * locally.
 */


class XDEBase;

typedef std::shared_ptr<XDEBase> XDEBasePtr;

/**
 * \brief Base class to most XDE class.
 *
 * This class is the base for most XDE classes
 */

class XDEBase {
   public:
    virtual ~XDEBase(){};
    

   protected:
   private:
};

#endif /*XDEBASE_H_*/
