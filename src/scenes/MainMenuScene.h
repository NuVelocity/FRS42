#ifndef FRS42_MAIN_MENU_SCENE_H
#define FRS42_MAIN_MENU_SCENE_H

#include <Image.h>
#include <Scene.h>
#include <ui/Button.h>
#include <ui/FocusContainer.h>

#include <array>
#include <memory>

#include "BarrierBrick.h"
#include "GameBoard.h"
#include "MainMenuButton.h"
#include "StandAloneFrame.h"

namespace nuvelocity::frs42
{
    class MainMenuScene : public Scene
    {
    private:
        static constexpr uint64_t kEntryFadeDurationMs = 600;
        static constexpr std::size_t kMenuButtonCount = 5;

        StandAloneFrame* mBackgroundImage = nullptr;
        MainMenuButtonAssets mMenuAssets;
        MainMenuButton mPlayButton;
        MainMenuButton mStatsButton;
        MainMenuButton mFriendButton;
        MainMenuButton mOptionsButton;
        MainMenuButton mExitButton;
        std::array<nuvelocity::Button*, kMenuButtonCount> mMenuButtonPointers{};
        std::unique_ptr<nuvelocity::FocusContainer> mFocusContainer;
        uint64_t mEntryFadeStartTick = 0;

        GameBoard mGameBoard;
        std::vector<BarrierBrick*> mBarriers;

        void OnPlayClick();
        void OnStatsClick();
        void OnFriendClick();
        void OnOptionsClick();
        void OnExitClick();

    public:
        MainMenuScene() = default;
        void Load(Game* aGame) override;
        void Update(Game* aGame) override;
        void UpdateGameBoard(Game* aGame);
        void Draw(Game* aGame) override;
        void Unload(Game* aGame) override {};
        std::string GetName() const override;
    };
} // namespace nuvelocity::frs42

#endif // FRS42_MAIN_MENU_SCENE_H
