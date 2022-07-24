
package edu.rochester.urmc.cbim.jni;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.Arrays;
import java.util.List;

import edu.rochester.urmc.cbim.jni.exception.ParameterRangeException;
import edu.rochester.urmc.cbim.jni.exception.XDEException;

public abstract class Algorithm  extends Threadable {


    final static String method_name = "s_getAvailableMethods";

    public static <T extends Algorithm> List<String> getAvailableMethods(Class<T> clazz) throws Exception{
        try {
            Method m = clazz.getMethod(method_name);
            String[] methods = (String[]) m.invoke(null);
            return Arrays.asList(methods);
        } catch (NoSuchMethodException ex) {
            System.err.println("No method named " + method_name + " defined for class " + clazz.getName());
            throw ex;
        } catch (IllegalAccessException e) {
            e.printStackTrace();
            throw e;
        } catch (InvocationTargetException e) {
            e.printStackTrace();
            throw e;
        }
    }



    final static String controlParamsMethodName = "s_getControlParameters";
    /**
     * Return the control parameters associated to the algorithm which is a subclass of class
     * @param clazz
     * @param name
     * @return
     * @param <T>
     * @throws Exception
     */
    public static <T extends Algorithm> List<JGeneralParameter> getControlParameters(Class<T> clazz, String name) throws Exception{
        try {
            Method m = clazz.getMethod(controlParamsMethodName);
            JGeneralParameter[] params = (JGeneralParameter []) m.invoke(name);
            return Arrays.asList(params);
        } catch (NoSuchMethodException ex) {
            System.err.println("No method named " + method_name + " defined for class " + clazz.getName());
            throw ex;
        } catch (IllegalAccessException e) {
            e.printStackTrace();
            throw e;
        } catch (InvocationTargetException e) {
            e.printStackTrace();
            throw e;
        }
    }


    public static native <T extends Algorithm>  String getMethodName(Class<T> clazz, String name) throws XDEException;
    public static native <T extends Algorithm> String getShortMethodDescription(Class<T> clazz, String name) throws XDEException;
    public static native <T extends Algorithm> String getLongMethodDescription(Class<T> clazz, String name) throws XDEException;
    public static native <T extends Algorithm> String[] getMethodAttributes(Class<T> clazz, String name) throws XDEException;


//    public static <T extends Algorithm> T createMethod(Class<T> clazz, String name) throws XDEException {
//        try {
//            Class[] parameterTypes = {String.class};
//            return clazz.getConstructor(parameterTypes).newInstance(name);
//        } catch (NoSuchMethodException | InstantiationException | IllegalAccessException | InvocationTargetException ex) {
//            ex.printStackTrace();
//            throw new XDEException(ex.getMessage());
//        }
//    }



    /**
     * Will leave room for getControlParameters to ignore some of the parameters by allowing it
     * to be redefined in the subclass. The only use for now is to allow the GUI code to call getControlParameters() and not
     * return the 3 time parameters, allowing the JNI code to use getAllControlParameters to access the time parameters.
     *
     */

}
