#include "StatsManager.h"
#include "RoundSetManager.h"
#include "decoders/DecodeUtils.h"
#include "model/PropertySerializer.h"
#include "system/LogCategory.h"
#include <SDL3/SDL.h>
#include <algorithm>
#include <sstream>
#include <system/AssetManager.h>
#include <vector>

namespace nuvelocity::frs42
{
    const std::string StatsManager::kGuestName = "Anonymous Guest";

    StatsManager& StatsManager::Get()
    {
        static StatsManager instance;
        return instance;
    }

    StatsManager::StatsManager()
    {
        mStats = new Stats();
        CreateGuestPlayer();
    }

    StatsManager::~StatsManager()
    {
        delete mStats;
    }

    bool StatsManager::Initialize(char** argv)
    {
        (void)argv;
        if (WarnIfAlreadyInitialized("StatsManager"))
        {
            return true;
        }

        Load();

        mInitialized = true;
        return true;
    }

    void StatsManager::Load()
    {
        if (LoadImpl())
        {
            return;
        }
        CreateGuestPlayer();
    }

    bool StatsManager::LoadImpl()
    {
        if (mAsset == nullptr)
        {
            return false;
        }

        // FIXME: Stats.dat should be saved in the same directory as the executable,
        // not in the asset directory.
        const char* statsPath = "Stats.dat";
        if (!mAsset->Exists(statsPath))
        {
            return false;
        }

        SDL_IOStream* stream = mAsset->Load(statsPath);
        if (stream == nullptr)
        {
            SDL_LogWarn(NVE_LOG_CATEGORY_PROPSYS, "Failed to open Stats.dat for reading");
            return false;
        }

        uint8_t version = 0;
        if (SDL_ReadIO(stream, &version, 1) != 1 || version != 1)
        {
            SDL_LogWarn(NVE_LOG_CATEGORY_PROPSYS, "Stats.dat version mismatch or read failure");
            SDL_CloseIO(stream);
            return false;
        }

        uint32_t defSize = 0;
        uint32_t infSize = 0;
        if (SDL_ReadIO(stream, &defSize, 4) != 4 || SDL_ReadIO(stream, &infSize, 4) != 4)
        {
            SDL_LogWarn(NVE_LOG_CATEGORY_PROPSYS, "Failed to read Stats.dat header sizes");
            SDL_CloseIO(stream);
            return false;
        }

        std::vector<uint8_t> compressedData(defSize);
        if (SDL_ReadIO(stream, compressedData.data(), defSize) != defSize)
        {
            SDL_LogWarn(NVE_LOG_CATEGORY_PROPSYS, "Failed to read Stats.dat compressed data");
            SDL_CloseIO(stream);
            return false;
        }
        SDL_CloseIO(stream);

        std::vector<uint8_t> inflatedData(infSize);
        uint32_t destLen = infSize;
        uint32_t sourceLen = defSize;

        if (DecodeUtils::Inflate(
                inflatedData.data(), &destLen, compressedData.data(), &sourceLen) != Z_OK)
        {
            SDL_LogWarn(NVE_LOG_CATEGORY_PROPSYS, "Failed to inflate Stats.dat");
            return false;
        }

        std::string content(reinterpret_cast<char*>(inflatedData.data()), destLen);
        Stats* loadedStats = nullptr;
        if (!PropertySerializer::Deserialize(content, loadedStats))
        {
            SDL_LogWarn(NVE_LOG_CATEGORY_PROPSYS, "Failed to deserialize Stats.dat");
            return false;
        }

        delete mStats;
        mStats = loadedStats;
        return true;
    }

    void StatsManager::Save()
    {
        if (mAsset == nullptr)
        {
            return;
        }

        std::string output;
        PropertySerializer::Serialize(*mStats, output);

        uint32_t infSize = static_cast<uint32_t>(output.size());
        uint32_t defSize = compressBound(infSize);
        std::vector<uint8_t> compressedData(defSize);

        if (DecodeUtils::Deflate(compressedData.data(),
                                 &defSize,
                                 reinterpret_cast<const uint8_t*>(output.data()),
                                 infSize) != Z_OK)
        {
            SDL_LogWarn(NVE_LOG_CATEGORY_PROPSYS, "Failed to deflate Stats.dat");
            return;
        }

#if DEBUG
        SDL_IOStream* debugStream = mAsset->OpenWrite("stats.txt");
        if (debugStream != nullptr)
        {
            SDL_WriteIO(debugStream, output.data(), output.size());
            SDL_CloseIO(debugStream);
        }
#endif

        const char* statsPath = "Stats.dat";
        SDL_IOStream* stream = mAsset->OpenWrite(statsPath);
        if (stream == nullptr)
        {
            SDL_LogError(NVE_LOG_CATEGORY_PROPSYS, "Failed to open Stats.dat for writing");
            return;
        }

        uint8_t version = 1;
        SDL_WriteIO(stream, &version, 1);
        SDL_WriteIO(stream, &defSize, 4);
        SDL_WriteIO(stream, &infSize, 4);
        SDL_WriteIO(stream, compressedData.data(), defSize);
        SDL_CloseIO(stream);
    }

    PlayerStats* StatsManager::GetCurrentPlayer()
    {
        if (mStats->mCurrentPlayer >= 0 &&
            static_cast<size_t>(mStats->mCurrentPlayer) < mStats->mPlayers.size())
        {
            return mStats->mPlayers[mStats->mCurrentPlayer];
        }
        return nullptr;
    }

    void StatsManager::SetCurrentPlayer(int index)
    {
        if (index >= 0 && static_cast<size_t>(index) < mStats->mPlayers.size())
        {
            mStats->mCurrentPlayer = index;
        }
    }

