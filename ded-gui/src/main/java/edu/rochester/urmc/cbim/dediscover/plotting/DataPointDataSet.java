package edu.rochester.urmc.cbim.dediscover.plotting;

import java.util.*;
import java.util.function.Function;

import edu.rochester.urmc.cbim.dediscover.simulation.DataMappingInfo;
import edu.rochester.urmc.cbim.dediscover.simulation.DataSeries;
import edu.rochester.urmc.cbim.jni.exception.XDEException;
import org.apache.commons.lang3.ArrayUtils;
import org.woodhill.ded.ui.chartoptions.ChartableControlsTB;
import org.woodhill.ded.ui.chartoptions.ParameterChartOptions;

public class DataPointDataSet extends XDEDataSet {

    /**
     * Class will contain data from datatable and replicates. It contains several time series indexed numerically.
     * Series with index below names.size() correspond to data directly from table columns, stored in data
     * Series with index >= names.size() correspond to replicates, with series indexed K gets information
     * from replicates[K-names.size()]
     */

    private final List<String> names;
    private final Map<String, DataSeries> data;
    private final List<DataMappingInfo> replicates;

    /**
     * @param names
     * @param data
     * @param replicates
     */
    private DataPointDataSet(List<String> names, Map<String, DataSeries> data,
                             List<DataMappingInfo> replicates) {
        this.names = names;
        this.data = data;
        this.replicates = replicates;
    }

    /**
     * Get data from datatable, as well as replicates, formulas
     *
     */
    private static XDEDataSet getDataPointSet(ChartableControlsTB chartOptions,
                                              Set<String> namesAllowed,
                                              MappingInfoInterface mappingInfo,
                                              List<DataMappingInfo> dataMappings,
                                              boolean log,
                                              boolean logMatters) {
        try {

            if (mappingInfo.timepoints().size() == 0 || chartOptions.getVisibleDataColumns().size() == 0)
                return new EmptyDataSet();

            final List<String> names = new ArrayList<>();
            Set<String> valid = new HashSet<>();
            List<DataMappingInfo> replicates = new ArrayList<>();

            for (DataMappingInfo dataMappingInfo : dataMappings) {
                if (namesAllowed.contains(dataMappingInfo.modelSymbolName)
                        || namesAllowed.contains(dataMappingInfo.formula)) {
                    // TODO THIS IS BAD!!! SHOULDN'T BE BOTHERED ABOUT KNOWING HOW TO CREATE KEY HERE!!!
                    for (String s : dataMappingInfo.dataColumnName) {
                        if (chartOptions.isVisible(s
                                + ChartableControlsTB.DATA_KEY)
                                && (!logMatters || (chartOptions.isLog(s
                                + ChartableControlsTB.DATA_KEY) == log))) {

                            names.add(s);
                        }
                    }
                    if (dataMappingInfo.dataColumnName.size() > 1
                            && (chartOptions.contains(dataMappingInfo.modelSymbolName
                            + "_MEAN"))
                            && chartOptions.isVisible(dataMappingInfo.modelSymbolName
                            + "_MEAN")
                            && (!logMatters || (chartOptions
                            .isLog(dataMappingInfo.modelSymbolName + "_MEAN") == log))) {
                        replicates.add(dataMappingInfo);
                    }

                }
                valid.addAll(dataMappingInfo.dataColumnName);
            }

            final Map<String, DataSeries> data = new HashMap<>();
            for (int i = 0; i < mappingInfo.nbMappings(); i++) {
                if (valid.contains(mappingInfo.names(i))) {
                    List<Double> dataAtCol = mappingInfo.data(i);
                    data.put(mappingInfo.names(i), new DataSeries(ArrayUtils.toPrimitive(mappingInfo.timepoints().toArray(new Double[mappingInfo.timepoints().size()])),
                            ArrayUtils.toPrimitive(dataAtCol.toArray(new Double[dataAtCol.size()]))));
                }
            }
            ;

            data.keySet().forEach(System.out::println);
            return new DataPointDataSet(names, data, replicates);
        } catch (XDEException e) {
            return new EmptyDataSet();
        }
    }



    /*
     * (non-Javadoc)
     *
     * @see org.jfree.data.xy.AbstractXYDataset#getYValue(int, int)
     */
    @Override
    public double getYValue(int series, int item) {
        if (series < names.size()) {
            return data.get(names.get(series)).yValues[item];
        } else {
            int count = 0;
            double sum = 0;
            for (String dataname : replicates.get(series - names.size()).dataColumnName) {
                sum += data.get(dataname).yValues[item];
                count++;
            }
            return sum / count;
        }
    }

    @Override
    public Number getY(int arg0, int arg1) {
        return null;
    }

