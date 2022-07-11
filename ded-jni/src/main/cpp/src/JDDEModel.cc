#include <JNIUtil.h>
#include <base/XDECSVFile.h>
#include <base/XDEException.h>
#include <edu_rochester_urmc_cbim_jni_JDDEModel.h>
#include <mapping/MappingFormula.h>
#include <model/XDEModel.h>
#include <parser/ParserException.h>

#include <memory>
#include <iostream>
#include <string>

using namespace std;

/*
 * Class:     edu_rochester_urmc_cbim_jni_JDDEModel
 * Method:    createModel
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_edu_rochester_urmc_cbim_jni_JDDEModel_createModel(
    JNIEnv *env, jobject obj, jstring modelString) {
    try {
        const char *str = env->GetStringUTFChars(modelString, NULL);
        ModelPtr model(new XDEModel(string(str)));
        assignTheNativeObject(env, obj, model);
        env->ReleaseStringUTFChars(modelString, str);
    } catch (ParserException &e) {
        throwParserException(env, e.getMsg());
    }
    JNI_EXCEPTIONS;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_JDDEModel
 * Method:    getMacroNames
 * Signature: ()[Ljava/lang/String;
 *
 * Returns the names of the macros defined in the model.
 *
 */
JNIEXPORT jobjectArray JNICALL
Java_edu_rochester_urmc_cbim_jni_JDDEModel_getMacroNames(JNIEnv *env,
                                                         jobject obj) {
    try {
        ModelPtr model = boost::any_cast<ModelPtr>(getNativePointer(env, obj));
        vector<string> macroNames = model->getMacroNames();
        return createJavaVector(env, macroNames);
    }
    JNI_EXCEPTIONS;
    return NULL;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_JDDEModel
 * Method:    getEquations
 * Signature: ()Ljava/lang/String;
 */
// JNIEXPORT jstring JNICALL
// Java_edu_rochester_urmc_cbim_jni_JDDEModel_getEquations(JNIEnv *env, jobject
// obj)
// {
// 	try
// 	{
// 		ModelPtr model = boost::any_cast<ModelPtr>(getNativePointer(env,
// obj)); 		return
// env->NewStringUTF((model->getEquationsString()).c_str());
// 	}
// 	JNI_EXCEPTIONS;
// 	return NULL;
// }

/*
 * Class:     edu_rochester_urmc_cbim_jni_JDDEModel
 * Method:    getNbDelayExpressions
 * Signature: ()I
 */

// TODO replace with boolean, which is what it is used for.

JNIEXPORT jint JNICALL
Java_edu_rochester_urmc_cbim_jni_JDDEModel_getNbDelayExpressions(JNIEnv *env,
                                                                 jobject obj) {
    try {
        ModelPtr model = boost::any_cast<ModelPtr>(getNativePointer(env, obj));
        if (model.get() != NULL)
            return (boost::polymorphic_cast<XDEModel *>(model.get()))
                           ->containsDelays()
                       ? 1
                       : 0;
        throwXDEException(env, "The model is null.");
        return -1;
    }
    JNI_EXCEPTIONS;
    return 0;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_JDDEModel
 * Method:    computeMacroValues_nat
 * Signature: ([D[[D[DDD)Ledu/rochester/urmc/cbim/jni/JXDECSVFile;
 */
JNIEXPORT jobject JNICALL
Java_edu_rochester_urmc_cbim_jni_JDDEModel_computeMacroValues_1nat(
    JNIEnv *env, jobject obj, jdoubleArray timepoints,

    // TODO AL 2021 Check if all really necessary
    // Initializing timepoints?
    // Initialize TVFunction and Covariates?
    // processTimepoints??
    jobjectArray dependentVarValues, jdoubleArray _modelParamValues, jdouble t0,
    jdouble tend) {
    try {
        ModelPtr model = boost::any_cast<ModelPtr>(getNativePointer(env, obj));
        auto timepts = createDoubleVector(env, timepoints);
        TDoubleVector modelParamValues =
            createTDoubleVector(env, _modelParamValues);
        TDoubleMatrix *depVarValues =
            createDoubleMatrix(env, dependentVarValues);
        vector<string> allNames = model->getMacroNames();

        model->setModelParameterValues(modelParamValues);

        // Initialize timepoints. Just really need first and last. Will do for
        // now. AL
        TDoubleVector timepoints(2);
        timepoints[0] = boost::numeric_cast<double>(t0);
        timepoints[1] = boost::numeric_cast<double>(tend);

        // What does this do???
        model->processTimepoints(timepoints);
        //		void XDEModel::processTimepoints(const TDoubleVector
        //&timepoints) {
        //            memorySynchronizer->updateMemory("t0", timepoints[0]);
        //            memorySynchronizer->updateMemory("tend",
        //            timepoints[timepoints.size() - 1]);
        //        }
        //        Still, why?

        // Initialize the time varying functions. May not have been done
        // previously.
        model->initializeTVFunctions();
        model->initializeCovariates();
        TDoubleMatrix allValues =
            model->computeMacrosAtTimepoints(timepts, *depVarValues);
        vector<string> covNames = model->getCovariateNames();
        if (covNames.size() > 0) {
            unsigned nbMacros = model->getMacroNames().size();
            allNames.insert(allNames.end(), covNames.begin(), covNames.end());
            TDoubleMatrix covValues =
                model->computeCovariatesAtTimepoints(timepts);
            allValues.resize(timepts.size(), allNames.size());

            for (unsigned i = 0; i < timepts.size(); i++) {
                for (unsigned j = nbMacros; j < allNames.size(); j++) {
                    allValues(i, j) = covValues(i, j - nbMacros);
                }
            }
        }

        XDECSVFilePtr result(new XDECSVFile(allNames, timepts, allValues));
        jobject jresult = createDefaultJavaObject(
            env, "edu/rochester/urmc/cbim/jni/JXDECSVFile", result);
        XDE_ASSERT(jresult != NULL);
        delete depVarValues;
        return jresult;
    }
    JNI_EXCEPTIONS;
    return NULL;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_JDDEModel
 * Method:    computeFormulas_nat
 * Signature:
 * ([D[[D[D[Ljava/lang/String;)Ledu/rochester/urmc/cbim/jni/JXDECSVFile;
 */
JNIEXPORT jobject JNICALL
Java_edu_rochester_urmc_cbim_jni_JDDEModel_computeFormulas_1nat(
    JNIEnv *env, jobject obj, jdoubleArray timepoints,
    jobjectArray dependentVarValues, jdoubleArray _modelParamValues,
    jobjectArray _formulas) {
    try {
        std::cerr << "Computing the formulas\n";
        ModelPtr model = boost::any_cast<ModelPtr>(getNativePointer(env, obj));
        auto timepts = createDoubleVector(env, timepoints);
        TDoubleVector modelParamValues =
            createTDoubleVector(env, _modelParamValues);
        TDoubleMatrix *depVarValues =
            createDoubleMatrix(env, dependentVarValues);
        vector<string> formulas = createStringVector(env, _formulas);

        model->setModelParameterValues(modelParamValues);
        vector<MappingFormula> mappedFormulas;
        vector<string> vars = model->getAllVariables();

        for (auto f : formulas) {
            mappedFormulas.push_back(MappingFormula(vars, f, model->getMemorySynchronizer()));
        }

        std::cerr << "Creating the results\n";
        TDoubleMatrix results(timepts.size(), mappedFormulas.size());
        TDoubleVector rowData(depVarValues->size1());
        
        for (auto i = 0; i < results.size1(); i++) {
            for (auto j = 0; j < depVarValues->size2(); j++) {
                rowData(j) = (*depVarValues)(i, j);
            }
            model->setDependentVariableValues(rowData);
            model->setTimeVariableValue(timepts[i]);
            model->computeMacros(timepts[i]);
            
            // currently computeMacros will compute the covariates.
            //model-> computeCovariatesAtTimepoints(timepts[i]);
            
            
            
            for (auto j = 0; j < results.size2(); j++) {
                results(i, j) = mappedFormulas[j].compute();
            }
        }

        XDECSVFilePtr result(new XDECSVFile(formulas, timepts, results));
        jobject jresult = createDefaultJavaObject(
            env, "edu/rochester/urmc/cbim/jni/JXDECSVFile", result);

        delete depVarValues;
        return jresult;

    }
    JNI_EXCEPTIONS;
    return NULL;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_JDDEModel
 * Method:    getInitialConditions
 * Signature: ()[Ledu/rochester/urmc/cbim/jni/JInitialCondition;
 */
JNIEXPORT jobjectArray JNICALL
Java_edu_rochester_urmc_cbim_jni_JDDEModel_getInitialConditions(JNIEnv *env,
                                                                jobject obj) {
    try {
        ModelPtr model = boost::any_cast<ModelPtr>(getNativePointer(env, obj));
        vector<ModelSymbolPtr> ms;
        ms.reserve(model->getDependentVariables().size());
        for (DependentVariablePtr p : model->getDependentVariables()) {
            ms.push_back(std::dynamic_pointer_cast<DependentVariable>(p));
        }
        return createJavaSmartPtrArray(
            env, ms, "edu/rochester/urmc/cbim/jni/JInitialCondition");
    }
    JNI_EXCEPTIONS;
    return NULL;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_JDDEModel
 * Method:    getModelParameters
 * Signature: ()[Ledu/rochester/urmc/cbim/jni/JModelParameter;
 */
JNIEXPORT jobjectArray JNICALL
Java_edu_rochester_urmc_cbim_jni_JDDEModel_getModelParameters(JNIEnv *env,
                                                              jobject obj) {
    try {
        ModelPtr model = boost::any_cast<ModelPtr>(getNativePointer(env, obj));
        vector<ModelSymbolPtr> ms;
        ms.reserve(model->getModelParameters().size());
        for (ModelParameterPtr p : model->getModelParameters()) {
            ms.push_back(std::dynamic_pointer_cast<ModelParameter>(p));
        }
        return createJavaSmartPtrArray(
            env, ms, "edu/rochester/urmc/cbim/jni/JModelParameter");
    }
    JNI_EXCEPTIONS;
    return NULL;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_JDDEModel
 * Method:    getMacros
 * Signature: ()[Ledu/rochester/urmc/cbim/jni/JMacro;
 *
 * Aug 2012: Updating function to reflect that JMacro now inherits
 * JIntermediateSymbol
 */

JNIEXPORT jobjectArray JNICALL
Java_edu_rochester_urmc_cbim_jni_JDDEModel_getMacros(JNIEnv *env, jobject obj) {
    try {
        ModelPtr model = boost::any_cast<ModelPtr>(getNativePointer(env, obj));
        vector<IntermediateSymbolPtr> is;
        is.reserve(model->getMacros().size());
        for (MacroPtr p : model->getMacros()) {
            is.push_back(std::dynamic_pointer_cast<Macro>(p));
        }
        return createJavaSmartPtrArray(env, is,
                                       "edu/rochester/urmc/cbim/jni/JMacro");
    }
    JNI_EXCEPTIONS;
    return NULL;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_JDDEModel
 * Method:    getCovariates
 * Signature: ()[Ledu/rochester/urmc/cbim/jni/JCovariate;
 */
JNIEXPORT jobjectArray JNICALL
Java_edu_rochester_urmc_cbim_jni_JDDEModel_getCovariates(JNIEnv *env,
                                                         jobject obj) {
    try {
        ModelPtr model = boost::any_cast<ModelPtr>(getNativePointer(env, obj));
        vector<IntermediateSymbolPtr> is;
        is.reserve(model->getCovariates().size());
        for (CovariatePtr p : model->getCovariates()) {
            is.push_back(std::dynamic_pointer_cast<Covariate>(p));
        }
        return createJavaSmartPtrArray(
            env, is, "edu/rochester/urmc/cbim/jni/JCovariate");
    }
    JNI_EXCEPTIONS;
    return NULL;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_JDDEModel
 * Method:    nat_getErrorMessages
 * Signature: ()[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL
Java_edu_rochester_urmc_cbim_jni_JDDEModel_nat_1getErrorMessages(JNIEnv *env,
                                                                 jobject obj) {
    try {
        ModelPtr model = boost::any_cast<ModelPtr>(getNativePointer(env, obj));
        ErrorMessageList errors = model->getErrorMessages();
        vector<string> returnedErrors;
        for (ErrorMessageType err : errors) {
            returnedErrors.push_back(boost::lexical_cast<string>(err.first) +
                                     ':' + err.second);
        }
        return createJavaVector(env, returnedErrors);
    }
    JNI_EXCEPTIONS;
    return NULL;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_JDDEModel
 * Method:    validateFormula
 * Signature: (Ljava/lang/String;)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL
Java_edu_rochester_urmc_cbim_jni_JDDEModel_validateFormula(JNIEnv *env,
                                                           jobject obj,
                                                           jstring formula) {
    try {
        const char *str = env->GetStringUTFChars(formula, NULL);
        ModelPtr model = boost::any_cast<ModelPtr>(getNativePointer(env, obj));
        ErrorMessageList errors = model->validateFormula(str);
        vector<string> returnedErrors;
        for (ErrorMessageType err : errors) {
            returnedErrors.push_back(err.second);
        }
        return createJavaVector(env, returnedErrors);
    }
    JNI_EXCEPTIONS;
    return NULL;
}
