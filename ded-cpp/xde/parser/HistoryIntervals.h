#ifndef HISTORY_INTERVALS_H
#define HISTORY_INTERVALS_H

#include <expressions/ExpressionHandler.h>
#include <parser/HistoryInterval.h>



/**
 * Class to handle history intervals.
 * */

template <typename K, typename R>
class HistoryIntervals {
   public:
    HistoryIntervals(const std::vector<K> _keys,
                     std::multimap<K, std::pair<HistoryInterval, R>> intervals)
        : keys(_keys) {
        historyIntervals.resize(keys.size());
        retVector.resize(keys.size());

        for (unsigned i = 0; i < _keys.size(); i++) {
            auto entries = intervals.equal_range(keys[i]);
            for (auto &it = entries.first; it != entries.second; it++) {
                historyIntervals[i].push_back(it->second);
            }
        }
    }

    const std::vector<double> &getHistory(const double &t,
                                         std::function<double(R)> &f) {
        std::fill(retVector.begin(), retVector.end(), 0.0);
        for (unsigned i = 0; i < keys.size(); i++) {
            for (const auto &intObj : historyIntervals[i]) {
                if (intObj.first.inRange(t)) {
                    retVector[i] = f(intObj.second);
                    break;
                }
            }
        }
        return retVector;
    }

   private:
    std::vector<K> keys;
    std::vector<double> retVector;
    std::vector<std::vector<std::pair<HistoryInterval, R>>> historyIntervals;

};

   template<typename K, typename R> using HistoryIntervalsPtr =  std::shared_ptr<HistoryIntervals<K,R>> ;


#endif // HISTORY_INTERVALS_H