
package edu.rochester.urmc.cbim.dediscover.plotting;

import java.awt.Shape;
import java.awt.geom.Ellipse2D;
import java.util.ArrayList;
import java.util.List;

import org.woodhill.ded.results.AbstractChartableResultModel;
import org.woodhill.ded.ui.chartoptions.ParameterChartOptions;
import org.jfree.chart.axis.NumberAxis;
import org.jfree.chart.axis.ValueAxis;
import org.jfree.chart.event.AxisChangeEvent;
import org.jfree.chart.event.AxisChangeListener;
import org.jfree.chart.renderer.xy.XYItemRenderer;
import org.jfree.chart.renderer.xy.XYLineAndShapeRenderer;

import edu.rochester.urmc.cbim.dediscover.simulation.NameKey;


public class VarToVarDisplayConfig extends DatasetDisplayConfig {

    private String xAxis;
    private String yAxis;

    /**
     */
    public VarToVarDisplayConfig(String plotTitlePrefix,
            final NameKey xAxisName,
                                 final NameKey yAxisName, String setName, boolean log,
                                 AbstractChartableResultModel simmodel) {
        super(plotTitlePrefix + " - " + xAxisName.display + "-" + yAxisName.display + " (" + setName
                + ")",
                log,
                simmodel, //Function<String, SeriesGrouping>
                (String title) -> {
                    List<SeriesDescriptor> descripts = new ArrayList<>();
                    descripts
                        .add(new SeriesDescriptor(title, xAxisName.display
                                + "-" + yAxisName.display, 0,
                                ParameterChartOptions.DisplayStyle.CURVE, false));
                return new SeriesGrouping(descripts);
            },
         null, setName);
        this.xAxis = xAxisName.display;
        this.yAxis = yAxisName.display;
    }

    /*
     * (non-Javadoc)
     *
     * @see edu.rochester.urmc.cbim.dediscover.plotting.DatasetDisplayConfig#
     * createRangeAxis()
     */
    @Override
    public ValueAxis createRangeAxis() {
        ValueAxis a = super.createRangeAxis();
        a.setLabel(properties.getRangeTitle(yAxis));
        return a;
    }

    /*
     * (non-Javadoc)
     *
     * @see edu.rochester.urmc.cbim.dediscover.plotting.DatasetDisplayConfig#
     * createDomainAxis()
     */
    @Override
    public ValueAxis createDomainAxis() {
        final NumberAxis axis = new NumberAxis(properties.getDomainTitle(xAxis));
        axis.setTickMarksVisible(properties.isDomainTicksVisible());
        axis.addChangeListener(new AxisChangeListener() {

            @Override
            public void axisChanged(AxisChangeEvent arg0) {
                String title = axis.getLabel();
                properties.setDomainTitle(title);
                properties.setDomainRange(!axis.isAutoRange());
                if (!axis.isAutoRange()) {
                    properties.setDomainMin(axis.getRange().getLowerBound());
                    properties.setDomainMax(axis.getRange().getUpperBound());
                }
                properties.setDomainTickMarks(axis.isTickMarksVisible());
                // properties.save(); not actualaly saving
                //simmodel.firePlotsChanged();
            }
        });
        return axis;
    }

    /*
     * (non-Javadoc)
     *
     * @see
     * edu.rochester.urmc.cbim.dediscover.plotting.DatasetDisplayConfig#getRenderer
     * ()
     */
    @Override
    public XYItemRenderer getRenderer() {
        return new XYLineAndShapeRenderer() {

            @Override
            public boolean getItemShapeVisible(int series, int item) {
                return item == 0;
            }

            @Override
            public Shape getItemShape(int row, int column) {
                if (column == 0) {
                    return new Ellipse2D.Double(-3d, -3d, 6d, 6d);
                } else {
                    return null;
                }
            }
        };
    }
}
