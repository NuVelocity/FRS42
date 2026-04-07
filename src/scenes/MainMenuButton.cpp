#include "MainMenuButton.h"

#include <Game.h>
#include <system/FontManager.h>
#include <system/InputManager.h>

namespace nuvelocity
{
    constexpr const char* kMainMenuButtonFontName = "OCR";

    static SDL_FPoint GetSequenceFrameSurfaceSize(Sequence* sequence, std::size_t frameIndex)
    {
        if (sequence == nullptr)
        {
            return SDL_FPoint{.x = 0.0F, .y = 0.0F};
        }

        Frame* frame = sequence->GetFrame(frameIndex);
        if (frame == nullptr)
        {
            return SDL_FPoint{.x = 0.0F, .y = 0.0F};
        }

        SDL_Surface* surface = frame->GetSurface();
        if (surface == nullptr)
        {
            return SDL_FPoint{.x = 0.0F, .y = 0.0F};
        }

        return SDL_FPoint{.x = static_cast<float>(surface->w), .y = static_cast<float>(surface->h)};
    }

    MainMenuButton::MainMenuButton()
            : mTargetBounds{.x = 0.0F, .y = 0.0F, .w = 0.0F, .h = 0.0F}
            , mCurrentBounds{.x = 0.0F, .y = 0.0F, .w = 0.0F, .h = 0.0F}
            , mRevealStarted(false)
            , mRevealComplete(false)
            , mPanelFlipComplete(false)
            , mRevealStartTick(0)
            , mPanelFlipStartTick(0)
    {
        Button::Style baseStyle = GetButtonStyle();
        baseStyle.showFocusRing = false;
        Button::SetStyle(baseStyle);
    }

    void MainMenuButton::SetAssets(const MainMenuButtonAssets& assets)
    {
        mAssets = assets;

        const SDL_FPoint size = GetSize();
        if (size.x > 0.0F && size.y > 0.0F)
        {
            mTargetBounds.w = size.x;
            mTargetBounds.h = size.y;
            mCurrentBounds.w = size.x;
            mCurrentBounds.h = size.y;
            SetRect(mCurrentBounds);
        }
    }

    void MainMenuButton::SetBounds(const SDL_FRect& bounds)
    {
        const SDL_FPoint size = GetSize();

        mTargetBounds = bounds;
        if (size.x > 0.0F && size.y > 0.0F)
        {
            mTargetBounds.w = size.x;
            mTargetBounds.h = size.y;
        }

        mCurrentBounds = bounds;
        if (size.x > 0.0F && size.y > 0.0F)
        {
            mCurrentBounds.w = size.x;
            mCurrentBounds.h = size.y;
        }

        SetRect(mCurrentBounds);
    }

    void MainMenuButton::SetStyle(const Style& style)
    {
        mStyle = style;
    }

    SDL_FPoint MainMenuButton::GetSize() const
    {
        SDL_FPoint size = GetSequenceFrameSurfaceSize(mAssets.armNormal, 0);
        if (size.x <= 0.0F || size.y <= 0.0F)
        {
            size = GetSequenceFrameSurfaceSize(mAssets.armHover, 0);
        }
        return size;
    }

    void MainMenuButton::ResetAnimation(uint64_t nowTick, uint64_t revealDelayMs)
    {
        mRevealStarted = true;
        mRevealComplete = false;
        mPanelFlipComplete = false;
        mRevealStartTick = nowTick + revealDelayMs;
        mPanelFlipStartTick = 0;
        mPressed = false;
        mHovered = false;
        mCurrentBounds = mTargetBounds;
        SetRect(mCurrentBounds);
    }

    void MainMenuButton::Update(InputManager& input, int windowWidth, uint64_t nowTick)
    {
        if (!IsVisible() || !IsEnabled())
        {
            mHovered = false;
            mPressed = false;
            return;
        }

        UpdateAnimation(windowWidth, nowTick);

        if (!mRevealComplete)
        {
            mHovered = false;
            mPressed = false;
            return;
        }

        SetRect(mCurrentBounds);
        Button::Update(input, SDL_FPoint{.x = 0.0F, .y = 0.0F});
    }

