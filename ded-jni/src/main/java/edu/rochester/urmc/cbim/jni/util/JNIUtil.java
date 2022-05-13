
package edu.rochester.urmc.cbim.jni.util;

import edu.rochester.urmc.cbim.jni.exception.OutOfMemoryException;

import java.util.List;
import java.util.stream.IntStream;

/**
 * Utility functions used by the jni package.
 */


public class JNIUtil {

    /** Will compare the available memory with the required
     * memory. Will run the garbage collector beforehand, and will
     * throw an OutOfMemoryException if there is not enough memory
     * available.
     */


    public static void validateMemoryRequirements(long memoryRequired) throws OutOfMemoryException {
        validateMemoryRequirement(memoryRequired, null);
    }

    public static void validateMemoryRequirement(long memoryRequired, String msg)
            throws OutOfMemoryException {
        System.gc();
        Runtime runtime = Runtime.getRuntime();
        long maxMem = runtime.maxMemory();
        long freeMemory = runtime.freeMemory();



        if (msg == null)
            msg = "There will not be enough memory to perform the operation.";

        if (memoryRequired > freeMemory)
            throw new OutOfMemoryException
                    (msg,
                            freeMemory, maxMem, memoryRequired);
    }

	public static double[] rawDoubleArray(List<Double> l) {
		double [] rawValues = new double[l.size()];
		IntStream.range(0, l.size()).forEach(i->
				rawValues[i] = l.get(i));
		return rawValues;
	}

	/**
	 * Create a matrix where the number of columns is the size of darray. It is assumed that
	 * all the entries of darray have the same size, and this is going to be the number of rows
	 *
	 * @param darray
	 * @return
	 */
	public static double[][] rawDoubleMatrix(List<List<Double>> darray) {
    	int nbRows = darray.get(0).size();
		double rawData[][] = new double[darray.get(0).size()][darray.size()];
		IntStream.range(0, nbRows).forEach( i-> {
			IntStream.range(0, darray.size()).forEach( j-> {
				rawData[i][j] = darray.get(j).get(i);
			});
		});
		return rawData;
	}
}