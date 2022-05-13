#ifndef HISTORYINTERVAL_H
#define HISTORYINTERVAL_H

#include <parser/ProcessedModelExpression.h>



/**
 * \brief Handles and interval in the history functions.
 *
 * Since DEDiscover handle delay variables it also supports history
 * functions to define values at times prior the start of the
 * simulation. This object defines an interval for a variable, and
 * holds a pointer to a function. The interval can be open or close.
 */

class HistoryInterval {
   public:

    HistoryInterval(double _lowBound, double _highBound,
                    bool _minIncluded, bool _maxIncluded
                    )
         {
        lowBound = _lowBound;
        highBound = _highBound;
        minIncluded = _minIncluded;
        maxIncluded = _maxIncluded;
    }

    bool inRange(double t) const;
    //auto getEquationID() { return 0; }

   private:
    double lowBound;
    double highBound;
    bool minIncluded;
    bool maxIncluded;

    //unsigned equationID;
};

typedef std::shared_ptr<HistoryInterval> HistoryIntervalPtr;

#endif
