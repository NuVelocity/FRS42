#ifndef FRS42_MAIN_MENU_BUTTON_H
#define FRS42_MAIN_MENU_BUTTON_H

#include <SDL3/SDL.h>
#include <ui/Button.h>

#include <cstddef>
#include <cstdint>

namespace nuvelocity
{
    class Sequence;

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
        void SetRect(const SDL_Rect& rect) override;
        void SetStyle(const Style& style);
        SDL_Point GetSize() const;

        void ResetAnimation(uint64_t nowTick, uint64_t revealDelayMs = 0);
        void Update(Game* game) override;
        void Draw(Game* game) override;

        bool Intersects(const SDL_Point& point) const;

    private:
        MainMenuButtonAssets mAssets;
        Sequence* mArmSequence;
        Sequence* mPanelSequence;
        std::size_t mPanelFrameIndex;
        SDL_Rect mTargetBounds;
        SDL_Rect mCurrentBounds;
        bool mRevealStarted;
        bool mRevealComplete;
        bool mPanelFlipComplete;
        uint64_t mRevealStartTick;
        uint64_t mPanelFlipStartTick;
        Style mStyle;

        static constexpr uint64_t kSlideDurationMs = 700;

        void UpdateAnimation(Game* game);
        SDL_Rect GetRenderBounds() const;
        std::size_t GetPanelFlipFrameIndex(uint64_t nowTick) const;

        void DrawSequenceFrame(Game* game,
                               Sequence* sequence,
                               std::size_t frameIndex,
                               const SDL_Rect& destination);
    };
} // namespace nuvelocity

#endif // FRS42_MAIN_MENU_BUTTON_H
