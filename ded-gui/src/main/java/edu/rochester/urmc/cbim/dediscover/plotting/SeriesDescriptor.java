
package edu.rochester.urmc.cbim.dediscover.plotting;

import org.woodhill.ded.ui.chartoptions.ParameterChartOptions;


public class SeriesDescriptor {

    public final int seriesId;
    public int primarySeriesId;
    private final ParameterChartOptions.DisplayStyle style;
    private final boolean data;
    private final boolean mean;
    private final boolean formula;
    public final String name;
    private final String title;

    public SeriesDescriptor(String title, String name, int id, int primaryId,
                            ParameterChartOptions.DisplayStyle style, boolean data,
                            boolean mean, boolean formula) {
        this.seriesId = id;
        this.primarySeriesId = primaryId;
        this.style = style;
        this.data = data;
        this.mean = mean;
        this.formula = formula;
        this.name = name;
        this.title = title;
    }

    public SeriesDescriptor(String title, String name, int id,
                            ParameterChartOptions.DisplayStyle style, boolean formula) {
        this(title, name, id, -1, style, false, false, formula);
    }

    /**
     * @return
     */
    public boolean isMean() {
        return mean;
    }

    /*
     * (non-Javadoc)
     *
     * @see java.lang.Object#hashCode()
     */
    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + ((name == null) ? 0 : name.hashCode());
        result = prime * result + ((title == null) ? 0 : title.hashCode());
        return result;
    }

    /*
     * (non-Javadoc)
     *
     * @see java.lang.Object#equals(java.lang.Object)
     */
    @Override
    public boolean equals(Object obj) {
        if (this == obj) {
            return true;
        }
        if (obj == null) {
            return false;
        }
        if (getClass() != obj.getClass()) {
            return false;
        }
        SeriesDescriptor other = (SeriesDescriptor) obj;
        if (name == null) {
            if (other.name != null) {
                return false;
            }
        } else if (!name.equals(other.name)) {
            return false;
        }
        if (title == null) {
            if (other.title != null) {
                return false;
            }
        } else if (!title.equals(other.title)) {
            return false;
        }
        return true;
    }

    /**
     * @return
     */
    public boolean isData() {
        return data;
    }

    /**
     * @return
     */
    public boolean isPrimary() {
        return primarySeriesId < 0;
    }

    /**
     * @return
     */
    public boolean isPoints() {
        return (style == ParameterChartOptions.DisplayStyle.BOTH) || (style == ParameterChartOptions.DisplayStyle.SHAPE);
    }

    /**
     * @return
     */
    public boolean isCurve() {
        return (style == ParameterChartOptions.DisplayStyle.BOTH) || (style == ParameterChartOptions.DisplayStyle.CURVE);
    }

    /**
     * @return
     */
    public int getPrimarySeriesId() {
        return primarySeriesId < 0 ? seriesId : primarySeriesId;
    }

    /**
     * @return
     */
    public boolean isFormula() {
        return formula;
    }

}
