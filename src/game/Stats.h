#ifndef FRS42_STATS_H
#define FRS42_STATS_H

#include "model/Object.h"
#include <vector>

namespace nuvelocity
{
    class PlayerStats;
    class GameStats;

    class Stats : public Object<Stats>
    {
    public:
        Stats();
        ~Stats();

        static void InitClassInfo(ClassInfo& info);

        std::vector<PlayerStats*> mPlayers;
        int mCurrentPlayer = 0;
        int mDemoTimeExtensions = 0;
        int mProgramLaunchesSinceVersionCheck = 0;
        GameStats* mAllPlayersTotals = nullptr;
        GameStats* mAllPlayersBest = nullptr;
        GameStats* mCurrentGame = nullptr;
    };
} // namespace nuvelocity

#endif // FRS42_STATS_H
