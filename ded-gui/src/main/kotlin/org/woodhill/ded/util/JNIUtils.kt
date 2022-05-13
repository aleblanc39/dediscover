package org.woodhill.ded.util

import edu.rochester.urmc.cbim.jni.JGeneralParameter

/**
Utility function to extract info from old jni class into format expected in the kotlin classes
 **/


fun getControlParametersMap(controlParams: Array<JGeneralParameter>): Map<String, String> {
    val ret = mutableMapOf<String, String>()

    for (param in controlParams) {
        ret[param.name] = param.value.toString()
    }
    return ret
}

