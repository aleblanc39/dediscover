
package edu.rochester.urmc.cbim.dediscover.plotting;

import java.awt.Color;


public class SeriesDisplayConfig {
    public final Color color;
    public final StrokeType stroke;
    public final boolean curve;
    public final boolean points;
    public final ShapeType pointShape;

    public SeriesDisplayConfig(Color color, StrokeType strokeType,
	    boolean curve, boolean points, ShapeType shape) {
	this.color = color;
	this.stroke = strokeType;
	this.curve = curve;
	this.points = points;
	this.pointShape = shape;
    }

}
