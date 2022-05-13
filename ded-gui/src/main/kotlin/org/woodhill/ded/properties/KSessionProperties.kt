package org.woodhill.ded.properties


abstract class KSessionProperties {
    private val props =  mutableMapOf<String, String>()
    private var dirty = false


    /**
     *
     */
    fun clear() {
        props.clear()
        setDirty()
    }

    protected fun setDirty() {
        dirty = true
    }

    /**
     * @param key
     * @param value
     * @return
     * @see java.util.Map.put
     */
    fun put(key: String, value: Any):String {
        if (!equiv(props[key], value)) {
            setDirty()
        }
        props[key] = value.toString()
        return value.toString()
    }

    /**
     * @param key
     * @return
     * @see java.util.Map.get
     */
    fun get(key: String): String? {
        return props[key]
    }
    fun get(key: String, default: String): String {
        return props[key]?:default
    }
    /**
     * @param key
     *
     * @return
     * @see java.util.Map.get
     */
     fun getAsString(key: Any): String {
        return props[key].toString()
    }

    /**
     * @param key
     * @return
     * @see java.util.Map.containsKey
     */
    fun containsKey(key: Any?): Boolean {
        return props.containsKey(key)
    }

    /**
     * @param key
     * @return
     * @see java.util.Map.remove
     */
    fun remove(key: Any?): Any? {
        if (props.containsKey(key)) {
            setDirty()
        }
        return props.remove(key)
    }

    /**
     * @return
     * @see java.util.Map.keySet
     */
    fun keySet(): Set<String> {
        return props.keys
    }


    fun copyPropertiesTo(toProps: KSessionProperties) {
        println("Copying properties")
        props.forEach{
            toProps.put(it.key, it.value)
        }
    }

    companion object {
        /**
         * Test for equals between two objects. Is safe for null parameters. Returns
         * true if both are null.
         *
         * @param a
         * @param b
         * @return
         */
        fun equiv(a: Any?, b: Any?): Boolean {
            return if (a == null && b == null) {
                true
            } else if (a == null || b == null) {
                false
            } else {
                a == b
            }
        }
    }
}