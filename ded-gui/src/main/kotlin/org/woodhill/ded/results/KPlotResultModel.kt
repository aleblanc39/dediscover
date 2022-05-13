package org.woodhill.ded.results

import edu.rochester.urmc.cbim.dediscover.plotting.DatasetDisplayConfig
import org.jfree.data.xy.XYDataset


interface KPlotResultsModel {
    /**
     * @param xyDataset
     * @return
     */
    fun getDatasetConfig(xyDataset: XYDataset): DatasetDisplayConfig

    /**
     * @return
     */
    //val dataSets: Iterable<Array<XYDataset>>

    /**
     * @return
     */
    val isTabular: Boolean
     /**
     * @return
     */
    val isTabularTransposed: Boolean
    /**
     * @return
     */
//TransposableTableModel getAllResultsTableModel();
    /**
     * @param setNames
     * @return
     */
    fun getResultsStartTime(setNames: Array<String> = emptyArray()): Double

    /**
     * @param setNames
     * @return
     */
    fun getResultsEndTime(setNames: Array<String> = emptyArray()): Double




    /**
     * @return
     */
    val isValid: Boolean
}