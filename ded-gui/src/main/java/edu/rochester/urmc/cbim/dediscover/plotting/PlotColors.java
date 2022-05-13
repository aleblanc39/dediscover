
package edu.rochester.urmc.cbim.dediscover.plotting;

import java.awt.Color;
import java.awt.Paint;
import java.util.HashMap;
import java.util.Map;


public class PlotColors {

    private static final long serialVersionUID = 3685861837112392501L;

    /**
     * A very dark red color.
     */
    private static final Color VERY_DARK_RED = new Color(0x80, 0x00, 0x00);

    /**
     * A dark red color.
     */
    private static final Color DARK_RED = new Color(0xc0, 0x00, 0x00);

    /**
     * A light red color.
     */
    private static final Color LIGHT_RED = new Color(0xFF, 0x40, 0x40);

    /**
     * A very light red color.
     */
    private static final Color VERY_LIGHT_RED = new Color(0xFF, 0x80, 0x80);

    /**
     * A very dark yellow color.
     */
    private static final Color VERY_DARK_YELLOW = new Color(0x80, 0x80, 0x00);

    /**
     * A dark yellow color.
     */
    private static final Color DARK_YELLOW = new Color(0xC0, 0xC0, 0x00);

    /**
     * A very dark green color.
     */
    private static final Color VERY_DARK_GREEN = new Color(0x00, 0x80, 0x00);

    /**
     * A dark green color.
     */
    private static final Color DARK_GREEN = new Color(0x00, 0xC0, 0x00);

    /**
     * A light green color.
     */
    private static final Color LIGHT_GREEN = new Color(0x40, 0xFF, 0x40);

    /**
     * A very light green color.
     */
    private static final Color VERY_LIGHT_GREEN = new Color(0x80, 0xFF, 0x80);

    /**
     * A very dark cyan color.
     */
    private static final Color VERY_DARK_CYAN = new Color(0x00, 0x80, 0x80);

    /**
     * A dark cyan color.
     */
    private static final Color DARK_CYAN = new Color(0x00, 0xC0, 0xC0);

    /**
     * A light cyan color.
     */
    private static final Color LIGHT_CYAN = new Color(0x40, 0xFF, 0xFF);

    /**
     * Aa very light cyan color.
     */
    private static final Color VERY_LIGHT_CYAN = new Color(0x80, 0xFF, 0xFF);

    /**
     * A very dark blue color.
     */
    private static final Color VERY_DARK_BLUE = new Color(0x00, 0x00, 0x80);

    /**
     * A dark blue color.
     */
    private static final Color DARK_BLUE = new Color(0x00, 0x00, 0xC0);

    /**
     * A light blue color.
     */
    private static final Color LIGHT_BLUE = new Color(0x40, 0x40, 0xFF);

    /**
     * A very light blue color.
     */
    private static final Color VERY_LIGHT_BLUE = new Color(0x80, 0x80, 0xFF);

    /**
     * A very dark magenta/purple color.
     */
    private static final Color VERY_DARK_MAGENTA = new Color(0x80, 0x00, 0x80);

    /**
     * A dark magenta color.
     */
    private static final Color DARK_MAGENTA = new Color(0xC0, 0x00, 0xC0);

    /**
     * A light magenta color.
     */
    private static final Color LIGHT_MAGENTA = new Color(0xFF, 0x40, 0xFF);

    /**
     * A very light magenta color.
     */
    private static final Color VERY_LIGHT_MAGENTA = new Color(0xFF, 0x80, 0xFF);

    /**
     * Convenience method to return an array of <code>Paint</code> objects that
     * represent the pre-defined colors in the <code>Color<code> and
     * <code>ChartColor</code> objects.
     * <p>
     * Avoid yellow color which is not obvious than other colors
     *
     * @return An array of objects with the <code>Paint</code> interface.
     */
    private static Paint[] colors = new Paint[]{
            new Color(0xFF, 0x55, 0x55),
            new Color(0x55, 0x55, 0xFF),
            new Color(0x55, 0xFF, 0x55),
            // new Color(0xFF, 0xFF, 0x55),
            new Color(0xFF, 0x55, 0xFF), new Color(0x55, 0xFF, 0xFF),
            Color.pink, Color.gray, DARK_RED, DARK_BLUE, DARK_GREEN,
            DARK_YELLOW, DARK_MAGENTA, DARK_CYAN, Color.darkGray, LIGHT_RED,
            LIGHT_BLUE, LIGHT_GREEN, LIGHT_MAGENTA, LIGHT_CYAN, VERY_DARK_RED,
            VERY_DARK_BLUE, VERY_DARK_GREEN, VERY_DARK_YELLOW,
            VERY_DARK_MAGENTA, VERY_DARK_CYAN, VERY_LIGHT_RED, VERY_LIGHT_BLUE,
            VERY_LIGHT_GREEN,
            // ColorUtility.VERY_LIGHT_YELLOW,
            VERY_LIGHT_MAGENTA, VERY_LIGHT_CYAN};

    private static Map<Object, PlotColors> colorUtilMap = new HashMap<Object, PlotColors>();

    /**
     * Get the color utility for the given key.
     *
     * @param key
     * @return
     */
    public static PlotColors makePlotColorUtil(Object key) {
        if (!colorUtilMap.containsKey(key)) {
            colorUtilMap.put(key, new PlotColors());
        }
        return colorUtilMap.get(key);
    }

    private int index = 0;

    /**
     * @return
     */
    public Paint nextColor() {
        Paint p = colors[index++];
        index = index % colors.length;
        return p;
    }
}
