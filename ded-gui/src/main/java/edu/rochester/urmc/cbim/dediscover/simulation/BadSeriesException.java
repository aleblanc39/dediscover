
package edu.rochester.urmc.cbim.dediscover.simulation;


public class BadSeriesException extends RuntimeException {

    /**
	 * 
	 */
    public BadSeriesException() {
    }

    /**
     * @param message
     */
    public BadSeriesException(String message) {
	super(message);
    }

    /**
     * @param cause
     */
    public BadSeriesException(Throwable cause) {
	super(cause);
    }

    /**
     * @param message
     * @param cause
     */
    public BadSeriesException(String message, Throwable cause) {
	super(message, cause);
    }

}
