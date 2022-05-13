
package edu.rochester.urmc.cbim.dediscover.plotting;

import org.woodhill.ded.results.KPlotResultsModel;

import java.util.function.Function;


public class CombinedDisplayConfig extends DatasetDisplayConfig {

    public CombinedDisplayConfig(
            String plotTitlePrefix, boolean log,
            KPlotResultsModel model,
            Function<String, SeriesGrouping> seriesGroupings,
            String[] seriesKeys, String... setNames) {
        super(plotTitlePrefix, log, model, seriesGroupings, seriesKeys, setNames);
    }

}
