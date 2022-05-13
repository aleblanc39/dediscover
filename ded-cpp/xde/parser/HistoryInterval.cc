#include "HistoryInterval.h"
using namespace std;


bool HistoryInterval::inRange(double t) const{
    if (t < lowBound || t > highBound) return false;
    if (t > lowBound && t < highBound) return true;
    if ((minIncluded && t == lowBound) || (maxIncluded && t == highBound))
        return true;
    return false;
}
