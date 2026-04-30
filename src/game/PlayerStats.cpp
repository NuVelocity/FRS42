#include "PlayerStats.h"
#include "GameStats.h"
#include "SuspendedGameStats.h"

namespace nuvelocity
{
    PlayerStats::PlayerStats() = default;
    PlayerStats::~PlayerStats()
    {
        delete mSuspendedGame;
        delete mCareerTotals;
        delete mCareerBest;
        for (auto* game : mAllGamesPlayed)
        {
            delete game;
        }
    }

    void PlayerStats::InitClassInfo(ClassInfo& info)
    {
        info.mName = "CPlayerStats";
        AddProperty(info, "Name", &PlayerStats::mName);
        AddProperty(info, "Ship Style", &PlayerStats::mShipStyle);
        AddProperty(info, "Last Selected Difficulty", &PlayerStats::mLastSelectedDifficulty);
        AddProperty(info, "Last Selected Round Set", &PlayerStats::mLastSelectedRoundSet);
        AddProperty(info, "Last Selected Check Point", &PlayerStats::mLastSelectedCheckPoint);
        AddProperty(info, "Suspended Game", &PlayerStats::mSuspendedGame);
        AddProperty(info, "Career Totals", &PlayerStats::mCareerTotals);
        AddProperty(info, "Career Best", &PlayerStats::mCareerBest);
        AddProperty(info, "All Games Played", &PlayerStats::mAllGamesPlayed, "Game Stats");
        AddProperty(
            info, "Check Points Reached", &PlayerStats::mCheckPointsReached, "Num Check Points");
    }
} // namespace nuvelocity
