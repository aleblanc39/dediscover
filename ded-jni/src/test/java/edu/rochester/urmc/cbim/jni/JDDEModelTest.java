package edu.rochester.urmc.cbim.jni;

import org.apache.commons.math3.util.Pair;
import org.junit.Before;
import org.junit.Test;

import java.util.*;
import java.util.concurrent.atomic.AtomicReference;
import java.util.stream.Collectors;
import java.util.stream.IntStream;

import static org.junit.Assert.*;

public class JDDEModelTest {

    static {
        System.loadLibrary("DEDiscover");
    }

    private JDDEModel model;
    private String equations =
            "dT_mE/dt = (rho_m * D_m - delta_m) * T_mE - (gamma_ms + gamma_ml) * T_mE\n" +
                    "dT_sE/dt = (rho_s * D_m - delta_s) * T_sE - gamma_sl * T_sE + gamma_ms * T_mE\n" +
                    "dT_lE/dt = gamma_ml * T_mE + gamma_sl * T_sE - delta_l * T_lE\n" +
                    "\n" +
                    "macro1 = 2*T_mE\n" +
                    "macro2 = 2*T_lE + 4\n" +
                    "// D_m is thus used to replace D_s in all calculations\n" +
                    "// fix tau=3.08\n" +
                    "Covariate{D_m, t-tau}";



    private double[] generateTimepoints(int nbPoints, final double first, final double incr) {
        double ret[] = new double[nbPoints];

        IntStream.range(0, nbPoints).forEach(i->{
            ret[i] = first + i*incr;
        });
        return ret;
    }

    /**
     * Generate values for testing. Just put random values.
     * @param nbTimepts
     * @param nbVars
     * @return
     */
    private double [][] generateDepValValues(int nbTimepts, int nbVars) {
        Random random = new Random();
        double ret[][] = new double [nbTimepts][nbVars];
        IntStream.range(0, nbTimepts).forEach(i->{
            IntStream.range(0, nbVars).forEach(j-> {
                ret[i][j] = random.nextDouble()*100;
            });
        });
        return ret;
    }

    private double [] generateModelParamValues(int nbParams) {
        Random rand = new Random();
        double [] ret = new double[nbParams];
        IntStream.range(0, nbParams).forEach(i-> {
            ret[i] = rand.nextDouble()*5.0;
        });
        return ret;
    }


    @Before
    public void setUp() throws Exception {
        model = new JDDEModel(equations);
    }


    @Test
    public void getInitialConditions() {
        JInitialCondition conds[] = model.getJInitialConditions().toArray(new JInitialCondition[0]);
        assertTrue(conds.length == 3);
    }

    @Test
    public void getErrorMessages() {
        JDDEModel model = new JDDEModel("dx/dt=a + b +++");
        assertTrue(model.getErrorMessages().size() > 0);
    }

    @Test
    public void getModelParameters() {
    }

    @Test
    public void updateJEquations() {
    }

    @Test
    public void interruptThread() {
    }

    @Test
    public void pauseThread() {
    }

    @Test
    public void resumeThread() {
    }


    @Test
    public void isTerminated() {
    }

    @Test
    public void terminate() {
    }

    @Test
    public void setBasePtr() {
    }

    @Test
    public void setObjectIndex() {
    }

    @Test
    public void testEquals() {
    }

    @Test
    public void testToString() {
    }

    @Test
    public void getObjectIndex() {
    }

    @Test
    public void testFinalize() {
    }


    /**
     * Create points to initialize covariates.Simple 4 timepoints
     * @param covs
     * @return
     */
    private Map<String, List<Pair<Double, Double>>> createCovariateInitialPoints(List<String> covs) {

        int nbPoints = 4;
        AtomicReference<Double> yValue = new AtomicReference<>((double) 0);

        Map<String, List<Pair<Double, Double>>> initialPoints = new HashMap<>();
        covs.forEach( name -> {
            List<Pair<Double, Double>> points = new ArrayList<>();
            for (int i = 0; i < nbPoints; i++) {
                points.add(new Pair<Double, Double>((double) i + 0.1, yValue.get()));
                yValue.updateAndGet(v -> new Double((double) (v + 0.5)));
            }
            initialPoints.put(name, points);
        });



        return initialPoints;
    }


    @Test
    public void computeMacroValues() {
        int nbTimepoints = 100;

        List<String> covNames = model.covariates.stream().map(x->x.getName()).collect(Collectors.toList());
        System.err.println("Covariate names:");
        covNames.forEach(System.err::println);
        model.covariates.forEach(x-> {
            x.setTimeVaryingFunction( new JTimeVaryingFunction("LOCF"));
        });

        double timepoints[] = generateTimepoints(nbTimepoints, 0, 0.1);

        double modelParamValues[] = generateModelParamValues(model.modelParameters.size());
        JXDECSVFile resultFile =  model.computeMacroValues(
                timepoints,
                generateDepValValues(nbTimepoints, model.getDependentVariableNames().size()),
                modelParamValues,
                createCovariateInitialPoints(covNames));

        System.out.println("Model param names");
        model.getJModelParameters().forEach(x->{
            System.out.println(x.getName());
        });

        Optional<JModelParameter> x = model.getJModelParameters().stream().filter(param->param.getName().equals("tau")).findFirst();
        int index = model.getJModelParameters().indexOf(x.get());
        System.out.println("Index of tau: " + index);
        System.out.println("Value of param (?): " + modelParamValues[index]);

        IntStream.range(0, resultFile.getNbTimepoints()).forEach(i->{
            System.out.print("" + timepoints[i] + ": ");
            IntStream.range(0, resultFile.getNbVariables()).forEach(j-> {
                System.out.print(resultFile.getData()[i][j] + " ");
            });
            System.out.println("");
        });
    }



    @Test
    public void findSymbolWithDisplayString() {
    }

    @Test
    public void findSymbolWithName() {
    }


    @Test
    public void getAllSymbols() {
    }

    @Test
    public void getDependentVariableNames() {
    }

    @Test
    public void testGetEquations() {
    }


    @Test
    public void getJInitialConditions() {
    }

    @Test
    public void getJMacros() {
    }

    @Test
    public void getJCovariates() {
    }

    @Test
    public void getJModelParameters() {
    }



    @Test
    public void testGetModelParameters() {
    }

    @Test
    public void getCovariates() {
    }

    @Test
    public void getNbDelayExpressions() {
    }


    @Test
    public void testUpdateJEquations() {
    }

    @Test
    public void testValidateFormula() {
        assertTrue(model.validateFormula("rho_m").length == 0);
        assertTrue(model.validateFormula("rho_m * 3").length == 0);
        assertTrue(model.validateFormula("rho_m * 3 +").length > 0);
    }
}
