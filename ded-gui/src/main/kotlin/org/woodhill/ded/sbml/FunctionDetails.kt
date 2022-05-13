package org.woodhill.ded.sbml


class FunctionDetails(val originalString: String, val functionName: String) {
    private val args = mutableListOf<String>()
    fun getArgs(): List<String> {
        return args
    }

    // completeCall will be used if the caller wants to replace the whole.
    var completeCall = ""

    private fun extractFunctionCall() {
        var str = originalString
        // OK Assume no space after function name.
        val start = str.indexOf("$functionName(")
        System.err.println(
            "replacing delays for " + str + " start = "
                    + start
        )
        if (start < 0) return

        // Walk through the string, extracting arguments. Will use matching
        // parenthesis count
        // to figure out where the start and end.
        str = str.substring(start)
        var index = str.indexOf('(') + 1
        var parenMatch = 1
        var currentArg = String()
        // index points to the first char past the first open paren
        while (parenMatch > 0 && index < str.length) {
            val c = str[index++]
            if (c == ',' && parenMatch == 1) {
                args.add(currentArg)
                currentArg = String()
            } else if (c == '(') {
                parenMatch++
                currentArg += c
            } else if (c == ')') {
                parenMatch--
                if (parenMatch == 0) {
                    // Could be a call with 0 arguments
                    if (currentArg.trim { it <= ' ' }.length > 0) {
                        args.add(currentArg)
                    }
                }
            } else {
                currentArg += c
            }
        }
        if (parenMatch > 0) throw RuntimeException("Parenthesis don't match")
        completeCall = str.substring(0, index)
    }

    init {
        extractFunctionCall()
    }
}


