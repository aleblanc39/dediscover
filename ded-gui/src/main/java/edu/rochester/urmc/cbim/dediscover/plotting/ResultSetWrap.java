package edu.rochester.urmc.cbim.dediscover.plotting;

import org.woodhill.ded.results.KResultData;

public class ResultSetWrap {

    public final KResultData data;
    public final String setName;

    public ResultSetWrap(KResultData data, String setName) {
        this.data = data;
        this.setName = setName;
    }
}
