package edu.rochester.urmc.cbim.dediscover.plotting;

import java.util.List;

public interface MappingInfoInterface {

    int nbMappings();
    List<Double> timepoints();
    String names(int i);
    List<Double> data(int i);
}