    void StatsManager::CreateGuestPlayer()
    {
        for (auto* player : mStats->mPlayers)
        {
            if (player->mName == kGuestName)
            {
                return;
            }
        }

        auto* guest = new PlayerStats();
        guest->mName = kGuestName;
        mStats->mPlayers.push_back(guest);
    }

    void StatsManager::UpdateCareerStats(GameStats* sessionStats, bool isGameOver)
    {
        PlayerStats* current = GetCurrentPlayer();
        if (current == nullptr || sessionStats == nullptr)
        {
            return;
        }

        if (current->mCareerTotals == nullptr)
        {
            current->mCareerTotals = new GameStats();
        }

        current->mCareerTotals->mSecondsPlayed += sessionStats->mSecondsPlayed;
        current->mCareerTotals->mPointsScored += sessionStats->mPointsScored;
        current->mCareerTotals->mBricksDestroyed += sessionStats->mBricksDestroyed;
        current->mCareerTotals->mBallsLost += sessionStats->mBallsLost;
        if (isGameOver)
        {
            current->mCareerTotals->mGamesFinished++;
        }

        if (current->mCareerBest == nullptr ||
            sessionStats->mPointsScored > current->mCareerBest->mPointsScored)
        {
            if (current->mCareerBest == nullptr)
            {
                current->mCareerBest = new GameStats();
            }
            *current->mCareerBest = *sessionStats;
        }

        // Record this game in player history for high score listing
        auto* recordedGame = new GameStats();
        *recordedGame = *sessionStats;
        current->mAllGamesPlayed.push_back(recordedGame);

        Save();
    }

    void StatsManager::UpdateCheckpoint(int globalRoundIndex)
    {
        PlayerStats* current = GetCurrentPlayer();
        if (current == nullptr)
            return;

        const auto& rounds = RoundSetManager::Get().GetAllRounds();
        if (globalRoundIndex < 0 || static_cast<size_t>(globalRoundIndex) >= rounds.size())
            return;

        const auto& entry = rounds[globalRoundIndex];
        int rsIdx = entry.setIndex;
        int roundIndex = entry.roundIndex;

        if (static_cast<size_t>(rsIdx) >= current->mCheckPointsReached.size())
        {
            current->mCheckPointsReached.resize(rsIdx + 1, 0);
        }

        if (roundIndex > current->mCheckPointsReached[rsIdx])
        {
            current->mCheckPointsReached[rsIdx] = roundIndex;
        }

        Save();
    }

    std::vector<PlayerEntry> StatsManager::GetPlayerSummaries()
    {
        std::vector<PlayerEntry> summaries;
        if (mStats == nullptr)
        {
            return summaries;
        }

        for (auto* player : mStats->mPlayers)
        {
            PlayerEntry summary;
            summary.name = player->mName;
            summary.gamesPlayed = static_cast<int>(player->mAllGamesPlayed.size());
            summary.totalTime =
                (player->mCareerTotals != nullptr) ? player->mCareerTotals->mSecondsPlayed : 0.0;
            summary.highestRoundIndex =
                (player->mCareerTotals != nullptr) ? player->mCareerTotals->mEndingRoundNumber : 0;
            summary.bestScore =
                (player->mCareerBest != nullptr) ? player->mCareerBest->mPointsScored : 0;
            summaries.push_back(summary);
        }

        return summaries;
    }

    std::vector<HighScoreEntry> StatsManager::GetHighScores(int difficulty)
    {
        std::vector<HighScoreEntry> records;
        if (mStats == nullptr)
        {
            return records;
        }

        for (auto* player : mStats->mPlayers)
        {
            for (auto* game : player->mAllGamesPlayed)
            {
                if (difficulty >= 0 && game->mLevelOfDifficulty != difficulty)
                {
                    continue;
                }

                HighScoreEntry record;
                record.playerName = player->mName;
                record.score = game->mPointsScored;
                record.difficulty = game->mLevelOfDifficulty;
                record.time = game->mSecondsPlayed;
                record.startRound = game->mStartingRoundNumber;
                record.endRound = game->mEndingRoundNumber;
                records.push_back(record);
            }
        }

        // Sort by score descending
        std::sort(records.begin(),
                  records.end(),
                  [](const HighScoreEntry& a, const HighScoreEntry& b)
                  { return a.score > b.score; });

        return records;
    }

    std::string StatsManager::FormatRoundIndex(int globalIndex)
    {
        const auto* entry = RoundSetManager::Get().GetRoundEntry(globalIndex);
        if (entry == nullptr)
        {
            return "0-0";
        }

        std::stringstream ss;
        ss << (entry->setIndex + 1) << "-" << (entry->roundIndex + 1);
        return ss.str();
    }

    bool StatsManager::RemovePlayer(int index)
    {
        if (mStats == nullptr || index < 0 || index >= static_cast<int>(mStats->mPlayers.size()))
        {
            return false;
        }

        PlayerStats* player = mStats->mPlayers[index];
        if (player == nullptr || player->mName == kGuestName)
        {
            return false;
        }

        delete player;
        mStats->mPlayers.erase(mStats->mPlayers.begin() + index);

        if (!mStats->mPlayers.empty())
        {
            if (mStats->mCurrentPlayer > index)
            {
                mStats->mCurrentPlayer--;
            }
            else if (mStats->mCurrentPlayer == index)
            {
                mStats->mCurrentPlayer =
                    std::min(mStats->mCurrentPlayer, static_cast<int>(mStats->mPlayers.size()) - 1);
            }
        }
        else
        {
            mStats->mCurrentPlayer = 0;
        }

        Save();
        return true;
    }
} // namespace nuvelocity::frs42
