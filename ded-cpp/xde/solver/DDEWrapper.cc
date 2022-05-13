#include "solver/DDEWrapper.h"

using namespace std;

DDEWrapper::DDEWrapper() {}

DDEWrapper::~DDEWrapper() {}

vector<string> DDEWrapper::s_getMethodAttributes() {
    vector<string> attrs = XDESolverWrapper::getAttributes();
    attrs.push_back("DDE");
    return attrs;
}



//vector<double> DDEWrapper::computeDerivatives(double time, vector<double> y0, TDoubleMatrix &delays, ModelPtr model) {
//    return vector<double>();

//}

// void DDEWrapper::computeHistory(double time, TDoubleVector,
//                                 TDoubleVector &yhist, void *user_data) {
//     XDEModel *model = (XDEModel *)user_data;
//     int size = yhist.size();

//     // FIX HERE
//     // for (int i = 0; i < size; i++) {
//     //     yhist[i] = model->getHistory(i, time);
//     // }
// }

// const std::vector<double> &DDEWrapper::computeHistory(double time,
//                                 ModelPtr model) {

//     return model -> getHistoryAtTime(time);

// }
