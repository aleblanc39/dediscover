package edu.rochester.urmc.cbim.dediscover.simulation;

import org.jfree.data.xy.XYDataset;

import java.util.List;

/**
 * Complete rewrite of the original class. At this point only want to implement the "Hello world" for dediscover
 * in tornado, meaning I'm only interested in the plotting the default solver result.
 */
public interface PlotResultModel {

    List<List<XYDataset>>  getDataSets();

}
