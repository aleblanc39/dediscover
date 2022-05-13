
package edu.rochester.urmc.cbim.jni;

import edu.rochester.urmc.cbim.jni.exception.XDEException;

import java.util.*;

public class JModelSymbol extends XDEBase implements MappableSymbol {

    private String name;

    // Make this constructor protected since it has derived classes.
    protected JModelSymbol() {
    }

    private native String _getName();

    public String getName() {
        if (name == null) {
            name = _getName();
        }
        return name;
    }

    public String getDisplayName() {
        return getName();
    }

    public native int getFirstOccurenceLine();

    int occurenceOrder = Integer.MIN_VALUE;

    /**
     * Get the local copy of the occurrence order
     *
     * @return
     */
    public int getJOccurenceOrder() {
        if (occurenceOrder == Integer.MIN_VALUE) {
            occurenceOrder = getOccurenceOrder();
        }
        return occurenceOrder;
    }

    private native int getOccurenceOrder();


    public static JModelSymbol find(List<? extends JModelSymbol> list, String name) throws XDEException {
        if (list == null) {
            return null;
        }
        for (JModelSymbol symbol : list) {
            if (symbol.getName().equals(name)) {
                return symbol;
            }
        }
        return null;
    }

    @Override
    public boolean equals(Object s) {

        if (s == null)
            return false;
        if (!(s instanceof JModelSymbol))
            return false;
        JModelSymbol ms = (JModelSymbol) s;
        return (ms.getName().equals(name));
    }
}
