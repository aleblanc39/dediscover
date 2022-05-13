#include "EstimationSymbolAttributes.h"

EstimationSymbolAttributes::EstimationSymbolAttributes() {
}

EstimationSymbolAttributes::~EstimationSymbolAttributes() {
}



std::ostream &
operator <<(std::ostream &out, const EstimationSymbolAttributes &o) {
    out << o.getName() << "\t" << o.getInitialValue() << "\t" << o.getMinValue() << "\t" << o.getMaxValue() << "\t" << o.isEstimated() <<"\n";
    return out;
}
