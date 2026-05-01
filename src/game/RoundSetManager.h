#ifndef FRS42_ROUND_SET_MANAGER_H
#define FRS42_ROUND_SET_MANAGER_H

#include "ListEntries.h"
#include <string>
#include <system/Manager.h>
#include <vector>

namespace nuvelocity
{
    class Game;
}

namespace nuvelocity::frs42
{
    class RoundSet;
    class BrickLayout;

    class RoundSetManager : public nuvelocity::Manager
    {
    public:
        static RoundSetManager& Get();

        bool Initialize(char** argv) override;
        void SetGame(Game* game)
        {
            mGame = game;
        }
        void Populate();

        const std::vector<RoundEntry>& GetAllRounds() const
        {
            return mAllRounds;
        }

        const RoundEntry* GetRoundEntry(int globalIndex) const;

        RoundSet* GetRoundSet(int setIndex) const;

        BrickLayout* LoadRoundLayout(int globalIndex) const;

    private:
        RoundSetManager() = default;

        std::vector<RoundEntry> mAllRounds;
        std::vector<RoundSet*> mRoundSets;
        Game* mGame = nullptr;
    };
} // namespace nuvelocity::frs42

#endif // FRS42_ROUND_SET_MANAGER_H