    /*
     * (non-Javadoc)
     *
     * @see edu.rochester.urmc.cbim.dediscover.simulation.XDEDataSet#isData(int)
     */
    @Override
    protected boolean isData(int series) {
        return series < names.size();
    }

    /*
     * (non-Javadoc)
     *
     * @see edu.rochester.urmc.cbim.dediscover.simulation.XDEDataSet#isFormula
     * (int)
     */
    @Override
    protected boolean isFormula(int i) {
        return false;
    }

    /*
     * (non-Javadoc)
     *
     * @see edu.rochester.urmc.cbim.dediscover.simulation.XDEDataSet#isMean(int)
     */
    @Override
    protected boolean isMean(int series) {
        return series >= names.size();
    }

    /*
     * (non-Javadoc)
     *
     * @see
     * edu.rochester.urmc.cbim.dediscover.simulation.XDEDataSet#isPrimarySeries
     * (int)
     */
    @Override
    protected boolean isPrimarySeries(int i) {
        return false;
    }

    /*
     * (non-Javadoc)
     *
     * @see edu.rochester.urmc.cbim.dediscover.simulation.XDEDataSet#
     * findPrimarySeries(int)
     */
    @Override
    protected int findPrimarySeries(int i) {
        return -1;
    }

    /*
     * (non-Javadoc)
     *
     * @see org.jfree.data.xy.AbstractXYDataset#getXValue(int, int)
     */
    @Override
    public double getXValue(int series, int item) {
        if (series < names.size()) {
            return data.get(names.get(series)).xValues[item];
        } else {
            int count = 0;
            double sum = 0;
            for (String dataname : replicates.get(series - names.size()).dataColumnName) {
                sum += data.get(dataname).xValues[item];
                count++;
            }
            return sum / count;
        }
    }

    @Override
    public Number getX(int arg0, int arg1) {
        return null;
    }

    @Override
    public int getItemCount(int series) {
        return series < names.size() ? data.get(names.get(series)).xValues.length
                : data.get(replicates.get(series - names.size()).dataColumnName
                .get(0)).xValues.length;
    }

    @Override
    public Comparable<String> getSeriesKey(int series) {
        return series < names.size() ? "Data (" + names.get(series) + ")"
                : "Mean ("
                + replicates.get(series - names.size()).modelSymbolName
                + ")";
    }

    @Override
    public int getSeriesCount() {
        return names.size() + replicates.size();
    }

    /*
     * (non-Javadoc)
     *
     * @see
     * edu.rochester.urmc.cbim.dediscover.simulation.XDEDataSet#matchesPrimary
     * (int, java.lang.String)
     */
    @Override
    public boolean matchesPrimary(int series, String seriesParam,
                                  Function<String, String> symbolMappedTo) {
        return series < names.size() ? seriesParam.equals(symbolMappedTo.apply(getSeriesDataname(series)))
                : seriesParam
                .equals(replicates.get(series - names.size()).modelSymbolName);
    }

    private String getSeriesDataname(int series) {
        return series < names.size() ? names.get(series) : replicates
                .get(series - names.size()).modelSymbolName + "_MEAN";
    }

    /*
     * (non-Javadoc)
     *
     * @see
     * edu.rochester.urmc.cbim.dediscover.simulation.XDEDataSet#getSeriesParam
     * (int)
     */
    @Override
    public String getSeriesParam(int series) {
        return series < names.size() ? names.get(series)
                + ChartableControlsTB.DATA_KEY : replicates.get(series
                - names.size()).modelSymbolName
                + "_MEAN";
    }

    @Override
    public SeriesGroup getShapeSeriesSet(
            Map<String, ParameterChartOptions> config, ParameterChartOptions.DisplayStyle style) {
        return null;
    }


    /**
     * @param chartVar
     * @return
     */
    public static XDEDataSet getDataPointSet(ChartableControlsTB tb, String chartVar, MappingInfoInterface mappingInfo,
                                             List<DataMappingInfo> dataMappings, boolean log,
                                             boolean logMatters) {
        Set<String> allowed = new HashSet<>();
        allowed.add(chartVar);
        return getDataPointSet(tb, allowed, mappingInfo, dataMappings, log, logMatters);
    }
    /**
     *
     * @param tb
     * @param log
     * @param logMatters
     * @return
     */
    public static XDEDataSet getDataPointSet(ChartableControlsTB tb, MappingInfoInterface mappingInfo,
                                             List<DataMappingInfo> dataMappings,  boolean log, boolean logMatters) {
        return getDataPointSet(tb, new HashSet<String>() {
            @Override
            public boolean contains(Object o) {
                return true;
            }
        }, mappingInfo, dataMappings, log, logMatters);
    }

}
