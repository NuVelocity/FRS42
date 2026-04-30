#ifndef FRS42_PLAYER_STATS_H
#define FRS42_PLAYER_STATS_H

#include "model/Object.h"
#include <string>
#include <vector>

namespace nuvelocity
{
    class GameStats;
    class SuspendedGameStats;

    class PlayerStats : public Object<PlayerStats>
    {
    public:
        PlayerStats();
        ~PlayerStats();

        static void InitClassInfo(ClassInfo& info);

        std::string mName;
        int mShipStyle = 1;
        int mLastSelectedDifficulty = 1;
        int mLastSelectedRoundSet = 0;
        int mLastSelectedCheckPoint = 0;
        SuspendedGameStats* mSuspendedGame = nullptr;
        GameStats* mCareerTotals = nullptr;
        GameStats* mCareerBest = nullptr;
        std::vector<GameStats*> mAllGamesPlayed;
        std::vector<int> mCheckPointsReached;
    };
} // namespace nuvelocity

#endif // FRS42_PLAYER_STATS_H
