
package edu.rochester.urmc.cbim.dediscover.plotting;

import java.awt.Color;
import java.text.DateFormat;
import java.util.Date;
import java.util.HashMap;
import java.util.Locale;
import java.util.Map;
import java.util.function.Function;

import javax.swing.SwingUtilities;

import org.woodhill.ded.properties.KPlotProperties;
import org.woodhill.ded.results.KPlotResultsModel;
import org.jfree.chart.ChartPanel;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.axis.LogarithmicAxis;
import org.jfree.chart.axis.NumberAxis;
import org.jfree.chart.axis.ValueAxis;
import org.jfree.chart.event.AxisChangeEvent;
import org.jfree.chart.event.AxisChangeListener;
import org.jfree.chart.event.ChartChangeEvent;
import org.jfree.chart.event.ChartChangeListener;
import org.jfree.chart.plot.XYPlot;
import org.jfree.chart.renderer.xy.XYItemRenderer;
import org.jfree.chart.title.DateTitle;
import org.jfree.chart.title.TextTitle;

import edu.rochester.urmc.cbim.dediscover.simulation.ChartMenu;
import edu.rochester.urmc.cbim.dediscover.simulation.ScientificFormatter;
import org.jfree.chart.ui.RectangleEdge;

/**
 * Hold gui display settings specific to a JFreeChart dataset.
 *
 */
public class DatasetDisplayConfig implements ChartChangeListener {

    static int counter = 0;
    int myCounter;

    public final String title;
    public final boolean log;
    private final Map<Comparable<?>, SeriesDisplayConfig> seriesConfigs = new HashMap<>();
    private String seriesKeys;
    protected KPlotProperties properties;

    // TODO simmodel only used to handle, keep track of start time, end time. Should remove
    protected KPlotResultsModel simmodel;
    private final SeriesGrouping seriesGroupings;
    private final String[] setNames;

    private ChartPanel chart;
    private JFreeChart jfc;
    // private XYPlot xyPlot;
    private TextTitle outofdate;
    private DateTitle timestamp;

    // TODO Check here...
    public DatasetDisplayConfig(String title,
                                boolean log,
                                KPlotResultsModel simmodel,
                                Function<String, SeriesGrouping> seriesGroupingsGen,
                                String[] seriesKeys,
                                String... setNames) {
        myCounter = counter++;
        this.title = title;
        this.log = log;
        this.simmodel = simmodel;
        if ((seriesKeys != null) && (seriesKeys.length > 0)) {
            StringBuilder buf = new StringBuilder("\n(" + seriesKeys[0]);
            for (int i = 1; i < seriesKeys.length; i++) {
                buf.append(", ");
                buf.append(seriesKeys[i]);
            }
            buf.append(")");
            this.seriesKeys = buf.toString();
        }
        this.properties = new KPlotProperties(this.title);//.retrieve(session);
        //this.title = properties.getTitle(title);
        //this.seriesGroupings = seriesGroupings.generate(title);
        this.seriesGroupings = seriesGroupingsGen.apply(this.title);
        this.setNames = setNames != null ? setNames : new String[0];
    }

    public SeriesDisplayConfig getSeriesConfig(Comparable<?> seriesKey) {
        return seriesConfigs.get(seriesKey);
    }

    public void clearProperties() {
        properties.clear();
        //properties.save();
        //simmodel.firePlotsChanged();
    }

    public void setSeriesConfig(Comparable<?> seriesKey,
                                SeriesDisplayConfig conf) {
        seriesConfigs.put(seriesKey, conf);
    }

    public ValueAxis createDomainAxis() {
        final NumberAxis axis = new NumberAxis(
                properties.getDomainTitle("t (Time)"));
        if (properties.hasDomainRange()) {
            axis.setRange(properties.getDomainMin(), properties.getDomainMax());
        } else {
            axis.setRange(simmodel.getResultsStartTime(setNames),
                    simmodel.getResultsEndTime(setNames));
        }
        axis.setTickMarksVisible(properties.isDomainTicksVisible());
        axis.addChangeListener(new AxisChangeListener() {

            @Override
            public void axisChanged(AxisChangeEvent arg0) {
                String title = axis.getLabel();
                properties.setDomainTitle(title);
                properties.setDomainRange(!isAxisAutoRange(axis));
                if (!isAxisAutoRange(axis)) {
                    properties.setDomainMin(axis.getRange().getLowerBound());
                    properties.setDomainMax(axis.getRange().getUpperBound());
                }
                properties.setDomainTickMarks(axis.isTickMarksVisible());
                //properties.save();

                // TODO Seems like a lot of info here. Shouldn't this object
                // fire the change,
                // have the simmodel listen, and decide what to do?

                //simmodel.firePlotsChanged();
            }
        });
        return axis;
    }

    /**
     * @return
     */
    public ValueAxis createRangeAxis() {
        final NumberAxis axis = log ? new DELogarithmicAxis(
                properties.getRangeTitle(getSeriesKeysString()), 15)
                : new NumberAxis(
                properties.getRangeTitle(getSeriesKeysString()));
        if (log) {
            ((LogarithmicAxis) axis).setAllowNegativesFlag(true);
        }
        if (properties.hasRangeRange()) {
            axis.setRange(properties.getRangeMin(), properties.getRangeMax());
        } else {
            axis.setAutoRange(true);
            axis.setAutoRangeIncludesZero(true);
            axis.setAutoRangeStickyZero(true);
        }
        (axis).setNumberFormatOverride(new ScientificFormatter(true, 2));
        axis.setTickMarksVisible(properties.isRangeTicksVisible());
        axis.addChangeListener(new AxisChangeListener() {

            @Override
            public void axisChanged(AxisChangeEvent arg0) {
                String title = axis.getLabel();
                properties.setRangeTitle(title);
                properties.setRangeRange(!axis.isAutoRange());
                if (!axis.isAutoRange()) {
                    properties.setRangeMin(axis.getRange().getLowerBound());
                    properties.setRangeMax(axis.getRange().getUpperBound());
                }
                properties.setRangeTickMarks(axis.isTickMarksVisible());
                //properties.save(); // AL2020 Really want to save here? Not actually saving
                //simmodel.firePlotsChanged();
            }
        });
        return axis;
    }

