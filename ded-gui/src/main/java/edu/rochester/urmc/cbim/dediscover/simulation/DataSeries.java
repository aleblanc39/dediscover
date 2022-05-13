
package edu.rochester.urmc.cbim.dediscover.simulation;

public class DataSeries {

    public final double[] xValues, yValues;

    public DataSeries(double[] x, double[] y) {
        if (x.length != y.length) {
            throw new BadSeriesException(
                    "Number of x and y values did not match");
        }
        this.xValues = x;
        this.yValues = y;
    }

}
