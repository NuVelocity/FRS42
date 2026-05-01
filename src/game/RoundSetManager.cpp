#include "RoundSetManager.h"
#include "BrickLayout.h"
#include "RoundSet.h"
#include <Game.h>
#include <algorithm>
#include <system/AssetManager.h>

namespace nuvelocity::frs42
{
    RoundSetManager& RoundSetManager::Get()
    {
        static RoundSetManager instance;
        return instance;
    }

    bool RoundSetManager::Initialize(char** argv)
    {
        (void)argv;
        if (WarnIfAlreadyInitialized("RoundSetManager"))
            return true;

        Populate();

        mInitialized = true;
        return true;
    }

    void RoundSetManager::Populate()
    {
        if (mGame == nullptr)
            return;

        mAllRounds.clear();
        mRoundSets.clear();

        auto roundSets = mGame->mAsset->EnumerateRoundSets();
        std::ranges::sort(roundSets,
                          [](const auto& a, const auto& b) { return a.second < b.second; });

        int setIndex = 0;
        int globalIndex = 0;
        for (const auto& [fullPath, rsName] : roundSets)
        {
            RoundSet* rs = static_cast<RoundSet*>(mGame->mAsset->LoadPropertyFile(fullPath));
            if (rs == nullptr)
                continue;

            mRoundSets.push_back(rs);

            const auto& rounds = rs->GetRoundList();
            for (size_t i = 0; i < rounds.size(); ++i)
            {
                RoundEntry entry;
                entry.setPath = fullPath;
                entry.setName = rsName;
                entry.name = rounds[i];
                entry.roundIndex = static_cast<int>(i);
                entry.setIndex = setIndex;
                entry.globalIndex = globalIndex++;
                mAllRounds.push_back(entry);
            }
            setIndex++;
        }
    }

    const RoundEntry* RoundSetManager::GetRoundEntry(int globalIndex) const
    {
        if (globalIndex >= 0 && static_cast<size_t>(globalIndex) < mAllRounds.size())
        {
            return &mAllRounds[globalIndex];
        }
        return nullptr;
    }

    RoundSet* RoundSetManager::GetRoundSet(int setIndex) const
    {
        if (setIndex >= 0 && static_cast<size_t>(setIndex) < mRoundSets.size())
        {
            return mRoundSets[setIndex];
        }
        return nullptr;
    }

    BrickLayout* RoundSetManager::LoadRoundLayout(int globalIndex) const
    {
        if (mGame == nullptr)
            return nullptr;

        const RoundEntry* entry = GetRoundEntry(globalIndex);
        if (entry == nullptr)
            return nullptr;

        std::string roundPath = "Resources/" + entry->name + ".Ricochet";
        return static_cast<BrickLayout*>(mGame->mAsset->LoadPropertyFile(roundPath));
    }
} // namespace nuvelocity::frs42
