
package edu.rochester.urmc.cbim.jni;

import java.util.logging.Level;
import java.util.logging.Logger;

import edu.rochester.urmc.cbim.jni.exception.XDEException;

public class XDELogger {
	public final static String CLASS_NAME = "XDELogger";

	public final static Logger LOGGER = Logger.getLogger(CLASS_NAME);

	public enum LogLevel {
		DEBUG5(9), DEBUG4(8), DEBUG3(7), DEBUG2(6), DEBUG1(5), DEBUG(4), INFO(3), WARNING(
				2), ERROR(1), NONE(0);

		private LogLevel(int intLevel) {
			this.intLevel = intLevel;
		}

		public int getLevel() {
			return intLevel;
		}

		private int intLevel;
	}

	public static void setLoggingLevel(LogLevel l) throws XDEException {
		if (loggerThread != null) {
			LoggerThread.setLoggingLevel(l.getLevel());
		}

	}

	/**
	 * Initilize the XDELogger with a Java Logger.
	 */
	public static void initializeLogger() throws XDEException {
		if (LOGGER.isLoggable(Level.INFO)) {
			LOGGER.info("Initializing XDELogger");
		}
		loggerThread = LoggerThread.getLoggerThread(LOGGER);
		thread = new Thread(loggerThread);
		thread.start();
		if (LOGGER.isLoggable(Level.INFO)) {
			LOGGER.warning("XDELogger spun up");
		}
		setLogLevel(LOGGER.getLevel());
	}

	
	    /**
	     * Initilize the XDELogger with a Java Logger.
	     */
	    public static void initializeLogger(Logger logger) throws XDEException {
		loggerThread = LoggerThread.getLoggerThread(logger);
		setLogLevel(logger.getLevel());
		thread = new Thread(loggerThread);
		thread.start();
	    }

	
	
	public static void terminateLogger() {
		if (loggerThread == null) {
			return;
		}
		LoggerThread.terminateLogger();
	}

	private static void setLogLevel(Level level) throws XDEException {
		if (level == Level.SEVERE) {
			LoggerThread.setLoggingLevel(LogLevel.ERROR.intLevel);
		} else if (level == Level.WARNING) {
			LoggerThread.setLoggingLevel(LogLevel.WARNING.intLevel);
		} else if (level == Level.INFO) {
			LoggerThread.setLoggingLevel(LogLevel.INFO.intLevel);
		} else if (level == Level.CONFIG) {
			LoggerThread.setLoggingLevel(LogLevel.DEBUG.intLevel);
		} else if (level == Level.FINE) {
			LoggerThread.setLoggingLevel(LogLevel.DEBUG1.intLevel);
		} else if (level == Level.FINER) {
			LoggerThread.setLoggingLevel(LogLevel.DEBUG2.intLevel);
		} else if ((level == Level.FINEST) || (level == Level.ALL)) {
			LoggerThread.setLoggingLevel(LogLevel.DEBUG5.intLevel);
		}
	}

	private static LoggerThread loggerThread;
	private static Thread thread;

}
