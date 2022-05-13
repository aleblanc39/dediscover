
package edu.rochester.urmc.cbim.dediscover.plotting;

import java.awt.Shape;

import org.jfree.chart.util.ShapeUtils;


public class PlotShapes {

    private final static Shape[] shapes = new Shape[]{
            ShapeUtils.createRegularCross(2, 2),
            ShapeUtils.createDownTriangle(2),
            ShapeUtils.createUpTriangle(2),
            ShapeUtils.createDiamond(2),
            ShapeUtils.createDiagonalCross(1, 1)};

    private static int index = 0;

    /**
     * @return
     */
    public static Shape nextShape() {
        Shape p = shapes[index++];
        index = index % shapes.length;
        return p;
    }
}
