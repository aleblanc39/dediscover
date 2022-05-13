
package edu.rochester.urmc.cbim.dediscover.plotting;

import org.jfree.data.xy.AbstractXYDataset;
import org.jfree.data.xy.XYDataset;
import org.woodhill.ded.results.KResultData;
import org.woodhill.ded.results.SimulationResultWrapper;


public class VarToVarDataSet extends AbstractXYDataset implements XYDataset {

    private final String xAxis;
    private final String yAxis;
    private final KResultData dataX;
    private final KResultData dataY;

    /**
     * @param res
     */
    public VarToVarDataSet(String xAxisVar, String yAxisVar,
                           SimulationResultWrapper res) {
        this.xAxis = xAxisVar;
        this.yAxis = yAxisVar;
        dataX = res.getData(xAxisVar);
        dataY = res.getData(yAxisVar);
    }

    /*
     * (non-Javadoc)
     *
     * @see org.jfree.data.xy.XYDataset#getItemCount(int)
     */
    @Override
    public int getItemCount(int series) {
        return dataX.getItemCount();
    }

    /*
     * (non-Javadoc)
     *
     * @see org.jfree.data.xy.AbstractXYDataset#getXValue(int, int)
     */
    @Override
    public double getXValue(int series, int item) {
        return dataX.get(0, item);
    }

    /*
     * (non-Javadoc)
     *
     * @see org.jfree.data.xy.AbstractXYDataset#getYValue(int, int)
     */
    @Override
    public double getYValue(int series, int item) {
        return dataY.get(0, item);
    }

    /*
     * (non-Javadoc)
     *
     * @see org.jfree.data.xy.XYDataset#getX(int, int)
     */
    @Override
    public Number getX(int series, int item) {
        return getXValue(series, item);
    }

    /*
     * (non-Javadoc)
     *
     * @see org.jfree.data.xy.XYDataset#getY(int, int)
     */
    @Override
    public Number getY(int series, int item) {
        return getYValue(series, item);
    }

    /*
     * (non-Javadoc)
     *
     * @see org.jfree.data.general.AbstractSeriesDataset#getSeriesCount()
     */
    @Override
    public int getSeriesCount() {
        return 1;
    }

    /*
     * (non-Javadoc)
     *
     * @see org.jfree.data.general.AbstractSeriesDataset#getSeriesKey(int)
     */
    @Override
    public Comparable getSeriesKey(int arg0) {
        return xAxis + "-" + yAxis;
    }

}
