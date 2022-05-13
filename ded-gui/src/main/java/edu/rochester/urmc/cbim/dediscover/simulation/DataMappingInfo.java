
package edu.rochester.urmc.cbim.dediscover.simulation;

import com.google.common.collect.HashMultimap;
import com.google.common.collect.Multimap;
import org.woodhill.ded.models.KCovariate;
import org.woodhill.ded.models.KMappingInfo;
import org.woodhill.ded.models.KMappingInfoSet;
import org.woodhill.ded.models.TVFunctionMapping;

import java.util.*;


/**
 * This class collects and organize the information on the data mapping to be used when creating the
 * plots based on the data table.
 *
 * For each column of the data table, the mapping can be:
 * - Ignore
 * - Plot Only
 * - Estimation -- map to a dependent variable
 * - Estimation -- map to a formula
 * - Covariate
 *
 * Will track cases where multiple columns map to the same variable or formula.
 */
public class DataMappingInfo {


    public final String modelSymbolName;
    public final List<String> dataColumnName;
    public final String formula;

    private DataMappingInfo(String modelSymbolName, String formula,
                            Collection<String> dataColumnNames) {
        this.modelSymbolName = modelSymbolName;
        this.dataColumnName = new ArrayList<>();
        this.dataColumnName.addAll(dataColumnNames);
        this.formula = formula;
    }

    /*
     * (non-Javadoc)
     *
     * @see java.lang.Object#toString()
     */
    @Override
    public String toString() {
        return "DataMappingInfo [modelSymbolName=" + modelSymbolName
                + ", dataColumnName=" + dataColumnName + ", formula=" + formula
                + "]";
    }

    public static List<DataMappingInfo> getDataMappings(KMappingInfoSet mis, List<KCovariate> covariates, Set<String> depVarNames) {
        Multimap<String, String> nonFormulas = HashMultimap.create();
        Multimap<String, String> formulas = HashMultimap.create();

        for (KMappingInfo info : mis.getMappings().values()) {
            if (isValidMappedColumn(info)) {
                if (info.isFormula()) {
                    formulas.put(info.getFormula(), info.getColumn_name());
                } else {
                    nonFormulas.put(getMapping(info, covariates), info.getColumn_name());
                }
            }
        }
        List<DataMappingInfo> infoList = new ArrayList<>();

        for (String key : nonFormulas.keySet()) {
            // Plot only column
            infoList.add(new DataMappingInfo(null, null, nonFormulas.get(key)));
        }

        for (String key : formulas.keySet()) {
            if (depVarNames.contains(key)) {
                infoList.add(new DataMappingInfo(key, null, formulas.get(key)));
            } else {
                infoList.add(new DataMappingInfo(null, key, formulas
                        .get(key)));
            }
        }


        return infoList;
    }


    /**
     * Valid mapped column: Covariate, estimation, or plot only
     * valid mapping
     *
     * @param mapInfo
     * @return
     */


    static Set<KMappingInfo.MappingType> validUsedFor =
            new HashSet<>(Arrays.asList(/**KMappingInfo.MappingType.COVARIATE,**/KMappingInfo.MappingType.ESTIMATION, KMappingInfo.MappingType.PLOT_ONLY));

    private static boolean isValidMappedColumn(KMappingInfo mapInfo) {

        if (mapInfo.getStatus() == KMappingInfo.MappingStatus.FAILED ||
                !validUsedFor.contains(mapInfo.getUsedFor()))
            return false;

        return true;
    }

    /**
     * @return
     */
    public static String getModelSymbolMappedTo(KMappingInfoSet mis, List<KCovariate> covariates, String dataName) {
        KMappingInfo mappingInfo = mis.getMappings().get(dataName);
        if (mappingInfo == null || !isValidMappedColumn(mappingInfo)) {
            return "";
        }
        return getMapping(mappingInfo, covariates);
    }

    private static String getMapping(KMappingInfo info, List<KCovariate> covariates) {
        return (info.getUsedFor().equals(KMappingInfo.MappingType.COVARIATE)) ? findModelSymbolForCovariateColumn(
                info, covariates) : info.getColumn_name();
    }

    /**
     *
     */
    private static String findModelSymbolForCovariateColumn(
            KMappingInfo info, List<KCovariate> covariates) {
        for (KCovariate jtv : covariates) {
            TVFunctionMapping tvfm = TVFunctionMapping.Companion.getMappingForCovariate(jtv);
            if (tvfm.getTableColumns().contains(info)) {
                return jtv.getName();
            }
        }
        System.out.println("WARNING: Column " + info.getColumn_name() + " defines maps to covariate but no covariate defined.");
        return null;
    }


    /**
     * @return
     */
    public static boolean isFormula(KMappingInfoSet mis, String formula) {
        for (KMappingInfo m : mis.getMappings().values()) {
            if (isValidMappedColumn(m) && m.isFormula() && m.getFormula().equals(formula)) {
                return true;
            }
        }
        return false;

    }
}