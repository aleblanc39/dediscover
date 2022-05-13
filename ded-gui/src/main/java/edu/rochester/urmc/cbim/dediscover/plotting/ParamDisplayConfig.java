
package edu.rochester.urmc.cbim.dediscover.plotting;
import org.woodhill.ded.results.KPlotResultsModel;

public class ParamDisplayConfig extends DatasetDisplayConfig {

    public ParamDisplayConfig(String plotTitlePrefix, String name, String chartVar, boolean log,
	     KPlotResultsModel simulationResultsModel,
	    boolean hasData,
                              java.util.function.Function<String, SeriesGrouping> seriesGroupingsGen,
	    String[] seriesKeys, String... setNames) {

        super(plotTitlePrefix + " - " + (name != null ? (name + " - ") : "") + chartVar, log,
		simulationResultsModel, seriesGroupingsGen, seriesKeys, setNames);
    }

    /*
     * (non-Javadoc)
     * 
     * @see
     * edu.rochester.urmc.cbim.dediscover.plotting.DatasetDisplayConfig#hasLegend
     * ()
     */
    @Override
    public boolean hasLegend() {
        return true;
//	return this.simmodel instanceof SensitivityTestModel ? false : (this
//		.getSetNames().length > 0 ? super.hasLegend() : false);
    }

}
