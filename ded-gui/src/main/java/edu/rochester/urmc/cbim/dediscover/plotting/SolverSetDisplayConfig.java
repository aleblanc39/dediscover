
package edu.rochester.urmc.cbim.dediscover.plotting;

import org.woodhill.ded.results.AbstractChartableResultModel;
import org.woodhill.ded.results.SimulationResultWrapper;

import java.util.function.Function;


public class SolverSetDisplayConfig extends DatasetDisplayConfig {


    public SolverSetDisplayConfig(String plotTitlePrefix, SimulationResultWrapper solverResultWrapper,
								  boolean log,
								  AbstractChartableResultModel model,
								  Function<String, SeriesGrouping> seriesGroupings, String[] seriesKeys) {
	super(plotTitlePrefix + " - " +solverResultWrapper.getSetName(), log,  model,
		seriesGroupings, seriesKeys, solverResultWrapper.getSetName());
    }

}
