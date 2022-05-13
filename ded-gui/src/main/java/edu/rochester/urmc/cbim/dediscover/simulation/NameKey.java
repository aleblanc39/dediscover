
package edu.rochester.urmc.cbim.dediscover.simulation;

// TODO Document use of this class
public class NameKey {

    public final String key;
    public final String display;
    private String mappedSymbolName;

    /*
     * (non-Javadoc)
     *
     * @see java.lang.Object#hashCode()
     */
    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + ((key == null) ? 0 : key.hashCode());
        return result;
    }

    /*
     * (non-Javadoc)
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
        NameKey other = (NameKey) obj;
        if (key == null) {
            if (other.key != null) {
                return false;
            }
        } else if (!key.equals(other.key)) {
            return false;
        }
        return true;
    }

    public NameKey(){
        this ("", "");
    }

    public NameKey(String key, String display) {
        this.key = key;
        this.display = display;
    }

    public NameKey(String keyAndName) {
        this(keyAndName, keyAndName);
    }


    public NameKey(String key, String display, String modelSymbolName) {
        this(key, display);
        this.setMappedSymbolName(modelSymbolName);
    }

    @Override
    public String toString() {
        return display;
    }

    /**
     * @param mappedSymbolName
     *            the mappedSymbolName to set
     */
    public void setMappedSymbolName(String mappedSymbolName) {
        this.mappedSymbolName = mappedSymbolName;
    }

    /**
     * @return the mappedSymbolName
     */
    public String getMappedSymbolName() {
        return mappedSymbolName;
    }

}
