#ifndef SOLVERFACTORY_H
#define SOLVERFACTORY_H


#include <algorithms/AlgorithmTemplates.h>

#include <solver/SundialsSolverWrapper.h>
#include <solver/SambineDDEWrapper.h>


typedef AlgorithmFactoryTemplate<XDESolverWrapper, 
    SambineDDEWrapper,
    SundialsSolverWrapper> SolverFactory;

#endif /* SOLVERFACTORY_H */
