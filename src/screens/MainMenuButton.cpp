#include "MainMenuButton.h"
#include "SDL3/SDL_rect.h"

#include <Game.h>
#include <Sequence.h>
#include <cmath>
#include <system/AssetManager.h>
#include <system/AudioManager.h>
#include <system/FontManager.h>
#include <system/InputManager.h>
#include <system/SpriteBatch.h>

namespace nuvelocity
{
    constexpr const char* kMainMenuButtonFontName = "OCR";

    static SDL_Point GetSequenceFrameSurfaceSize(Sequence* sequence, std::size_t frameIndex)
    {
        if (sequence == nullptr)
        {
            return SDL_Point{.x = 0, .y = 0};
        }

        Frame* frame = sequence->GetFrame(frameIndex);
        if (frame == nullptr)
        {
            return SDL_Point{.x = 0, .y = 0};
        }

        SDL_Surface* surface = frame->GetSurface();
        if (surface == nullptr)
        {
            return SDL_Point{.x = 0, .y = 0};
        }

        return SDL_Point{.x = surface->w, .y = surface->h};
    }

    MainMenuButton::MainMenuButton()
            : mTargetBounds{.x = 0, .y = 0, .w = 0, .h = 0}
            , mCurrentBounds{.x = 0, .y = 0, .w = 0, .h = 0}
            , mRevealStarted(false)
            , mRevealComplete(false)
            , mPanelFlipComplete(false)
            , mRevealStartTick(0)
            , mPanelFlipStartTick(0)
            , mArmSequence(nullptr)
            , mPanelSequence(nullptr)
            , mPanelFrameIndex(0)
    {
        Button::Style baseStyle = GetButtonStyle();
        baseStyle.showFocusRing = false;
        Button::SetStyle(baseStyle);
    }

    void MainMenuButton::SetAssets(const MainMenuButtonAssets& assets)
    {
        mAssets = assets;

        const SDL_Point size = GetSize();
        if (size.x > 0 && size.y > 0)
        {
            mTargetBounds.w = size.x;
            mTargetBounds.h = size.y;
            mCurrentBounds.w = size.x;
            mCurrentBounds.h = size.y;
            SetRect(mCurrentBounds);
        }
    }

    void MainMenuButton::SetRect(const SDL_Rect& rect)
    {
        const SDL_Point size = GetSize();

        mTargetBounds = rect;
        if (size.x > 0 && size.y > 0)
        {
            mTargetBounds.w = size.x;
            mTargetBounds.h = size.y;
        }

        mCurrentBounds = rect;
        if (size.x > 0 && size.y > 0)
        {
            mCurrentBounds.w = size.x;
            mCurrentBounds.h = size.y;
        }

        Widget::SetRect(mCurrentBounds);
    }

    void MainMenuButton::SetStyle(const Style& style)
    {
        mStyle = style;
    }

