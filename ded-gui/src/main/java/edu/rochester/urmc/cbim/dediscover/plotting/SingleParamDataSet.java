
package edu.rochester.urmc.cbim.dediscover.plotting;

import edu.rochester.urmc.cbim.dediscover.simulation.DataMappingInfo;
import org.woodhill.ded.results.KResultData;
import org.woodhill.ded.results.SimulationResultWrapper;
import org.woodhill.ded.ui.chartoptions.ChartableControlsTB;

import java.util.List;
import java.util.Set;
import java.util.function.Function;


public class SingleParamDataSet extends XDEDataSet {

    protected final KResultData data;
    private final String setName;
    private final String chartVar;
    private final Function<String, Boolean> isDataColFormula;
    private XDEDataSet dataPointSet;

    /**
     * @param name
     * @param chartVar
     */
    public SingleParamDataSet(ChartableControlsTB chartTB, String name,
                              MappingInfoInterface mappingInfoInterface,
                              List<DataMappingInfo> dataMappingInfoList,
                              final String chartVar,
                              SimulationResultWrapper result,
                              boolean primaryIsVisible,
                              Function<String, Boolean> isDataColFormula) {
        this.chartVar = chartVar;
        this.isDataColFormula = isDataColFormula;
        if (primaryIsVisible) {
            this.data = result.getData().filter(name1 -> chartVar.equals(name1));
        } else {
            data = null;
        }
        this.setName = name;
        dataPointSet = DataPointDataSet.getDataPointSet(chartTB, chartVar, mappingInfoInterface, dataMappingInfoList, false, false);
    }


    /*
     * (non-Javadoc)
     *
     * @see org.jfree.data.general.AbstractSeriesDataset#getSeriesCount()
     */
    @Override
    public int getSeriesCount() {
        return (data != null ? 1 : 0) + dataPointSet.getSeriesCount();
    }

    /*
     * (non-Javadoc)
     *
     * @see edu.rochester.urmc.cbim.dediscover.simulation.XDEDataSet#isData(int)
     */
    @Override
    protected boolean isData(int i) {
        if (data != null) {
            return i == 0 ? false : dataPointSet.isData(i - 1);
        } else {
            return dataPointSet.isData(i);
        }
    }

    /*
     * (non-Javadoc)
     *
     * @see edu.rochester.urmc.cbim.dediscover.simulation.XDEDataSet#isMean(int)
     */
    @Override
    protected boolean isMean(int i) {
        if (data != null) {
            return i == 0 ? false : dataPointSet.isMean(i - 1);
        } else {
            return dataPointSet.isMean(i);
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
        if (data != null) {
            try {
                return i == 0 ? isDataColFormula.apply(getSeriesParam(i)) : dataPointSet.isFormula(i - 1);
            } catch (Exception e) {
                return false;
            }
        } else {
            return dataPointSet.isFormula(i);
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
        if (data != null) {
            return i < 1;
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
        return 0;
    }

    /*
     * (non-Javadoc)
     *
     * @see org.jfree.data.general.AbstractSeriesDataset#getSeriesKey(int)
     */
    @Override
    public Comparable<String> getSeriesKey(int series) {
        if (data != null) {
            return series == 0 ? setName + "-" + chartVar : dataPointSet
                    .getSeriesKey(series - 1);
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
        if (data != null) {
            return series == 0 ? chartVar : dataPointSet
                    .getSeriesParam(series - 1);
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
        if (data != null) {
            return series == 0 ? data.getItemCount() : dataPointSet
                    .getItemCount(series - 1);
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
        if (data != null) {
            return series == 0 ? data.timepoint(item) : dataPointSet.getXValue(
                    series - 1, item);
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
        double y;
        if (data != null) {
            y = series == 0 ? data.get(series, item) : dataPointSet.getYValue(
                    series - 1, item);
        } else {
            y = dataPointSet.getYValue(series, item);
        }
        return y;
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
