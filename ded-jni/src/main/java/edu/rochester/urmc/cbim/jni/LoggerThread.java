
package edu.rochester.urmc.cbim.jni;

import java.util.logging.Logger;

import edu.rochester.urmc.cbim.jni.exception.XDEException;

public class LoggerThread implements Runnable {

	public static native void setLoggingLevel(int level) throws XDEException;

	public static LoggerThread getLoggerThread(Logger logger) throws XDEException {
		if (threadRunning) {
			throw new XDEException("A logger thread is already running.");
		}

		return new LoggerThread(logger);
	}

	public void run() {
		try {
			loggerUsed.info("Reading messages from C++");
			readXDEMessages(loggerUsed);
		} catch (XDEException e) {
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public static void terminateLogger() {
		sendTerminationMessage();
		threadRunning = false;
	}

	protected LoggerThread(Logger logger) {
		loggerUsed = logger;
		threadRunning = true;
	}

	private static boolean threadRunning = false;
	private static Logger loggerUsed = null;

	private static native void sendTerminationMessage();

	private native void readXDEMessages(Logger logger) throws XDEException;

}
