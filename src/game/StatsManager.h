#ifndef FRS42_STATS_MANAGER_H
#define FRS42_STATS_MANAGER_H

#include "GameStats.h"
#include "ListEntries.h"
#include "PlayerStats.h"
#include "Stats.h"
#include <string>
#include <system/Manager.h>

namespace nuvelocity
{
    class AssetManager;
}

namespace nuvelocity::frs42
{
    using nuvelocity::AssetManager;

    class StatsManager : public nuvelocity::Manager
    {
    public:
        static StatsManager& Get();

        bool Initialize(char** argv) override;

        void SetAssetManager(AssetManager* asset)
        {
            mAsset = asset;
        }

        void Load();

        void Save();

        Stats* GetStats()
        {
            return mStats;
        }

        PlayerStats* GetCurrentPlayer();

        void SetCurrentPlayer(int index);

        void UpdateCareerStats(GameStats* sessionStats, bool isGameOver);

        void UpdateCheckpoint(int globalRoundIndex);

        std::vector<PlayerEntry> GetPlayerSummaries();

        std::vector<HighScoreEntry> GetHighScores(int difficulty = -1);

        std::string FormatRoundIndex(int globalIndex);

        bool RemovePlayer(int index);

        static constexpr int kMaxPlayers = 13;

        static const std::string kGuestName;

    private:
        StatsManager();
        ~StatsManager();

        Stats* mStats = nullptr;
        AssetManager* mAsset = nullptr;

        bool LoadImpl();
        void CreateGuestPlayer();
    };
} // namespace nuvelocity::frs42

#endif // FRS42_STATS_MANAGER_H
