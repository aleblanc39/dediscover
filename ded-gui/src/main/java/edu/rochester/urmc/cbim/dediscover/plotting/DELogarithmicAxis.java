
package edu.rochester.urmc.cbim.dediscover.plotting;

import java.awt.Graphics2D;
import java.awt.geom.Rectangle2D;
import java.text.NumberFormat;
import java.util.List;

import org.jfree.chart.axis.LogarithmicAxis;
import org.jfree.chart.axis.NumberTick;
import org.jfree.chart.ui.RectangleEdge;
import org.jfree.chart.ui.TextAnchor;
import org.jfree.data.Range;


public class DELogarithmicAxis extends LogarithmicAxis {

    private int maxNumTickLabesl;

    /**
     * @param label
     */
    public DELogarithmicAxis(String label, int maxNumTickLabels) {
        super(label);
        this.maxNumTickLabesl = maxNumTickLabels;
    }

    /*
     * (non-Javadoc)
     *
     * @see
     * org.jfree.chart.axis.LogarithmicAxis#refreshTicksVertical(java.awt.Graphics2D
     * , java.awt.geom.Rectangle2D, org.jfree.ui.RectangleEdge)
     */
    @Override
    protected List refreshTicksVertical(Graphics2D g2, Rectangle2D dataArea,
                                        RectangleEdge edge) {
		//System.err.println("Refreshing ticks");

        List ticks = new java.util.ArrayList();

        // get lower bound value:
        double lowerBoundVal = getRange().getLowerBound();
        // if small log values and lower bound value too small
        // then set to a small value (don't allow <= 0):
        if (this.smallLogFlag && (lowerBoundVal < SMALL_LOG_VALUE)) {
            lowerBoundVal = SMALL_LOG_VALUE;
        }
        // get upper bound value
        double upperBoundVal = getRange().getUpperBound();

        // get log10 version of lower bound and round to integer:
        int iBegCount = (int) Math.rint(switchedLog10(lowerBoundVal));
        // get log10 version of upper bound and round to integer:
        int iEndCount = (int) Math.rint(switchedLog10(upperBoundVal));

        if ((iBegCount == iEndCount) && (iBegCount > 0)
                && (Math.pow(10, iBegCount) > lowerBoundVal)) {
            // only 1 power of 10 value, it's > 0 and its resulting
            // tick value will be larger than lower bound of data
            --iBegCount; // decrement to generate more ticks
        }

        double tickVal;
        String tickLabel;
        boolean zeroTickFlag = false;

        int skipFactor = (iEndCount - iBegCount) / maxNumTickLabesl;

        for (int i = iBegCount; i <= iEndCount; i++) {
            // for each tick with a label to be displayed
            int jEndCount = isMinorTickMarksVisible() ? 10 : 1;
            if (i == iEndCount) {
                jEndCount = 1;
            }
            boolean skip = !(i % (skipFactor + 1) == 0);

            for (int j = 0; j < jEndCount; j += skipFactor + 1) {
                // for each tick to be displayed
                if (this.smallLogFlag) {
                    // small log values in use
                    tickVal = Math.pow(10, i) + (Math.pow(10, i) * j);
                    if (!skip && (j == 0)) {
                        // first tick of group; create label text
                        if (this.log10TickLabelsFlag) {
                            // if flag then
                            tickLabel = "10^" + i; // create "log10"-type label
                        } else { // not "log10"-type label
                            if (this.expTickLabelsFlag) {
                                // if flag then
                                tickLabel = "1e" + i; // create "1e#"-type label
                            } else { // not "1e#"-type label
                                if (i >= 0) { // if positive exponent then
                                    // make integer
                                    NumberFormat format = getNumberFormatOverride();
                                    if (format != null) {
                                        tickLabel = format.format(tickVal);
                                    } else {
                                        tickLabel = Long.toString((long) Math
                                                .rint(tickVal));
                                    }
                                } else {
                                    // negative exponent; create fractional
                                    // value
                                    // set exact number of fractional digits to
                                    // be shown:
                                    this.numberFormatterObj
                                            .setMaximumFractionDigits(-i);
                                    // create tick label:
                                    tickLabel = this.numberFormatterObj
                                            .format(tickVal);
                                }
                            }
                        }
                    } else { // not first tick to be displayed
                        tickLabel = ""; // no tick label
                    }
                } else { // not small log values in use; allow for values <= 0
                    if (zeroTickFlag) { // if did zero tick last iter then
                        --j;
                    } // decrement to do 1.0 tick now
                    tickVal = (i >= 0) ? Math.pow(10, i)
                            + (Math.pow(10, i) * j)
                            : -(Math.pow(10, -i) - (Math.pow(10, -i - 1) * j));
                    if (!skip && (j == 0)) { // first tick of group
                        if (!zeroTickFlag) { // did not do zero tick last
                            // iteration
                            if ((i > iBegCount) && (i < iEndCount)
                                    && (Math.abs(tickVal - 1.0) < 0.0001)) {
                                // not first or last tick on graph and value
                                // is 1.0
                                tickVal = 0.0; // change value to 0.0
                                zeroTickFlag = true; // indicate zero tick
                                tickLabel = "0"; // create label for tick
                            } else {
                                // first or last tick on graph or value is 1.0
                                // create label for tick:
                                if (this.log10TickLabelsFlag) {
                                    // create "log10"-type label
                                    tickLabel = (((i < 0) ? "-" : "") + "10^" + Math
                                            .abs(i));
                                } else {
                                    if (this.expTickLabelsFlag) {
                                        // create "1e#"-type label
                                        tickLabel = (((i < 0) ? "-" : "")
                                                + "1e" + Math.abs(i));
                                    } else {
                                        NumberFormat format = getNumberFormatOverride();
                                        if (format != null) {
                                            tickLabel = format.format(tickVal);
                                        } else {
                                            tickLabel = Long
                                                    .toString((long) Math
                                                            .rint(tickVal));
                                        }
                                    }
                                }
                            }
                        } else { // did zero tick last iteration
                            tickLabel = ""; // no label
                            zeroTickFlag = false; // clear flag
                        }
                    } else { // not first tick of group
                        tickLabel = ""; // no label
                        zeroTickFlag = false; // make sure flag cleared
                    }
                }

                if (tickVal > upperBoundVal) {
                    return ticks; // if past highest data value then exit method
                }

                if (tickVal >= lowerBoundVal - SMALL_LOG_VALUE) {
                    // tick value not below lowest data value
                    TextAnchor anchor = null;
                    TextAnchor rotationAnchor = null;
                    double angle = 0.0;
                    if (isVerticalTickLabels()) {
                        if (edge == RectangleEdge.LEFT) {
                            anchor = TextAnchor.BOTTOM_CENTER;
                            rotationAnchor = TextAnchor.BOTTOM_CENTER;
                            angle = -Math.PI / 2.0;
                        } else {
                            anchor = TextAnchor.BOTTOM_CENTER;
                            rotationAnchor = TextAnchor.BOTTOM_CENTER;
                            angle = Math.PI / 2.0;
                        }
                    } else {
                        if (edge == RectangleEdge.LEFT) {
                            anchor = TextAnchor.CENTER_RIGHT;
                            rotationAnchor = TextAnchor.CENTER_RIGHT;
                        } else {
                            anchor = TextAnchor.CENTER_LEFT;
                            rotationAnchor = TextAnchor.CENTER_LEFT;
                        }
                    }
                    // create tick object and add to list:
                    ticks.add(new NumberTick(new Double(tickVal), tickLabel,
                            anchor, rotationAnchor, angle));
                }
            }
        }
        return ticks;
    }

    /**
     * Override is necessary because JFreeChart LogarithmicAxis does not honor
     * the autoRangeIncludesZero setting
     */
    @Override
    public void autoAdjustRange() {
        super.autoAdjustRange();
        if (getAutoRangeIncludesZero() && (getRange().getLowerBound() > 0.0d)) {
            setRange(new Range(0.0d, getRange().getUpperBound()), false, false);
        }

    }

}
