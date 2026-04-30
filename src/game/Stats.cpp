#include "Stats.h"
#include "GameStats.h"
#include "PlayerStats.h"

namespace nuvelocity
{
    Stats::Stats() = default;
    Stats::~Stats()
    {
        for (auto* player : mPlayers)
        {
            delete player;
        }
        delete mAllPlayersTotals;
        delete mAllPlayersBest;
        delete mCurrentGame;
    }

    void Stats::InitClassInfo(ClassInfo& info)
    {
        info.mName = "CStats";
        AddProperty(info, "Players", &Stats::mPlayers, "Player");
        AddProperty(info, "Current Player", &Stats::mCurrentPlayer);
        AddProperty(info, "Demo Time Extensions", &Stats::mDemoTimeExtensions);
        AddProperty(info,
                    "Program Launches Since Version Check",
                    &Stats::mProgramLaunchesSinceVersionCheck);
        AddProperty(info, "All Players Totals", &Stats::mAllPlayersTotals);
        AddProperty(info, "All Players Best", &Stats::mAllPlayersBest);
        AddProperty(info, "Current Game", &Stats::mCurrentGame);
    }
} // namespace nuvelocity
