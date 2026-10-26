#ifndef FRS42_ARENA_SCREEN_H
#define FRS42_ARENA_SCREEN_H

#include "Difficulty.h"
#include "Megovision.h"
#include "Playfield.h"
#include "RoundSet.h"
#include <Screen.h>
#include <string>
#include <system/ui/Label.h>

namespace nuvelocity::frs42
{
    class BrickLayout;
    class BrickInfo;
    class ArenaScreen : public Screen
    {
    public:
        ArenaScreen(const RoundEntry* entry,
                   bool resetStartTime = false,
                   Difficulty diff = Difficulty::Normal);
        ~ArenaScreen() override = default;

        void Load(Game* game) override;
        void Unload(Game* game) override;
        void Update(Game* game) override;
        void Draw(Game* game) override;
        void SetInitialStats(const GameStats& stats, int lives);
        void SuspendGame();
        void EndGame(Game* game, bool isGameOver);
        int GetScore() const
        {
            return mPlayfield.GetScore();
        }

        std::string GetName() const override
        {
            return "ArenaScreen";
        }

    private:
        const RoundEntry* mRoundEntry = nullptr;
        bool mEnding = false;

        RoundSet* mRoundSet = nullptr;
        Playfield mPlayfield;
        Megovision mMegovision;

        SDL_Rect mPlayfieldRect;
        StandAloneFrame* mGameOverFrame;
        std::string mCheatBuffer;
        bool mIsPaused = false;

        void PopulateBricks(Game* game, BrickLayout* layout);
        void BuildLevelUI(Game* game, BrickLayout* layout, const RoundEntry* entry);
        BrickInfo* GetOrLoadBrickInfo(Game* game, const std::string& path);
        void ShowPauseMenu(Game* game);
        void ShowOptionsDialog(Game* game);
        void MarkCheated();
    };
} // namespace nuvelocity::frs42

#endif // FRS42_ARENA_SCREEN_H
