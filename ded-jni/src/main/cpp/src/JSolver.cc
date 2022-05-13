#include <JNIUtil.h>
#include <base/StopThread.h>
#include <base/XDEException.h>
#include <base/XDEUtil.h>
#include <edu_rochester_urmc_cbim_jni_JSolver.h>
#include <model/XDEModel.h>
#include <solver/SolverFactory.h>
#include <solver/SolverResult.h>

#include <boost/cast.hpp>
#include <cassert>
#include <iostream>
using namespace std;

// TODO Should implement all the factories as singletons. Can probably
// be done in templated class so needs to be done only once.

static SolverFactory solverFactory;

/*
 * Class:     edu_rochester_urmc_cbim_jni_JSolver
 * Method:    s_getAvailableMethods
 * Signature: ()[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL
Java_edu_rochester_urmc_cbim_jni_JSolver_s_1getAvailableMethods(JNIEnv *env,
                                                                jclass cls) {
    return getAvailableMethods(env, solverFactory);
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_JSolver
 * Method:    solveEqns
 * Signature:
 * (Ledu/rochester/urmc/cbim/jni/JDDEModel;[D[D)Ledu/rochester/urmc/cbim/jni/SolverResult;
 *
 */
JNIEXPORT jobject JNICALL
Java_edu_rochester_urmc_cbim_jni_JSolver_solveEqns__Ledu_rochester_urmc_cbim_jni_JDDEModel_2_3D_3D(
    JNIEnv *env, jobject obj, jobject jmodel, jdoubleArray modelParameterValues,
    jdoubleArray initialConditionValues) {
    try {
        SolverPtr solver =
            boost::any_cast<SolverPtr>(getNativePointer(env, obj));
        ModelPtr model =
            boost::any_cast<ModelPtr>(getNativePointer(env, jmodel));
        TDoubleMatrix yout;
        TDoubleVector tout;
        TDoubleVector mp = createTDoubleVector(env, modelParameterValues);
        TDoubleVector ic = createTDoubleVector(env, initialConditionValues);


        model->initializeCovariates();
        solver->solve(model, mp, ic, tout, yout);

        vector<string> variableNames =
            getAllNames(model->getDependentVariables());
        SolverResultPtr solverResult(
            new SolverResult(variableNames, tout, yout));

        jobject jresult = createDefaultJavaObject(
            env, "edu/rochester/urmc/cbim/jni/SolverResult", solverResult);
        return jresult;

    } catch (StopThread) {
    std::cerr << "JSolver.cc::solverEqns1 StopThread thrown\n";
        throwInterruptException(env, "Interrupt from JSolver");
        return NULL;
    }
    JNI_EXCEPTIONS;
    return NULL;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_JSolver
 * Method:    solveEqns
 * Signature:
 * (Ledu/rochester/urmc/cbim/jni/JDDEModel;[D[D[D)Ledu/rochester/urmc/cbim/jni/SolverResult;
 *
 * Solves the equations for the case where we explicitely gives the timepoints
 * for which data must be returned. Could merge both methods, having tspan of
 * size 0 when using solver start, end, timesteps.
 */
JNIEXPORT jobject JNICALL
Java_edu_rochester_urmc_cbim_jni_JSolver_solveEqns__Ledu_rochester_urmc_cbim_jni_JDDEModel_2_3D_3D_3D(
    JNIEnv *env, jobject obj, jobject jmodel, jdoubleArray modelParameterValues,
    jdoubleArray initialConditionValues, jdoubleArray tspanUsed) {
    try {
        SolverPtr solver =
            boost::any_cast<SolverPtr>(getNativePointer(env, obj));
        ModelPtr model =
            boost::any_cast<ModelPtr>(getNativePointer(env, jmodel));
        TDoubleMatrix yout;
        TDoubleVector tout;
        TDoubleVector mp = createTDoubleVector(env, modelParameterValues);
        TDoubleVector ic = createTDoubleVector(env, initialConditionValues);
        TDoubleVector tsp = createTDoubleVector(env, tspanUsed);

        // Explicitely call the initialization here since it is not called from
        // the solve() method in order to prevent the initialization to be
        // called at each iteration of the estimation.
        model->initializeCovariates();

        solver->setTSpan(tsp);

        solver->solve(model, mp, ic, tout, yout);
        vector<string> variableNames =
            getAllNames(model->getDependentVariables());
        SolverResultPtr solverResult(
            new SolverResult(variableNames, tout, yout));

        jobject jresult = createDefaultJavaObject(
            env, "edu/rochester/urmc/cbim/jni/SolverResult", solverResult);
        return jresult;

    } catch (StopThread) {
        std::cerr << "JSolver.cc::solverEqns1 StopThread thrown\n";

        throwInterruptException(env, "Interrupt from JSolver");
        return NULL;
    }
    JNI_EXCEPTIONS;
    return NULL;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_JSolver
 * Method:    setTSpan
 * Signature: ([D)V
 */
JNIEXPORT void JNICALL Java_edu_rochester_urmc_cbim_jni_JSolver_setTSpan(
    JNIEnv *env, jobject obj, jdoubleArray values) {
    try {
        SolverPtr solver =
            boost::any_cast<SolverPtr>(getNativePointer(env, obj));
        TDoubleVector vals = createTDoubleVector(env, values);
        if (vals.size() == 0)
            solver->setTSpan();
        else
            solver->setTSpan(vals);
    }
    JNI_EXCEPTIONS;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_JSolver
 * Method:    getStartTime
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL Java_edu_rochester_urmc_cbim_jni_JSolver_getStartTime(
    JNIEnv *env, jobject obj) {
    try {
        SolverPtr solver =
            boost::any_cast<SolverPtr>(getNativePointer(env, obj));
        assert(solver.get() != NULL);
        return boost::numeric_cast<jdouble>(solver->getStartTime());
    }
    JNI_EXCEPTIONS;
    return boost::numeric_cast<double>(XDEUtil::XDE_NAN);
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_JSolver
 * Method:    getStopTime
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_edu_rochester_urmc_cbim_jni_JSolver_getStopTime(JNIEnv *env, jobject obj) {
    try {
        SolverPtr solver =
            boost::any_cast<SolverPtr>(getNativePointer(env, obj));
        assert(solver.get() != NULL);
        return boost::numeric_cast<jdouble>(solver->getStopTime());
    }
    JNI_EXCEPTIONS;
    return boost::numeric_cast<double>(XDEUtil::XDE_NAN);
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_JSolver
 * Method:    getStepSize
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_edu_rochester_urmc_cbim_jni_JSolver_getStepSize(JNIEnv *env, jobject obj) {
    try {
        SolverPtr solver =
            boost::any_cast<SolverPtr>(getNativePointer(env, obj));
        assert(solver.get() != NULL);
        return boost::numeric_cast<jdouble>(solver->getStepSize());
    }
    JNI_EXCEPTIONS;
    return boost::numeric_cast<double>(XDEUtil::XDE_NAN);
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_JSolver
 * Method:    setStartTime
 * Signature: (D)V
 */
JNIEXPORT void JNICALL Java_edu_rochester_urmc_cbim_jni_JSolver_setStartTime(
    JNIEnv *env, jobject obj, jdouble d) {
    try {
        SolverPtr solver =
            boost::any_cast<SolverPtr>(getNativePointer(env, obj));
        assert(solver.get() != NULL);
        solver->setStartTime(boost::numeric_cast<double>(d));
    }
    JNI_EXCEPTIONS;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_JSolver
 * Method:    setStopTime
 * Signature: (D)V
 */
JNIEXPORT void JNICALL Java_edu_rochester_urmc_cbim_jni_JSolver_setStopTime(
    JNIEnv *env, jobject obj, jdouble d) {
    try {
        SolverPtr solver =
            boost::any_cast<SolverPtr>(getNativePointer(env, obj));
        assert(solver.get() != NULL);
        solver->setStopTime(boost::numeric_cast<double>(d));
    }
    JNI_EXCEPTIONS;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_JSolver
 * Method:    setStepSize
 * Signature: (D)V
 */
JNIEXPORT void JNICALL Java_edu_rochester_urmc_cbim_jni_JSolver_setStepSize(
    JNIEnv *env, jobject obj, jdouble d) {
    try {
        SolverPtr solver =
            boost::any_cast<SolverPtr>(getNativePointer(env, obj));
        assert(solver.get() != NULL);
        solver->setStepSize(boost::numeric_cast<double>(d));
    }
    JNI_EXCEPTIONS;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_JSolver
 * Method:    estimatedMemoryUse
 * Signature: (Ledu/rochester/urmc/cbim/jni/JDDEModel;)J
 */
JNIEXPORT jlong JNICALL
Java_edu_rochester_urmc_cbim_jni_JSolver_estimatedMemoryUse(JNIEnv *env,
                                                            jobject obj,
                                                            jobject jmodel) {
    try {
        SolverPtr solver =
            boost::any_cast<SolverPtr>(getNativePointer(env, obj));
        ModelPtr model =
            boost::any_cast<ModelPtr>(getNativePointer(env, jmodel));
        return (jlong)(solver->estimateMemoryUse(model));
    }
    JNI_EXCEPTIONS;
    return 0;
}


/*
 * Class:     edu_rochester_urmc_cbim_jni_JSolver
 * Method:    createSolver
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_edu_rochester_urmc_cbim_jni_JSolver_createSolver(
    JNIEnv *env, jobject obj, jstring jsolverName) {
    try {
        const char *str = env->GetStringUTFChars(jsolverName, NULL);
        SolverPtr newSolver(solverFactory.createMethod(string(str)));
        assignTheNativeObject(env, obj, newSolver);
        env->ReleaseStringUTFChars(jsolverName, str);
    }
    JNI_EXCEPTIONS;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_JSolver
 * Method:    isAttributeDefined
 * Signature: (Ljava/lang/String;Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_rochester_urmc_cbim_jni_JSolver_isAttributeDefined(JNIEnv *env,
                                                            jclass cls,
                                                            jstring method,
                                                            jstring attribute) {
    return isAttributeDefined(env, solverFactory, method, attribute);
}
