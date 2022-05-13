
package edu.rochester.urmc.cbim.dediscover.plotting;

import java.util.HashMap;
import java.util.Map;

import edu.rochester.urmc.cbim.utility.exception.XDEInternalErrorRTException;

public class SeriesGrouping {

    private Map<Integer, SeriesDescriptor> descriptors;

    public SeriesGrouping(SeriesDescriptor... descriptors) {
        throw new XDEInternalErrorRTException("Ever used???");
        // this.descriptors = new HashMap<Integer, SeriesDescriptor>();
        // for (SeriesDescriptor d : descriptors) {
        // this.descriptors.put(d.seriesId, d);
        // }
    }

    public SeriesGrouping(Iterable<SeriesDescriptor> descriptors) {
        this.descriptors = new HashMap<Integer, SeriesDescriptor>();
        for (SeriesDescriptor d : descriptors) {
            this.descriptors.put(d.seriesId, d);
        }
    }

    /**
     * Get the descriptor for the given series index
     *
     * @param series
     * @return
     */
    public SeriesDescriptor getDescriptor(int series) {
        return descriptors.get(series);
    }

    /**
     * @param series
     * @return
     */
    public int getPrimarySeriesFor(int series) {
        if (descriptors.get(series) == null) {
            return series;
        }
        return descriptors.get(series).getPrimarySeriesId();
    }

    /**
     * @param series
     * @return
     */
    public boolean isCurve(int series) {
        if (descriptors.get(series) == null) {
            return true;
        }
        return descriptors.get(series).isCurve();
    }

    /**
     * @param series
     * @return
     */
    public boolean isPoints(int series) {
        if (descriptors.get(series) == null) {
            return false;
        }
        return descriptors.get(series).isPoints();
    }

    /**
     * @param series
     * @return
     */
    public boolean isPrimary(int series) {
        if (descriptors.get(series) == null) {
            return true;
        }
        return descriptors.get(series).isPrimary();
    }

    /**
     * @param series
     * @return
     */
    public boolean isDataSeries(int series) {
        if (descriptors.get(series) == null) {
            return false;
        }
        return descriptors.get(series).isData();
    }

    /**
     * @param series
     * @return
     */
    public boolean isMeanSeries(int series) {
        if (descriptors.get(series) == null) {
            return false;
        }
        return descriptors.get(series).isMean();
    }

    /**
     * @param series
     * @return
     */
    public boolean isFormulaSeries(int series) {
        if (descriptors.get(series) == null) {
            return false;
        }
        return descriptors.get(series).isFormula();
    }

}
