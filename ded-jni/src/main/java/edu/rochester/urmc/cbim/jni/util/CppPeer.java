
package edu.rochester.urmc.cbim.jni.util;

import edu.rochester.urmc.cbim.jni.exception.XDESystemException;

/**
 * Code originally copied from thread
 * 
 * http://forum.java.sun.com/thread.jspa?threadID=5201404&messageID=9802058
 * 
 * then added some new material.
 * 
 * This class establishes a relationship between a java class and a
 * corresponding C++ class. Classes derived from Cpp will define a pointer to a
 * C++ object.
 * 
 * Updated version: Instead of the Java object storing a pointer to a C++ object
 * it will store an index into a C++ singleton class (NativeObjetManager) which
 * will handle the pointers to native object through the use of boost smart
 * pointers, ensuring that the native objects are destroyed when no longer in
 * use, but not before.
 */

abstract public class CppPeer {
	private long nativeObjectIndex; // Index used in shared ptr array
	public long basePtr; // value of raw c++ pointer to base object. Really needed here?

	public CppPeer() {
		nativeObjectIndex = -1;
	}
	// Called from C++. Needed
	void setBasePtr(long ptr) {
		basePtr = ptr;
	}

	public void setObjectIndex(long ptr) {
		this.nativeObjectIndex = ptr;
	}


	/*
	 * (non-Javadoc)
	 * 
	 * Perform comparison between two object. Returns true if
	 * this is the same java object, or if both objects
	 * refer to the same c++ object.
	 * 
	 * 
	 * @see java.lang.Object#equals(java.lang.Object)
	 */
	@Override
	public boolean equals(Object obj) {
		if (this == obj) {
			return true;
		}
		if (obj == null) {
			return false;
		}
		
		
		if (getClass() != obj.getClass()) {
			return false;
		}
		CppPeer other = (CppPeer) obj;
		
		if (nativeObjectIndex == other.nativeObjectIndex) {
			return true;
		}
		return this.basePtr == ((CppPeer) obj).basePtr;
	}

	@Override
	public String toString() {
		return getClass().getName() + "(" + this.nativeObjectIndex + ")";
	}

	public long getObjectIndex() {
		if (nativeObjectIndex < 0) {
			throw new XDESystemException("Got a negative index");
		}
		return this.nativeObjectIndex;
	}

	@Override
	protected void finalize() {
	    /**
	     * A native index less than 0 will occur in a buggy program. Just 
	     * conveniently ignore it for now and hope it works.
	     */
		if (this.nativeObjectIndex < 0) {
			System.err.println("*** Negative index for object of class: "
					+ this.getClass().toString());
			
		}
		releaseNativeObject(this.nativeObjectIndex);
	}

	private native void releaseNativeObject(long objectIndex);

}
