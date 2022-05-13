package org.woodhill.ded.results

interface KResultData {


    fun getNames(): List<String?>?

    /**
     * Get a single data point given the data series index and the item index.
     *
     * @param seriesIndex
     * @param item
     * @return
     */
    operator fun get(seriesIndex: Int, item: Int): Double

    /**
     * Get the number of items in each series
     *
     * @return
     */
    fun getItemCount(): Int

    /**
     * Get the timepoint value of the given item
     *
     * @param item
     * @return
     */
    fun timepoint(item: Int): Double

    /**
     * Get the number of series names in the result data
     *
     * @return
     */
    fun getNumNames(): Int

    /**
     * Get the name of a given series
     *
     * @param i
     * @return
     */
    fun getName(i: Int): String?

    /**
     * Select a subset of the data
     *
     * @param filter
     * @return
     */
    fun filter(accept: (String) -> Boolean): KResultData
}