    private String getSeriesKeysString() {
        return (seriesKeys != null ? seriesKeys : "");
    }

    /**
     * @return
     */
    public XYItemRenderer getRenderer() {
        return new DelegationRenderer(seriesGroupings,
                PlotColors.makePlotColorUtil(title));
    }

    /**
     * @return
     */
    public boolean hasLegend() {
        return true;
    }

    /*
     * (non-Javadoc)
     *
     * @see
     * org.jfree.chart.event.ChartChangeListener#chartChanged(org.jfree.chart
     * .event.ChartChangeEvent)
     */
    @Override
    public void chartChanged(final ChartChangeEvent cce) {
        SwingUtilities.invokeLater(new Runnable() {

            @Override
            public void run() {
                if ((cce != null) && (cce.getChart() != null)) {
                    properties.saveChartProperties(chart);
                    if (cce.getChart().getTitle() == null) {
                        properties.setShowTitle(false);
                    } else {
                        properties.setShowTitle(true);
                        String title = cce.getChart().getTitle().getText();
                        properties.setTitleColor((Color) cce.getChart()
                                .getTitle().getPaint());
                        properties.setTitleFont(cce.getChart().getTitle()
                                .getFont());
                        properties.setShowTitle(cce.getChart().getTitle()
                                .isVisible());
                        if ((title != null)
                                && !title
                                .equals(DatasetDisplayConfig.this.title)
                                && (title.length() > 0)) {
                            properties.setTitle(title);
                        }
                    }
                    //properties.save();
                }
            }
        });
    }

    /**
     * @param chart
     * @param jfc
     * @param xyPlot
     */
    public void configure(final ChartPanel chart, final JFreeChart jfc,
                          final XYPlot xyPlot) {
        this.chart = chart;
        this.jfc = jfc;
        // this.xyPlot = xyPlot;
        chart.setBackground(Color.white);
        jfc.setBackgroundPaint(Color.white);
        properties.restoreChartProperties(chart);
        // jfc.setNotify(false);
        setSubtitles(jfc);
        setGridLineVisible(jfc);
        chart.setMinimumDrawHeight(0);
        chart.setMinimumDrawWidth(150);
        ChartMenu.adjustNewPopupMenu(chart, this);
        jfc.getTitle().setPaint(properties.getTitleColor(Color.black));
        jfc.getTitle().setFont(
                properties.getTitleFont(jfc.getTitle().getFont()));
        if (!properties.isShowTitle(true)) {
            jfc.setTitle((TextTitle) null);
        }
        jfc.addChangeListener(this);
    }

    private void setSubtitles(final JFreeChart jfc) {
        if (jfc != null) {
            if (outofdate != null) {
                jfc.removeSubtitle(outofdate);
            }
            if (timestamp != null) {
                jfc.removeSubtitle(timestamp);
            }
            outofdate = null;
            timestamp = null;
            if (isChartDirty()) {
                outofdate = new TextTitle("Chart is out of date");
                outofdate.setPaint(Color.red);
                outofdate.setPosition(RectangleEdge.TOP);
                jfc.addSubtitle(outofdate);
            }
//	    if (XDEApplication.getPreferencesValue("plot.timestamp", "true")
//		    .toLowerCase().startsWith("true")) {
            timestamp = new DateTitle(DateFormat.FULL) {
                @Override
                public void setDateFormat(int style, Locale locale) {
                    setText(DateFormat.getDateTimeInstance(style,
                            DateFormat.LONG, locale).format(new Date()));
                }
            };
            timestamp.setDateFormat(DateFormat.LONG, Locale.getDefault());
            timestamp.setPosition(RectangleEdge.BOTTOM);
            jfc.addSubtitle(timestamp);
        }
        chart.invalidate();

    }

    // set plot grid line visible or not based on user preference's setting
    private void setGridLineVisible(final JFreeChart jfc) {
        XYPlot plot = ((XYPlot) jfc.getPlot());
//        if (XDEApplication.getPreferencesValue("plot.gridline", "true")
//                .toLowerCase().startsWith("true")) {
//            plot.setDomainGridlinesVisible(true);
//            plot.setRangeGridlinesVisible(true);
//        } else {
            plot.setDomainGridlinesVisible(false);
            plot.setRangeGridlinesVisible(false);
 //       }
        chart.invalidate();
    }

    /**
     * @return
     */
    private boolean isChartDirty() {
//        for (String set : setNames) {
//            if (simmodel.isDirty(set)) {
//                return true;
//            }
//        }
        return false;
    }

    /**
     *
     */
    public void updateChart() {
        setSubtitles(jfc);
    }

    public boolean isAxisAutoRange(final ValueAxis axis) {
        return axis.isAutoRange()
                || ((axis.getRange().getLowerBound() == simmodel
                .getResultsStartTime(setNames)) && (axis.getRange()
                .getUpperBound() == simmodel
                .getResultsEndTime(setNames)));
    }
}
