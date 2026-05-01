#include "GameStats.h"

namespace nuvelocity
{
    GameStats::GameStats() = default;
    GameStats::~GameStats() = default;

    void GameStats::InitClassInfo(ClassInfo& info)
    {
        info.mName = "CGameStats";
        AddProperty(info, "Starting Round Number", &GameStats::mStartingRoundNumber);
        AddProperty(info, "Ending Round Number", &GameStats::mEndingRoundNumber);
        AddProperty(info, "Bricks Destroyed", &GameStats::mBricksDestroyed);
        AddProperty(info, "Balls Lost", &GameStats::mBallsLost);
        AddProperty(info, "Points Scored", &GameStats::mPointsScored);
        AddProperty(info, "Seconds Played", &GameStats::mSecondsPlayed);
        AddProperty(info, "Level Of Difficulty", &GameStats::mLevelOfDifficulty);
        AddProperty(info, "Used Cheat", &GameStats::mUsedCheat);
        AddProperty(info, "Games Finished", &GameStats::mGamesFinished);

        // Time tracking was introduced in build 75
        AddProperty(info, "Start Time", &GameStats::mStartTime);
        AddProperty(info, "End Time", &GameStats::mEndTime);
    }
} // namespace nuvelocity
