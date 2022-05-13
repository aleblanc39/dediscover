package edu.rochester.urmc.cbim.dediscover.plotting;

import edu.rochester.urmc.cbim.dediscover.simulation.DataMappingInfo;
import org.woodhill.ded.results.SimulationResultWrapper;
import org.woodhill.ded.ui.chartoptions.ChartableControlsTB;

import java.util.List;
import java.util.function.Function;

/**
 * Dataset for plotting where everything is on one plot. Multiple variables, parameter sets and data/formulas
 */
public class CombinedDataSet extends XDEDataSet {

    private final ResultSetWrap[] resultSetWraps;
    private final XDEDataSet dataPointSet;
    private final int nbNames;
    private final Function<String, String> symbolMappedTo;
    private final Function<String, Boolean> isMappingFormula;

    public CombinedDataSet(ChartableControlsTB chartableControlsTB,
                           MappingInfoInterface mappingInfoInterface,
                           List<DataMappingInfo> dataMappingInfoList,
                           boolean log,
                           Function<String, String> symbolMappedTo,
                           Function<String, Boolean> isFormula,
                           SimulationResultWrapper... results) {

        this.symbolMappedTo = symbolMappedTo;
        this.isMappingFormula = isFormula;
        resultSetWraps = new ResultSetWrap[results.length];
        for (int i = 0; i < results.length; i++) {
            resultSetWraps[i] = new ResultSetWrap(results[i].getData(chartableControlsTB, log
                    ), results[i].getSetName());
        }
        dataPointSet = DataPointDataSet.getDataPointSet( chartableControlsTB, mappingInfoInterface, dataMappingInfoList, log, true);

        nbNames = resultSetWraps[0].data.getNumNames();

    }

    /*
     * (non-Javadoc)
     *
     * @see org.jfree.data.general.AbstractSeriesDataset#getSeriesCount()
     */
    @Override
    public int getSeriesCount() {
        return mainSeriesCount() + dataPointSet.getSeriesCount();

    }

    /*
     * (non-Javadoc)
     *
     * @see edu.rochester.urmc.cbim.dediscover.simulation.XDEDataSet#isData(int)
     */
    @Override
    protected boolean isData(int i) {
        return i >= mainSeriesCount() && dataPointSet.isData(i
                - resultSetWraps.length);
    }

    /*
     * (non-Javadoc)
     *
     * @see
     * edu.rochester.urmc.cbim.dediscover.simulation.XDEDataSet#isFormula(int)
     */
    @Override
    protected boolean isFormula(int i) {
        return i < mainSeriesCount() ? isMappingFormula.apply(getSeriesParam(i))
                : dataPointSet.isFormula(i - 1);
   }

    /*
     * (non-Javadoc)
     *
     * @see edu.rochester.urmc.cbim.dediscover.simulation.XDEDataSet#isMean(int)
     */
    @Override
    protected boolean isMean(int i) {
        return i >= mainSeriesCount() && dataPointSet.isMean(i
                - resultSetWraps.length);
    }

    /*
     * (non-Javadoc)
     *
     * @see
     * edu.rochester.urmc.cbim.dediscover.simulation.XDEDataSet#isPrimarySeries
     * (int)
     */
    @Override
    protected boolean isPrimarySeries(int i) {
        return i < mainSeriesCount();
    }

    /*
     * (non-Javadoc)
     *
     * @see
     * edu.rochester.urmc.cbim.dediscover.simulation.XDEDataSet#findPrimarySeries
     * (int)
     */
    @Override
    protected int findPrimarySeries(int i) {
        if (i < mainSeriesCount()) {
            return i;
        } else {
            i = i - mainSeriesCount();
            for (int j = 0; j < mainSeriesCount(); j++) {
                if (dataPointSet.matchesPrimary(i, getSeriesParam(j), symbolMappedTo)) {
                    return j;
                }
            }
            return -1;
        }
    }

    private int mainSeriesCount() {
        return nbNames * resultSetWraps.length;
    }

    /*
     * (non-Javadoc)
     *
     * @see org.jfree.data.general.AbstractSeriesDataset#getSeriesKey(int)
     */
    @Override
    public Comparable<String> getSeriesKey(int series) {
        if (series < mainSeriesCount()) {
            return (resultSetWraps.length > 1 ? (resultSetWraps[series / nbNames].setName + " - ")
                    : "")
                    + resultSetWraps[series / nbNames].data
                    .getName(series % nbNames);
        } else {
            return dataPointSet.getSeriesKey(series - mainSeriesCount());
        }
    }

    @Override
    public String getSeriesParam(int series) {
        if (series < mainSeriesCount()) {
            return resultSetWraps[series / nbNames].data
                    .getName(series % nbNames);
        } else {
            return dataPointSet.getSeriesParam(series - mainSeriesCount());
        }
    }

    /*
     * (non-Javadoc)
     *
     * @see org.jfree.data.xy.XYDataset#getItemCount(int)
     */
    @Override
    public int getItemCount(int series) {
        if (series < mainSeriesCount()) {
            return resultSetWraps[series / nbNames].data
                    .getItemCount();
        } else {
            return dataPointSet.getItemCount(series - mainSeriesCount());
        }
    }

    /*
     * (non-Javadoc)
     *
     * @see org.jfree.data.xy.AbstractXYDataset#getXValue(int, int)
     */
    @Override
    public double getXValue(int series, int item) {
        if (series < mainSeriesCount()) {
            return resultSetWraps[series / nbNames].data
                    .timepoint(item);
        } else {
            return dataPointSet.getXValue(series - mainSeriesCount(), item);
        }
    }

    /*
     * (non-Javadoc)
     *
     * @see org.jfree.data.xy.XYDataset#getX(int, int)
     */
    @Override
    public Number getX(int series, int item) {
        return null;
    }

    /*
     * (non-Javadoc)
     *
     * @see org.jfree.data.xy.AbstractXYDataset#getYValue(int, int)
     */
    @Override
    public double getYValue(int series, int item) {
        if (series < mainSeriesCount()) {
            return resultSetWraps[series / nbNames].data.get(series
                    % nbNames, item);
        } else {
            return dataPointSet.getYValue(series - mainSeriesCount(), item);
        }
    }

    /*
     * (non-Javadoc)
     *
     * @see org.jfree.data.xy.XYDataset#getY(int, int)
     */
    @Override
    public Number getY(int series, int item) {
        return null;
    }

    /**
     * @return
     */
    public String[] getSeriesKeys() {
        String[] keys = new String[getSeriesCount()];
        for (int i = 0; i < getSeriesCount(); i++) {
            keys[i] = (String) getSeriesKey(i);
        }
        return keys;
    }

    /**
     * @return
     */
    public SeriesGroup getDataSeriesSet() {
        SeriesGroup grp = new SeriesGroup();
        for (int i = mainSeriesCount(); i < getSeriesCount(); i++) {
            grp.add(i);
        }
        return grp;
    }

}
