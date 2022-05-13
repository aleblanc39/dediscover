
package edu.rochester.urmc.cbim.dediscover.simulation;

import java.text.DecimalFormat;
import java.text.FieldPosition;
import java.text.NumberFormat;


public class ScientificFormatter extends DecimalFormat {

    private NumberFormat scienceFormatter = new DecimalFormat("0.0######E0");
    private NumberFormat simpleFormatter = new DecimalFormat("##0.0######");

    boolean useScientific = true;
    int minDigits = 4;

    /**
     *
     */
    public ScientificFormatter(boolean useScientific, int minDigits) {
        super();
        this.useScientific = useScientific;
        this.minDigits = minDigits;
        setup(minDigits);

    }

    private void setup(int digits) {
        char di = '0';
        StringBuilder buf = new StringBuilder();
        for (int i = 0; i < digits - 1; i++) {
            buf.append(di);
        }
        scienceFormatter = new DecimalFormat("0.0" + buf.toString() + "E00##");
        scienceFormatter.setMaximumFractionDigits(digits);
        simpleFormatter = new DecimalFormat("##0.0" + buf.toString());
        simpleFormatter.setMaximumFractionDigits(digits);
    }

    /*
     * (non-Javadoc)
     *
     * @see java.text.DecimalFormat#format(double, java.lang.StringBuffer,
     * java.text.FieldPosition)
     */
    @Override
    public StringBuffer format(double number, StringBuffer result,
                               FieldPosition fieldPosition) {
        if (useScientific
                && ((Math.abs(number) >= 1000d) || ((Math.abs(number) > 0d) && (Math
                .abs(number) < .001d)))) {
            return scienceFormatter.format(number, result, fieldPosition);
        } else {
            return simpleFormatter.format(number, result, fieldPosition);
        }
    }

}
