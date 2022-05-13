project (xde CXX)
cmake_minimum_required(VERSION 3.0)
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".so" ".so.0" ".so.1" ".so.2" ".so.3" ".so.4" ".so.5" ".so.1.62.0")
include ("../xde-config.cmake")
message("bin dir for xde: ${PROJECT_BINARY_DIR}")


set (SUNDIALS_CVODE sundials_cvode)
set (SUNDIALS_NVECSERIAL sundials_nvecserial)
set (XDEFORTRAN  xde_fortran)
set (MUPARSER  muparser)
set (SPLINELIB splinelibspl)
set (SPLINEBAF splinelibbaf)

find_library(GFORTRAN gfortran PATHS /usr/lib/gcc/x86_64-linux-gnu/8 /usr/lib64 /usr/lib/gcc/x86_64-linux-gnu/9/)
find_library(LAPACK  NAMES lapack  PATHS /usr/lib/x86_64-linux-gnu/ /usr/lib64 /usr/lib)
find_library(BLAS NAMES blas PATHS /usr/lib/x86_64-linux-gnu/ /usr/lib /usr/lib64)

set (sources 
./mapping/MappingFormula.cc
./mapping/NewWorkingData.cc
./mapping/ParsingResults.cc
./mapping/ModelMappingInfoSet.cc
./mapping/MappingInfoSet.cc
./mapping/DataTableMapping.cc
./extra_parser_functions/TimeVaryingFunction.cc
./extra_parser_functions/BoostSpline.cc
./extra_parser_functions/MfcatsSpline.cc
./extra_parser_functions/LOCF.cc
./extra_parser_functions/TVFunctionMapping.cc
./extra_parser_functions/FortranSmoothingBSpline.cc
./statistics/EstimatorResult.cc
./statistics/NLSSummary.cc
./statistics/XDEConfidenceInterval.cc
./statistics/MathUtil.cc
./statistics/nls_summary.cc
./solver/DDEWrapper.cc
./solver/SolverResult.cc
./solver/ODEWrapper.cc
./solver/SundialsSolverWrapper.cc
./solver/SambineDDEWrapper.cc
./solver/XDESolverWrapper.cc
./base/XDECSVFile.cc
./base/XDEBase.cc
./base/AlgorithmResult.cc
./base/MemoryBank.cc
./base/XDEUtil.cc
./base/XDEMessage.cc
./base/ThreadInterface.cc
./estimator/SolverBasedEstimator.cc
./estimator/XDEEstimator.cc
./model/XDEModel.cc
./objectfunction/LeastSquare.cc
./objectfunction/DataGenerator.cc
./objectfunction/XDEObjectiveFunction.cc
./objectfunction/SolvingDataGenerator.cc
./objectfunction/ParameterMapping.cc
./parser/ProcessedModelExpression.cc
./parser/ModelParameter.cc
./parser/Covariate.cc
./parser/XDEParser.cc
./parser/IntermediateSymbol.cc
./parser/HistoryInterval.cc
./parser/ParseTreeComponents.cc
./parser/ParseTree.cc
./parser/DependentVariable.cc
./parser/Macro.cc
./parser/SpiritParser.cc
./parser/ModelSymbol.cc
./optimizer/DifferentialEvolution.cc
./optimizer/TrustRegion.cc
./optimizer/XDEOptimizer.cc
./optimizer/AdrianDE.cc
./algorithms/GeneralParameter.cc
./algorithms/XDEAlgorithm.cc
./algorithms/GenericParameter.cc
./algorithms/AlgorithmTemplates.cc
./parameters/EstimationSymbolAttributes.cc
./parameters/ParameterRangeException.cc
./expressions/TimeVaryingFunctionHandler.cc
./expressions/MuParserExpressionHandler.cc
./expressions/ExpressionHandler.cc
./expressions/MuParserExpressionEvaluator.cc
./expressions/ExpressionSetEvaluator.cc
./expressions/MemorySynchronizer.cc
./expressions/MemoryBankSynchronizer.cc
./expressions/DelayedVariable.cc
)

set_source_files_properties(${sources}
  PROPERTIES 
  COMPILE_FLAGS  "${EXTRA_OPTIONS}")


include_directories(
  .
  ../muparser/include
  ../cvode/include
  ../f90/include
  ../SplineLib/src/baf
  ../SplineLib/src/spl
  ../SplineLib/src/util

  ${PROJECT_BINARY_DIR}/../cvode/include
)

link_directories("${PROJECT_BINARY_DIR}/../cvode/src/cvode")
link_directories("${PROJECT_BINARY_DIR}/../cvode/src/nvec_ser")
link_directories("${PROJECT_BINARY_DIR}/../SplineLib/src/spl")
link_directories("${PROJECT_BINARY_DIR}/../SplineLib/src/baf")


add_library(xde  ${LIBRARY_TYPE} ${sources} )

target_link_libraries(xde ${EXTRA_OPTIONS}
  ${SPLINELIB}
  ${SPLINEBAF}
  ${SUNDIALS_CVODE}
  ${SUNDIALS_NVECSERIAL}
  ${XDEFORTRAN}
  ${BLAS}
  ${LAPACK}
  ${GFORTRAN}
  ${MUPARSER}
  ${BOOST_LIBRARIES}
#  ${CMATH}
)
message("Done with cmake.")
