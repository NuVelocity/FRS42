#ifndef FRS42_GAME_STATS_H
#define FRS42_GAME_STATS_H

#include "model/Object.h"

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
        int mLevelOfDifficulty = 0;
        bool mUsedCheat = false;
        int mGamesFinished = 0;
    };
} // namespace nuvelocity

#endif // FRS42_GAME_STATS_H
