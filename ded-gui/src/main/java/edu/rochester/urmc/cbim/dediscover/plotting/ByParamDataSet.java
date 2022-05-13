
package edu.rochester.urmc.cbim.dediscover.plotting;

import edu.rochester.urmc.cbim.dediscover.simulation.DataMappingInfo;
import edu.rochester.urmc.cbim.utility.exception.XDEInternalErrorRTException;
import org.woodhill.ded.results.SimulationResultWrapper;
import org.woodhill.ded.ui.chartoptions.ChartableControlsTB;

import java.util.List;
import java.util.function.Function;

/**
 * Dataset with multiple parameter sets for one chart variable.
 */
public class ByParamDataSet extends XDEDataSet {

    private final ResultSetWrap[] resultSetWraps;
    private final String chartVar;
    private final XDEDataSet dataPointSet;
    private final Function<String, String> symbolMappedTo;
    private final Function<String, Boolean> isMappingFormula;


    /**
     */
    public ByParamDataSet(ChartableControlsTB tb, String chartVar, MappingInfoInterface mappingInfoSet, List<DataMappingInfo> dataMappings, boolean log,
                          boolean vis, Function<String, String> symbolMappedTo, Function<String, Boolean> isMappingFormula, SimulationResultWrapper... results) {

        this.symbolMappedTo = symbolMappedTo;
        this.isMappingFormula = isMappingFormula;
        if (results.length == 0)
            throw new XDEInternalErrorRTException(
                    "Must have at least one result wrapper.");
        if (vis) {
            this.resultSetWraps = new ResultSetWrap[results.length];
            for (int i = 0; i < results.length; i++) {
                resultSetWraps[i] = new ResultSetWrap(results[i].getData(chartVar, log,
                        tb), results[i].getSetName());
            }
        } else {
            this.resultSetWraps = null;
        }
        this.chartVar = chartVar;
        dataPointSet = DataPointDataSet.getDataPointSet(tb, mappingInfoSet, dataMappings, log, false);
    }

    /**
     */

    /*
     * (non-Javadoc)
     *
     * @see org.jfree.data.general.AbstractSeriesDataset#getSeriesCount()
     */
    @Override
    public int getSeriesCount() {
        return (resultSetWraps != null ? resultSetWraps.length : 0) + dataPointSet.getSeriesCount();
    }

    /*
     * (non-Javadoc)
     *
     * @see edu.rochester.urmc.cbim.dediscover.simulation.XDEDataSet#isData(int)
     */
    @Override
    protected boolean isData(int i) {
        if (resultSetWraps != null) {
            return i >= resultSetWraps.length && dataPointSet.isData(i
                    - resultSetWraps.length);
        } else {
            return dataPointSet.isData(i);
        }
    }

    /*
     * (non-Javadoc)
     *
     * @see
     * edu.rochester.urmc.cbim.dediscover.simulation.XDEDataSet#isFormula(int)
     */
    @Override
    protected boolean isFormula(int i) {
        if (resultSetWraps != null) {
            return i < resultSetWraps.length ? isMappingFormula.apply(getSeriesParam(i)) : dataPointSet.isFormula(i - 1);
        } else {
            return dataPointSet.isFormula(i);
        }
    }

    /*
     * (non-Javadoc)
     *
     * @see edu.rochester.urmc.cbim.dediscover.simulation.XDEDataSet#isMean(int)
     */
    @Override
    protected boolean isMean(int i) {
        if (resultSetWraps != null) {
            return i >= resultSetWraps.length && dataPointSet.isMean(i
                    - resultSetWraps.length);
        } else {
            return dataPointSet.isMean(i);
        }
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
        if (resultSetWraps != null) {
            return i < resultSetWraps.length;
        } else {
            return false;
        }
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
        if (resultSetWraps != null) {
            if (i < resultSetWraps.length) {
                return i;
            } else {
                i = i - resultSetWraps.length;
                for (int j = 0; j < resultSetWraps.length; j++) {
                    if (dataPointSet.matchesPrimary(i, getSeriesParam(j), symbolMappedTo)) {
                        return j;
                    }
                }
                return -1;
            }
        } else {
            return 0;
        }
    }

    /*
     * (non-Javadoc)
     *
     * @see org.jfree.data.general.AbstractSeriesDataset#getSeriesKey(int)
     */
    @Override
    public Comparable<String> getSeriesKey(int series) {
        if (resultSetWraps != null) {
            if (series >= resultSetWraps.length) {
                return dataPointSet.getSeriesKey(series - resultSetWraps.length);
            } else {
                return resultSetWraps[series].setName;
            }
        } else {
            return dataPointSet.getSeriesKey(series);
        }
    }

    /*
     * (non-Javadoc)
     *
     * @see
     * edu.rochester.urmc.cbim.dediscover.simulation.XDEDataSet#getSeriesParam
     * (int)
     */
    @Override
    public String getSeriesParam(int series) {
        if (resultSetWraps != null) {
            if (series >= resultSetWraps.length) {
                return dataPointSet.getSeriesParam(series - resultSetWraps.length);
            } else {
                return chartVar;
            }
        } else {
            return dataPointSet.getSeriesParam(series);
        }
    }

    /*
     * (non-Javadoc)
     *
     * @see org.jfree.data.xy.XYDataset#getItemCount(int)
     */
    @Override
    public int getItemCount(int series) {
        if (resultSetWraps != null) {
            if (series >= resultSetWraps.length) {
                return dataPointSet.getItemCount(series - resultSetWraps.length);
            } else {
                return resultSetWraps[series].data.getItemCount();
            }
        } else {
            return dataPointSet.getItemCount(series);
        }
    }

    /*
     * (non-Javadoc)
     *
     * @see org.jfree.data.xy.AbstractXYDataset#getXValue(int, int)
     */
    @Override
    public double getXValue(int series, int item) {
        if (resultSetWraps != null) {
            if (series >= resultSetWraps.length) {
                return dataPointSet.getXValue(series - resultSetWraps.length, item);
            } else {
                return resultSetWraps[series].data.timepoint(item);
            }
        } else {
            return dataPointSet.getXValue(series, item);
        }
    }

    /*
     * (non-Javadoc)
     *
     * @see org.jfree.data.xy.AbstractXYDataset#getYValue(int, int)
     */
    @Override
    public double getYValue(int series, int item) {
        if (resultSetWraps != null) {
            if (series >= resultSetWraps.length) {
                return dataPointSet.getYValue(series - resultSetWraps.length, item);
            } else {
                return resultSetWraps[series].data.get(0, item);
            }
        } else {
            return dataPointSet.getYValue(series, item);
        }
    }

    /*
     * (non-Javadoc)
     *
     * @see org.jfree.data.xy.XYDataset#getX(int, int)
     */
    @Override
    public Number getX(int arg0, int arg1) {
        return null;
    }

    /*
     * (non-Javadoc)
     *
     * @see org.jfree.data.xy.XYDataset#getY(int, int)
     */
    @Override
    public Number getY(int arg0, int arg1) {
        return null;
    }

}