    SDL_Point MainMenuButton::GetSize() const
    {
        SDL_Point size = GetSequenceFrameSurfaceSize(mAssets.armNormal, 0);
        if (size.x <= 0 || size.y <= 0)
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

    void MainMenuButton::Update(Game* game)
    {
        if (game == nullptr || game->mInput == nullptr)
        {
            return;
        }

        if (!IsVisible() || !IsEnabled())
        {
            mHovered = false;
            mPressed = false;
            return;
        }

        UpdateAnimation(game);

        mArmSequence = mAssets.armNormal;
        mPanelSequence = mAssets.panelFlip;
        mPanelFrameIndex = 0;

        if (!mRevealComplete)
        {
            mHovered = false;
            mPressed = false;
            return;
        }

        SetRect(mCurrentBounds);
        bool hoveredBefore = mHovered;
        Button::Update(game);

        if (mHovered && !hoveredBefore)
        {
            const char* hoverSound = "UI/Button Hover.ogg";
            AudioData* data = game->mAsset->LoadSound(hoverSound);
            if (data != nullptr)
            {
                game->mAudio->RegisterSfx(data);
                game->mAudio->PlaySfx(hoverSound);
            }
        }

        if (mHovered || mPressed || IsFocused())
        {
            mArmSequence = mAssets.armHover;
        }

        if (mPanelFlipComplete)
        {
            if (mPressed)
            {
                mPanelSequence = mAssets.panelPressed;
            }
            else if (mHovered || IsFocused())
            {
                mPanelSequence = mAssets.panelHover;
            }
            else
            {
                mPanelSequence = mAssets.panelNormal;
            }
        }
        else
        {
            mPanelFrameIndex = GetPanelFlipFrameIndex(SDL_GetTicks());
        }
    }

    void MainMenuButton::Draw(Game* game)
    {
        if (!IsVisible() || game == nullptr || game->mSpriteBatch == nullptr ||
            game->mFont == nullptr)
        {
            return;
        }

        const SDL_Rect renderBounds = GetRenderBounds();

        const SDL_Point panelSize = GetSequenceFrameSurfaceSize(mPanelSequence, mPanelFrameIndex);
        const SDL_Rect panelBounds{
            .x = renderBounds.x, .y = renderBounds.y, .w = panelSize.x, .h = panelSize.y};

        DrawSequenceFrame(game, mArmSequence, 0, renderBounds);
        DrawSequenceFrame(game, mPanelSequence, mPanelFrameIndex, panelBounds);

        if (mPanelFlipComplete)
        {
            game->mFont->DrawStringWithFontAt(kMainMenuButtonFontName,
                                              game->mSpriteBatch,
                                              GetDisplayCaption(),
                                              renderBounds.x + (panelSize.x / 2) + 20,
                                              renderBounds.y + (panelSize.y / 2) + 6,
                                              mStyle.textColor,
                                              13,
                                              TextAlignment::Center,
                                              nullptr,
                                              GetMnemonicIndex(),
                                              mStyle.textColor);
        }

        if (GetButtonStyle().showFocusRing && IsFocused() && !mHovered)
        {
            const SDL_Rect focusRect{.x = panelBounds.x + 2,
                                     .y = panelBounds.y + 2,
                                     .w = SDL_max(0, panelBounds.w - 4),
                                     .h = SDL_max(0, panelBounds.h - 4)};
            SDL_Color focusColor{.r = 255, .g = 255, .b = 255, .a = 64};
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

    bool MainMenuButton::Intersects(const SDL_Point& point) const
    {
        return point.x >= mCurrentBounds.x && point.y >= mCurrentBounds.y &&
               point.x <= mCurrentBounds.x + mCurrentBounds.w &&
               point.y <= mCurrentBounds.y + mCurrentBounds.h;
    }

    void MainMenuButton::UpdateAnimation(Game* game)
    {
        if (game == nullptr)
        {
            return;
        }

        uint64_t nowTick = SDL_GetTicks();

        if (!mRevealStarted)
        {
            ResetAnimation(nowTick);
        }

        const int startX = game->mWindowWidth + mTargetBounds.w;
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
        mCurrentBounds.x =
            startX +
            static_cast<int>(std::lround(static_cast<float>(mTargetBounds.x - startX) * progress));

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

                const char* flipSound = "UI/Button Flip.ogg";
                AudioData* data = game->mAsset->LoadSound(flipSound);
                if (data != nullptr)
                {
                    game->mAudio->RegisterSfx(data);
                    game->mAudio->PlaySfx(flipSound);
                }
            }
        }
    }

    SDL_Rect MainMenuButton::GetRenderBounds() const
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
                                           const SDL_Rect& destination)
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

        const SDL_Rect renderBounds = GetRenderBounds();

        const std::size_t clampedIndex = SDL_min(frameIndex, frameCount - 1);
        int x = destination.x + (renderBounds.w / 2);
        int y = destination.y + (renderBounds.h / 2);
        game->mSpriteBatch->Draw(sequence, clampedIndex, x, y);
    }
} // namespace nuvelocity