    void MainMenuButton::Draw(Game* game) const
    {
        if (!IsVisible() || game == nullptr || game->mSpriteBatch == nullptr ||
            game->mFont == nullptr)
        {
            return;
        }

        const SDL_FRect renderBounds = GetRenderBounds();

        Sequence* armSequence = mAssets.armNormal;
        if (mRevealComplete && (mHovered || mPressed || IsFocused()))
        {
            armSequence = mAssets.armHover;
        }

        Sequence* panelSequence = mAssets.panelFlip;
        std::size_t panelFrameIndex = 0;
        if (mRevealComplete && mPanelFlipComplete)
        {
            if (mPressed)
            {
                panelSequence = mAssets.panelPressed;
            }
            else if (mHovered || IsFocused())
            {
                panelSequence = mAssets.panelHover;
            }
            else
            {
                panelSequence = mAssets.panelNormal;
            }
        }
        else if (mRevealComplete)
        {
            panelFrameIndex = GetPanelFlipFrameIndex(SDL_GetTicks());
        }

        const SDL_FPoint panelSize = GetSequenceFrameSurfaceSize(panelSequence, panelFrameIndex);
        const SDL_FRect panelBounds{
            .x = renderBounds.x + 18, .y = renderBounds.y + 12, .w = panelSize.x, .h = panelSize.y};

        DrawSequenceFrame(game, armSequence, 0, renderBounds);
        DrawSequenceFrame(game, panelSequence, panelFrameIndex, panelBounds);

        if (mPanelFlipComplete)
        {
            game->mFont->DrawStringWithFontAt(kMainMenuButtonFontName,
                                              game->mSpriteBatch,
                                              GetDisplayCaption(),
                                              panelBounds.x + (panelBounds.w / 2),
                                              panelBounds.y + 6.0F,
                                              mStyle.textColor,
                                              13,
                                              TextAlignment::Center,
                                              nullptr,
                                              GetMnemonicIndex());
        }

        if (GetButtonStyle().showFocusRing && IsFocused() && !mHovered)
        {
            const SDL_FRect focusRect{.x = panelBounds.x + 2.0F,
                                      .y = panelBounds.y + 2.0F,
                                      .w = SDL_max(0.0F, panelBounds.w - 4.0F),
                                      .h = SDL_max(0.0F, panelBounds.h - 4.0F)};
            SDL_Color focusColor{255, 255, 255, 64};
            game->mSpriteBatch->DrawLine(
                focusRect.x, focusRect.y, focusRect.x + focusRect.w, focusRect.y, focusColor);
            game->mSpriteBatch->DrawLine(focusRect.x + focusRect.w,
                                         focusRect.y,
                                         focusRect.x + focusRect.w,
                                         focusRect.y + focusRect.h,
                                         focusColor);
            game->mSpriteBatch->DrawLine(focusRect.x + focusRect.w,
                                         focusRect.y + focusRect.h,
                                         focusRect.x,
                                         focusRect.y + focusRect.h,
                                         focusColor);
            game->mSpriteBatch->DrawLine(
                focusRect.x, focusRect.y + focusRect.h, focusRect.x, focusRect.y, focusColor);
        }
    }

    bool MainMenuButton::Intersects(const SDL_FPoint& point) const
    {
        return point.x >= mCurrentBounds.x && point.y >= mCurrentBounds.y &&
               point.x <= mCurrentBounds.x + mCurrentBounds.w &&
               point.y <= mCurrentBounds.y + mCurrentBounds.h;
    }

    void MainMenuButton::UpdateAnimation(int windowWidth, uint64_t nowTick)
    {
        if (!mRevealStarted)
        {
            ResetAnimation(nowTick);
        }

        const float startX = static_cast<float>(windowWidth) + SDL_max(48.0F, mTargetBounds.w);
        if (nowTick <= mRevealStartTick)
        {
            mCurrentBounds = mTargetBounds;
            mCurrentBounds.x = startX;
            return;
        }

        const uint64_t elapsed = nowTick - mRevealStartTick;
        const float progress =
            SDL_min(1.0F, static_cast<float>(elapsed) / static_cast<float>(kSlideDurationMs));

        mCurrentBounds = mTargetBounds;
        mCurrentBounds.x = startX + ((mTargetBounds.x - startX) * progress);

        if (!mRevealComplete && elapsed >= kSlideDurationMs)
        {
            mRevealComplete = true;
            mPanelFlipStartTick = nowTick;
        }

        if (mRevealComplete && !mPanelFlipComplete)
        {
            const float fps =
                mAssets.panelFlip != nullptr ? mAssets.panelFlip->GetFramesPerSecond() : 0.0F;
            const std::size_t frameCount =
                mAssets.panelFlip != nullptr ? mAssets.panelFlip->GetFrameCount() : 0;
            uint64_t durationMs = 0;
            if (fps > 0.0F && frameCount > 0)
            {
                durationMs = static_cast<uint64_t>((static_cast<double>(frameCount) * 1000.0) /
                                                   static_cast<double>(fps));
            }

            if (durationMs == 0 || nowTick - mPanelFlipStartTick >= durationMs)
            {
                mPanelFlipComplete = true;
            }
        }
    }

    SDL_FRect MainMenuButton::GetRenderBounds() const
    {
        return mCurrentBounds;
    }

    std::size_t MainMenuButton::GetPanelFlipFrameIndex(uint64_t nowTick) const
    {
        if (mAssets.panelFlip == nullptr)
        {
            return 0;
        }

        const std::size_t frameCount = mAssets.panelFlip->GetFrameCount();
        const float fps = mAssets.panelFlip->GetFramesPerSecond();
        if (frameCount == 0 || fps <= 0.0F)
        {
            return 0;
        }

        const uint64_t elapsed = nowTick - mPanelFlipStartTick;
        const std::size_t frameIndex = static_cast<std::size_t>(
            (static_cast<double>(elapsed) * static_cast<double>(fps)) / 1000.0);
        return SDL_min(frameIndex, frameCount - 1);
    }

    void MainMenuButton::DrawSequenceFrame(Game* game,
                                           Sequence* sequence,
                                           std::size_t frameIndex,
                                           const SDL_FRect& destination)
    {
        if (game == nullptr || game->mSpriteBatch == nullptr || sequence == nullptr)
        {
            return;
        }

        const std::size_t frameCount = sequence->GetFrameCount();
        if (frameCount == 0)
        {
            return;
        }

        const std::size_t clampedIndex = SDL_min(frameIndex, frameCount - 1);
        SDL_Surface* surface = sequence->GetSurface(clampedIndex);
        if (surface == nullptr)
        {
            return;
        }

        game->mSpriteBatch->Draw(surface, &destination);
    }
} // namespace nuvelocity
