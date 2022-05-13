
package edu.rochester.urmc.cbim.dediscover.plotting;

import java.awt.BasicStroke;
import java.awt.Paint;
import java.awt.Shape;
import java.awt.Stroke;
import java.util.HashMap;
import java.util.Map;

import org.jfree.chart.renderer.xy.XYLineAndShapeRenderer;


@SuppressWarnings("serial")
public class DelegationRenderer extends XYLineAndShapeRenderer {

    private class PaintShape {
	private Paint p;
	private Shape s;

	public Shape shape() {
	    if (s == null) {
		s = PlotShapes.nextShape();
	    }
	    return s;
	}

	public Paint paint() {
	    if (p == null) {
		p = colorUtil.nextColor();
	    }
	    return p;
	}
    }

    private static Map<SeriesDescriptor, PaintShape> colorMap = new HashMap<SeriesDescriptor, PaintShape>();

    /**
	 * 
	 */
    private static final BasicStroke FORMULA_STROKE = new BasicStroke(2.0f,
	    BasicStroke.CAP_BUTT, BasicStroke.JOIN_BEVEL, 1.0f, new float[] {
		    6.0f, 4.0f }, 0.0f);
    private static final BasicStroke DASHED_STROKE = new BasicStroke(2.0f,
	    BasicStroke.CAP_BUTT, BasicStroke.JOIN_BEVEL, 1.0f, new float[] {
		    3.0f, 3.0f }, 0.0f);
    private static final BasicStroke MEAN_STROKE = new BasicStroke(2.0f,
	    BasicStroke.CAP_BUTT, BasicStroke.JOIN_BEVEL, 1.0f, new float[] {
		    2.0f, 8.0f }, 0.0f);
    private static final BasicStroke SOLID_STROKE = new BasicStroke(2.0f,
	    BasicStroke.CAP_BUTT, BasicStroke.JOIN_BEVEL, 1.0f, new float[] {
		    10.0f, 0.0f }, 0.0f);

    transient private SeriesGrouping groups;

    transient private PlotColors colorUtil;

    /**
     */
    public DelegationRenderer(SeriesGrouping groups, PlotColors colorUtil) {
	super(true, true);
	synchronized (colorMap) {
	    int i = 0;
	    SeriesDescriptor descriptor = groups.getDescriptor(i);
	    if ((descriptor != null) && descriptor.isPrimary()) {
		while (descriptor != null) {
		    if (!colorMap.containsKey(descriptor)) {
			colorMap.put(descriptor, new PaintShape());
		    }
		    descriptor = groups.getDescriptor(++i);
		}
	    }
	    this.colorUtil = colorUtil;
	    this.groups = groups;
	}
    }

    /*
     * (non-Javadoc)
     * 
     * @see org.jfree.chart.renderer.AbstractRenderer#getSeriesPaint(int)
     */
    @Override
    public Paint getSeriesPaint(int series) {
	Paint p = getMappedPaintShape(groups.getPrimarySeriesFor(series))
		.paint();
	return p;
    }

    private PaintShape getMappedPaintShape(int primarySeries) {
	synchronized (colorMap) {
	    SeriesDescriptor descriptor = groups.getDescriptor(primarySeries);
	    if (!colorMap.containsKey(descriptor)) {
		colorMap.put(descriptor, new PaintShape());

	    }
	    return colorMap.get(descriptor);
	}
    }

    /*
     * (non-Javadoc)
     * 
     * @see org.jfree.chart.renderer.AbstractRenderer#getSeriesShape(int)
     */
    @Override
    public Shape getSeriesShape(int series) {
	return getMappedPaintShape(series).shape();
    }

    /*
     * (non-Javadoc)
     * 
     * @see
     * org.jfree.chart.renderer.xy.XYLineAndShapeRenderer#getItemLineVisible
     * (int, int)
     */
    @Override
    public boolean getItemLineVisible(int series, int item) {
	return groups.isCurve(series);
    }

    /*
     * (non-Javadoc)
     * 
     * @see org.jfree.chart.renderer.AbstractRenderer#getSeriesStroke(int)
     */
    @Override
    public Stroke getSeriesStroke(int series) {
	if (groups.isDataSeries(series)) {
	    return DASHED_STROKE;
	} else if (groups.isMeanSeries(series)) {
	    return MEAN_STROKE;
	} else if (groups.isFormulaSeries(series)) {
	    return FORMULA_STROKE;
	}
	return SOLID_STROKE;
    }

    /*
     * (non-Javadoc)
     * 
     * @see
     * org.jfree.chart.renderer.xy.XYLineAndShapeRenderer#getItemShapeVisible
     * (int, int)
     */
    @Override
    public boolean getItemShapeVisible(int series, int item) {
	return groups.isPoints(series);
    }

}
