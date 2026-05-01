#ifndef FRS42_GAME_STATS_H
#define FRS42_GAME_STATS_H

#include "Difficulty.h"
#include "model/Object.h"
#include <cstdint>

namespace nuvelocity
{
    class GameStats : public Object<GameStats>
    {
    public:
        GameStats();
        virtual ~GameStats();

        static void InitClassInfo(ClassInfo& info);

        int mStartingRoundNumber = 0;
        int mEndingRoundNumber = 0;
        int mBricksDestroyed = 0;
        int mBallsLost = 0;
        int mPointsScored = 0;
        double mSecondsPlayed = 0.0;
        int mLevelOfDifficulty = frs42::DifficultyToInt(frs42::Difficulty::Easy);
        bool mUsedCheat = false;
        int mGamesFinished = 0;

        // Time tracking was introduced in build 75
        uint64_t mStartTime = 0;
        uint64_t mEndTime = 0;
    };
} // namespace nuvelocity

#endif // FRS42_GAME_STATS_H
