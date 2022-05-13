
package edu.rochester.urmc.cbim.jni;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.Arrays;
import java.util.List;

import edu.rochester.urmc.cbim.jni.exception.ParameterRangeException;
import edu.rochester.urmc.cbim.jni.exception.XDEException;

public abstract class Algorithm  extends Threadable {

    // This variable will be used by the JNI code to set a pointer to
    // the C++ algorithm, object.
    private long algorithmPtr;

    public Algorithm() {
    }


    /**
     * AL 2020
     * Some java method wrappers around the native methods so I can mock the classes
     * @return
     * @throws XDEException
     */

    public String getjMethodName() {
        return getMethodName();
    }

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


    public native String getMethodName() throws XDEException;
    public native String getShortMethodDescription() throws XDEException;
    public native String getLongMethodDescription() throws XDEException;
    public native String[] getMethodAttributes() throws XDEException;

    public static <T extends Algorithm> T createMethod(Class<T> clazz, String name) throws XDEException {
        try {
            Class[] parameterTypes = {String.class};
            return clazz.getConstructor(parameterTypes).newInstance(name);
        } catch (NoSuchMethodException | InstantiationException | IllegalAccessException | InvocationTargetException ex) {
            ex.printStackTrace();
            throw new XDEException(ex.getMessage());
        }
    }

    public JGeneralParameter getControlParameter(String paramName)
            throws XDEException {
        JGeneralParameter controlParameters[] = getControlParameters();
        for (JGeneralParameter p : controlParameters) {
            if (p.getName().equals(paramName)) {
                return p;
            }
        }
        return null;
    }

    /**
     * Will leave room for getControlParameters to ignore some of the parameters by allowing it
     * to be redefined in the subclass. The only use for now is to allow the GUI code to call getControlParameters() and not
     * return the 3 time parameters, allowing the JNI code to use getAllControlParameters to access the time parameters.
     *
     */
    public JGeneralParameter[] getjControlParameters() {
        return getControlParameters();
    }

    public native JGeneralParameter[] getControlParameters();

    public void setDefaults() throws XDEException, ParameterRangeException {
        JGeneralParameter[] ctrlParas = getControlParameters();
        // ctrlParas should not be empty or null
        if ((ctrlParas != null) && (ctrlParas.length > 0)) {
            for (JGeneralParameter ctrlPara : ctrlParas) {
                ctrlPara.setValue(ctrlPara.getDefault());
            }
        }
    }
}
