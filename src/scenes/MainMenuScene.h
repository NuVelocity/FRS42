#ifndef FRS42_MAIN_MENU_SCENE_H
#define FRS42_MAIN_MENU_SCENE_H

#include <Image.h>
#include <Scene.h>
#include <ui/Button.h>
#include <ui/FocusContainer.h>

#include <array>
#include <memory>

#include "MainMenuButton.h"
#include "Playfield.h"

namespace nuvelocity
{
    class StandAloneFrame;
}

namespace nuvelocity::frs42
{
    class PlayfieldBarrier;

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
        bool mHideMenuButtons = false;

        Playfield mPlayfield;
        std::vector<PlayfieldBarrier*> mBarriers;

        void OnPlayClick(Game* game);
        void OnStatsClick(Game* game);
        void OnFriendClick(Game* game);
        void OnOptionsClick(Game* game);
        void OnExitClick(Game* game);

    public:
        MainMenuScene() = default;
        void Load(Game* game) override;
        void Update(Game* game) override;
        void UpdatePlayfield(Game* game);
        void Draw(Game* game) override;
        void Unload(Game* game) override {};
        std::string GetName() const override;
    };
} // namespace nuvelocity::frs42

#endif // FRS42_MAIN_MENU_SCENE_H
