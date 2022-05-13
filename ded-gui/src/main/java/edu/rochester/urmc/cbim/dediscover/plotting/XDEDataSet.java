
package edu.rochester.urmc.cbim.dediscover.plotting;

import org.jfree.data.xy.AbstractXYDataset;
import org.woodhill.ded.ui.chartoptions.ChartableControlsTB;
import org.woodhill.ded.ui.chartoptions.ParameterChartOptions;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.function.Function;

/**
 * isFormula refers to formula in mapping.
 * primarySeries used in plotting. Need a good definition.
 * getSeriesParam returns dep var names, table col names.DATA --- figure out how used
 * only used as parameter to getDisplayStyle, and in finding primary series
 */

public abstract class XDEDataSet extends AbstractXYDataset {

    /**
     * Get the name of a parameter given the series index
     *
     * @param series
     * @return
     */
    abstract public String getSeriesParam(int series);

    /**
     * Get all series that are displayed with a particular display style
     *
     * @param config
     * @param style
     * @return
     */
    public SeriesGroup getShapeSeriesSet(
            Map<String, ParameterChartOptions> config,
            ParameterChartOptions.DisplayStyle style) {
        SeriesGroup shapeSeries = new SeriesGroup();
        for (int i = 0; i < getSeriesCount(); i++) {
            ParameterChartOptions s = config.get(getSeriesParam(i));
            if ((s.getDisplayStyle() == style)
                    || (s.getDisplayStyle() == ParameterChartOptions.DisplayStyle.BOTH)) {
                shapeSeries.add(i);
            }
        }
        return shapeSeries;
    }

    public SeriesGrouping getSeriesGroupings(String title,
                                             ChartableControlsTB chartOptionData) {
        List<SeriesDescriptor> descriptors = new ArrayList<>();
        for (int i = 0; i < getSeriesCount(); i++) {
            if (chartOptionData.contains(getSeriesParam(i))) {
                if (isPrimarySeries(i)) {
                    descriptors.add(new SeriesDescriptor(title,
                            (String) getSeriesKey(i), i, chartOptionData
                            .getDisplayStyle(getSeriesParam(i)),
                            isFormula(i)));
                } else {
                    descriptors.add(new SeriesDescriptor(title,
                            (String) getSeriesKey(i), i, findPrimarySeries(i),
                            chartOptionData.getDisplayStyle(getSeriesParam(i)),
                            isData(i), isMean(i), false));
                }
            }
        }
        return new SeriesGrouping(descriptors);
    }



    /**
     * @param i
     * @return
     */
    protected abstract boolean isFormula(int i);

    /**
     * @param i
     * @return
     */
    protected abstract boolean isMean(int i);

    /**
     * @param i
     * @return
     */
    protected abstract boolean isData(int i);

    /**
     * @param i
     * @return
     */
    protected abstract boolean isPrimarySeries(int i);

    /**
     * @param i
     * @return
     */
    protected abstract int findPrimarySeries(int i);

    /**
     * @param i
     * @param seriesParam
     * @return
     */

    public boolean matchesPrimary(int i, String seriesParam, Function<String, String> symbolMappedTo) {
        return false;
    }

}
