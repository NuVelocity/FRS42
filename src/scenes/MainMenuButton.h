#ifndef FRS42_MAIN_MENU_BUTTON_H
#define FRS42_MAIN_MENU_BUTTON_H

#include <Game.h>
#include <SDL3/SDL.h>
#include <Sequence.h>
#include <ui/Button.h>

#include <cstddef>
#include <cstdint>

namespace nuvelocity
{
    struct MainMenuButtonAssets
    {
        Sequence* armNormal = nullptr;
        Sequence* armHover = nullptr;
        Sequence* panelFlip = nullptr;
        Sequence* panelNormal = nullptr;
        Sequence* panelHover = nullptr;
        Sequence* panelPressed = nullptr;
    };

    class MainMenuButton : public Button
    {
    public:
        struct Style
        {
            SDL_Color textColor = SDL_Color{.r = 255, .g = 255, .b = 255, .a = 255};
            int fontPointSize = 12;
        };

        MainMenuButton();

        void SetAssets(const MainMenuButtonAssets& assets);
        void SetRect(const SDL_FRect& rect) override;
        void SetStyle(const Style& style);
        SDL_FPoint GetSize() const;

        void ResetAnimation(uint64_t nowTick, uint64_t revealDelayMs = 0);
        void Update(Game* aGame) override;
        void Draw(Game* aGame) override;

        bool Intersects(const SDL_FPoint& point) const;

    private:
        MainMenuButtonAssets mAssets;
        SDL_FRect mTargetBounds;
        SDL_FRect mCurrentBounds;
        bool mRevealStarted;
        bool mRevealComplete;
        bool mPanelFlipComplete;
        uint64_t mRevealStartTick;
        uint64_t mPanelFlipStartTick;
        Style mStyle;

        static constexpr uint64_t kSlideDurationMs = 700;

        void UpdateAnimation(int windowWidth);
        SDL_FRect GetRenderBounds() const;
        std::size_t GetPanelFlipFrameIndex(uint64_t nowTick) const;

        static void DrawSequenceFrame(Game* game,
                                      Sequence* sequence,
                                      std::size_t frameIndex,
                                      const SDL_FRect& destination);
    };
} // namespace nuvelocity

#endif // FRS42_MAIN_MENU_BUTTON_H
