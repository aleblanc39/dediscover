#include <model/XDEModel.h>
#include <solver/SambineDDEWrapper.h>
#include <boost/make_shared.hpp>
#include <string>

using namespace std;

ModelPtr model = std::make_shared<XDEModel>(
    "//formula\n\
dEP/dt 	= delta_E*(EP[t0]-EP) - beta_E*EP*V\n\
dEPs/dt = beta_E*EP*V - k_E*EPs*gamma*TE[t-tau_T] - delta_Es*EPs\n\
dV/dt 	= pi_V*EPs - c_V*V - k_V*V*A\n\
dD/dt 	= delta_D*(D[t0]-D) - beta_D*D*V\n\
dDs/dt 	= beta_D*D*V - delta_Ds*Ds\n\
dDM/dt 	= k_D*Ds[t-tau_D] - delta_DM*DM\n\
dHN/dt 	= delta_HN*(HN[t0]-HN) - pi_H_DM*HN\n\
dHE/dt 	= pi_H_DM*HN + rho_HE_DM*HE - delta_HE_DM*HE\n\
dTN/dt 	= delta_TN*(TN[t0]-TN) - pi_T_DM*TN\n\
dTE/dt 	= pi_T_DM*TN + rho_TE_DM*TE - delta_TE_DM*TE\n\
dBN/dt 	= delta_B*(BN[t0]-BN) - pi_B_DM*BN\n\
dBA/dt 	= pi_B_DM*BN + rho_BA_DM_h_HE*BA - delta_BA*BA - pi_S*BA - pi_L*HE*BA\n\
dPS/dt 	= pi_S*BA - delta_S*PS\n\
dPL/dt 	= pi_L*HE*BA - delta_L*PL\n\
dA/dt 	= pi_AS*PS + pi_AL*PL - delta_A*A\n\
\n\
//macro\n\
pi_H_DM 	= pi_H1*DM/(DM+pi_H2)\n\
rho_HE_DM 	= rho_HE1*DM/(DM+rho_HE2)\n\
delta_HE_DM= delta_HE1*DM/(DM+delta_HE2)\n\
pi_T_DM 	= pi_T1*DM/(DM+pi_T2)\n\
rho_TE_DM 	= rho_TE1*DM/(DM+rho_TE2)\n\
delta_TE_DM	= delta_TE1*DM/(DM+delta_TE2)\n\
pi_B_DM 	= pi_B1*DM/(DM+pi_B2)\n\
rho_BA_DM_h_HE = rho_BA1*(DM+h*HE)/((DM+h*HE)+rho_BA2)\n\
t_E_lung = gamma * TE[t-tau_T]\n\
\n\
HE_HN = HE + HN\n\
TE_TN = TE + TN\n\
BA_BN = BA + BN\n\
\n\
// History functions\n\
TE[t]=0, -INF<t<t0\n\
Ds[t]=0, -INF<t<t0"

);

map<string, double> paramValues = {
    {"A", 110.2},        {"BA", 0.0},          {"BN", 1000.0},
    {"D", 1000.0},       {"DM", 0.0},          {"Ds", 0.0},
    {"EP", 500000.0},    {"EPs", 0.0},         {"HE", 0.0},
    {"HN", 1000.0},      {"PL", 0.0},          {"PS", 0.0},
    {"TE", 0.0},         {"TN", 1000.0},       {"V", 10.0},
    {"beta_D", 0.01},    {"beta_E", 0.000070}, {"c_V", 1.0},
    {"delta_A", 0.04},   {"delta_B", 0.002},   {"delta_BA", 0.9},
    {"delta_D", 0.001},  {"delta_DM", 0.5},    {"delta_Ds", 2.9},
    {"delta_E", 0.001},  {"delta_Es", 1.2},    {"delta_HE1", 0.4},
    {"delta_HE2", 1.0},  {"delta_HN", 0.002},  {"delta_L", 0.03},
    {"delta_S", 0.1},    {"delta_TE1", 0.75},  {"delta_TE2", 1.0},
    {"delta_TN", 0.002}, {"gamma", 0.15},      {"h", 0.0},
    {"k_D", 200.0},      {"k_E", 0.00119},     {"k_V", 0.004},
    {"pi_AL", 0.8},      {"pi_AS", 0.06},      {"pi_B1", 3.0},
    {"pi_B2", 10000.0},  {"pi_H1", 1.5},       {"pi_H2", 100.0},
    {"pi_L", 8.0E-9},    {"pi_S", 0.001},      {"pi_T1", 3.0},
    {"pi_T2", 100.0},    {"pi_V", 1.9},        {"rho_BA1", 2.6},
    {"rho_BA2", 4000.0}, {"rho_HE1", 1.51},    {"rho_HE2", 4000.0},
    {"rho_TE1", 2.6},    {"rho_TE2", 4000.0},  {"tau_D", 1.0},
    {"tau_T", 0.}

};

int main(int, char **) {
    cerr << model->containsDelays() << endl;

    SambineDDEWrapper solver;
    solver.setStartTime(0.0);
    solver.setStopTime(100.0);
    solver.setStepSize(0.1);

    TDoubleVector initCondValues(model->getDependentVariables().size());
    TDoubleVector modelParamValues(model->getModelParameters().size());

    unsigned cntr = 0;
    for (const auto &initCond: model-> getDependentVariables()) {
        initCondValues(cntr++) = paramValues.at(initCond -> getName());
    }

     cntr = 0;
    for (const auto &modelParam: model-> getModelParameters()) {
        modelParamValues(cntr++) = paramValues.at(modelParam -> getName());
    }

    TDoubleVector tout; 
    TDoubleMatrix yout;

    solver.solve(model, modelParamValues, initCondValues, tout, yout);

    cerr << "SambineDDE_ex: Simulation done\n";

}