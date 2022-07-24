#ifndef ODEWRAPPER_H_
#define ODEWRAPPER_H_

#include <solver/XDESolverWrapper.h>

/**
 * \brief Class at the bases of all the classes handling ODEs.
 *
 * virtual public to handle the diamond inheritance problem.
 *
 */

class ODEWrapper : virtual public XDESolverWrapper
{
public:
   ODEWrapper() {}
   virtual ~ODEWrapper() {}
   static std::vector<std::string> s_getMethodAttributes()
   {
      return {"ODE"};
   }
};

#endif /*ODEWRAPPER_H_*/
