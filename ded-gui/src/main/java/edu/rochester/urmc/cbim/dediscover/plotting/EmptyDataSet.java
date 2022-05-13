
package edu.rochester.urmc.cbim.dediscover.plotting;


public class EmptyDataSet extends XDEDataSet {

    /*
     * (non-Javadoc)
     * 
     * @see org.jfree.data.xy.XYDataset#getItemCount(int)
     */
    @Override
    public int getItemCount(int arg0) {
	return 0;
    }

    /*
     * (non-Javadoc)
     * 
     * @see org.jfree.data.xy.XYDataset#getX(int, int)
     */
    @Override
    public Number getX(int arg0, int arg1) {
	return null;
    }

    /*
     * (non-Javadoc)
     * 
     * @see org.jfree.data.xy.XYDataset#getY(int, int)
     */
    @Override
    public Number getY(int arg0, int arg1) {
	return null;
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
	return null;
    }/*
      * (non-Javadoc)
      * 
      * @see
      * edu.rochester.urmc.cbim.dediscover.simulation.XDEDataSet#isFormula(int)
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
    protected boolean isMean(int i) {
	return false;
    }

    /*
     * (non-Javadoc)
     * 
     * @see edu.rochester.urmc.cbim.dediscover.simulation.XDEDataSet#isData(int)
     */
    @Override
    protected boolean isData(int i) {
	return false;
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
     * @see
     * edu.rochester.urmc.cbim.dediscover.simulation.XDEDataSet#findPrimarySeries
     * (int)
     */
    @Override
    protected int findPrimarySeries(int i) {
	return 0;
    }

    /*
     * (non-Javadoc)
     * 
     * @see org.jfree.data.general.AbstractSeriesDataset#getSeriesCount()
     */
    @Override
    public int getSeriesCount() {
	return 0;
    }

    /*
     * (non-Javadoc)
     * 
     * @see org.jfree.data.general.AbstractSeriesDataset#getSeriesKey(int)
     */
    @Override
    public Comparable getSeriesKey(int arg0) {
	return null;
    }

}
