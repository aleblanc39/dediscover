package org.woodhill.ded.sbml

import org.sbml.jsbml.Species
import org.sbml.jsbml.xml.XMLNode
import java.util.regex.Pattern


object SBMLUtil {
    /**
     * Convert some function names that are different in muParser than they are
     * in SBML.Still looking for complete documentation, but for now, we
     * convert:
     *
     * (SBML --> muParser) log --> ln log10 --> log delay(X, Y) --> X[t-(Y)]
     *
     * Right now it will also convert xxlog(, but since xxxlog is not a defined
     * valid function it will be caught by DEDiscover. May add a prior step to
     * alidate all function beforehand.
     *
     * @param str
     * @return
     */
    fun convertEquationToMUParser(str: String): String {
        var str = str
        val logStr = "log\\s*\\("
        val log10Str = "log10\\s*\\("
        val logPattern = Pattern.compile(logStr)
        var m = logPattern.matcher(str)
        if (m.find()) str = m.replaceAll("ln(")
        val log10Pattern = Pattern.compile(log10Str)
        m = log10Pattern.matcher(str)
        if (m.find()) str = m.replaceAll("log(")
        return replaceDelays(str)
    }

    /*
     * Replace the delay(X, Y) calls with X[t-(Y)]. Had to create a special
     * function as Y could have the form a-(b+c) and regexps are not powerful
     * enough.
     */
    private fun replaceDelays(originalStr: String): String {
        var str = originalStr
        val fd = FunctionDetails(str, "delay")
        while (fd.completeCall !== "") {
            val args = fd.getArgs()
            if (args.size != 2) throw RuntimeException(
                "Wrong numbers of arguments in delay function"
            )
            val newStr = args[0] + "[t-(" + args[1] + ")]"
            str = str.replace(fd.completeCall, newStr)
        }
        return str
    }

    /**
     * Returns the celldesigner:class xml element. This element is in
     * <annotation> <celldesigner:extension> <celldesigner:speciesIdentity>
     * <celldesigner:class>
     *
     * @param s
     * @return
    </celldesigner:class></celldesigner:speciesIdentity></celldesigner:extension></annotation> */
    fun getCellDesignerClass(s: Species): String {
        val ann = s.annotation ?: return ""
        val annNode = ann.fullAnnotation ?: return ""
        val extensionNode = getXMLNode(annNode, "extension") ?: return ""
        val speciesIdentity = getXMLNode(extensionNode, "speciesIdentity") ?: return ""
        val classNode = getXMLNode(speciesIdentity, "class")
            ?: //	    System.err.println("Class node is null...");
            return ""

        return   classNode.getChildAt(0).toXMLString()

    }

    private fun getXMLNode(node: XMLNode, xmltag: String): XMLNode? {
        for (i in 0 until node.childCount) {
            val childNode = node.getChildAt(i)
            //	    System.err.println("Looking for extension node. Found ++"
//		    + childNode.getName() + "++");
            if (childNode.name == xmltag) {
//		System.err.println("Returning with child node");
                return childNode
            }
        }
        return null
    }
}
