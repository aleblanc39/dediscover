#include <estimator/EstimatorFactory.h>
#include <estimator/SolverBasedEstimator.h>

#include <memory>
#include "JNIUtil.h"
#include "edu_rochester_urmc_cbim_jni_JSolverBasedEstimator.h"

static EstimatorFactory estimatorFactory;
using namespace std;

/*
 * Class:     edu_rochester_urmc_cbim_jni_JSolverBasedEstimator
 * Method:    initializeEstimator
 * Signature: ()V
 * 
 * Initialize an estimator object.
 */
JNIEXPORT void JNICALL
Java_edu_rochester_urmc_cbim_jni_JSolverBasedEstimator_initializeEstimator(
    JNIEnv *env, jobject obj, jobject model, jobject solver, jobject objFcn, jobject optimizer, jdoubleArray timepoints,  jobjectArray formulas, jobjectArray data) {
    try {
        ModelPtr _model = boost::any_cast<ModelPtr>(getNativePointer(env, model));
        SolverPtr _solver = boost::any_cast<SolverPtr>(getNativePointer(env, solver));
        ObjectiveFunctionPtr _objFcn = boost::any_cast<ObjectiveFunctionPtr>(getNativePointer(env, objFcn));
        OptimizerPtr _optimizer = boost::any_cast<OptimizerPtr>(getNativePointer(env, optimizer));
        vector<string> _formulas = createStringVector(env, formulas);
        TDoubleMatrix *columnData = createDoubleMatrix(env, data);
        vector<double> timepointsUsed = createDoubleVector(env, timepoints);
        DataTableMappingPtr mapping(new DataTableMapping(timepointsUsed, _formulas, *columnData));
        SolverBasedEstimatorPtr estimator(new SolverBasedEstimator(_model, _solver, _objFcn, _optimizer, mapping));
        assignTheNativeObject(
            env, obj, std::static_pointer_cast<XDEEstimator>(estimator));

        delete columnData;
    }
    JNI_EXCEPTIONS;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_JSolverBasedEstimator
 * Method:    s_getAvailableMethods
 * Signature: ()[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL
Java_edu_rochester_urmc_cbim_jni_JSolverBasedEstimator_s_1getAvailableMethods(
    JNIEnv *env, jclass cls) {
    return getAvailableMethods(env, estimatorFactory);
}
