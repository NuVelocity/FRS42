#ifndef FRS42_SUSPENDED_GAME_STATS_H
#define FRS42_SUSPENDED_GAME_STATS_H

#include "GameStats.h"

namespace nuvelocity
{
    class SuspendedGameStats : public Object<SuspendedGameStats, GameStats>
    {
    public:
        SuspendedGameStats();
        ~SuspendedGameStats() override;

        static void InitClassInfo(ClassInfo& info);

        int mBallsLeft = 0;
    };
} // namespace nuvelocity

#endif // FRS42_SUSPENDED_GAME_STATS_H
