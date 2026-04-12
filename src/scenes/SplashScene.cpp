#include <Game.h>
#include <SDL3/SDL.h>
#include <system/SpriteBatch.h>

#include "MainMenuScene.h"
#include "SplashScene.h"

namespace nuvelocity::frs42
{
    uint8_t SplashScene::GetBlackOverlayAlpha(uint64_t nowTick) const
    {
        if (mCurrentFrameIndex >= mFrames.size() || mFrameStartTick == 0)
        {
            return SDL_ALPHA_OPAQUE;
        }

        const uint64_t elapsedMs = nowTick - mFrameStartTick;
        const uint64_t frameDurationMs = (kFadeDurationMs * 2) + kHoldDurationMs;
        const uint64_t clampedElapsedMs = SDL_min(elapsedMs, frameDurationMs);

        if (clampedElapsedMs < kFadeDurationMs)
        {
            const float progress =
                static_cast<float>(clampedElapsedMs) / static_cast<float>(kFadeDurationMs);
            return static_cast<uint8_t>((1.0F - progress) * 255.0F);
        }

        if (clampedElapsedMs < kFadeDurationMs + kHoldDurationMs)
        {
            return SDL_ALPHA_TRANSPARENT;
        }

        const uint64_t fadeOutElapsedMs = clampedElapsedMs - (kFadeDurationMs + kHoldDurationMs);
        const float progress =
            static_cast<float>(fadeOutElapsedMs) / static_cast<float>(kFadeDurationMs);
        return static_cast<uint8_t>(progress * 255.0F);
    }

    void SplashScene::Load(Game* aGame)
    {
        aGame->mAudio->PlayBgm("Rock Fast");

        mFrames[0].assetPath = "Resources/Splash Screens/CompanyLogo";
        mFrames[1].assetPath = "Resources/Splash Screens/Splash";

        for (SplashFrame& splashFrame : mFrames)
        {
            auto* frame = aGame->mAsset->LoadStandAloneFrame(splashFrame.assetPath);
            if (frame != nullptr)
            {
                splashFrame.image = Image(*frame);
            }
        }

        mCurrentFrameIndex = 0;
        mFrameStartTick = SDL_GetTicks();
        mTransitioned = false;
    }

    void SplashScene::Update(Game* aGame)
    {
        if (mTransitioned || mFrameStartTick == 0)
        {
            return;
        }

        const uint64_t nowTick = SDL_GetTicks();
        const uint64_t elapsedMs = nowTick - mFrameStartTick;
        const uint64_t frameDurationMs = (kFadeDurationMs * 2) + kHoldDurationMs;

        // Clicking while the splash screen is fully visible will skip it.
        if (aGame->mInput != nullptr && aGame->mInput->IsMouseButtonPressed(SDL_BUTTON_LEFT) &&
            elapsedMs >= kFadeDurationMs && elapsedMs < kFadeDurationMs + kHoldDurationMs)
        {
            mFrameStartTick = nowTick - (kFadeDurationMs + kHoldDurationMs);
            return;
        }

        if (elapsedMs < frameDurationMs)
        {
            return;
        }

        if (mCurrentFrameIndex + 1 < mFrames.size())
        {
            ++mCurrentFrameIndex;
            mFrameStartTick = nowTick;
            return;
        }

        mTransitioned = true;
        aGame->SetScene(new MainMenuScene());
    }

    void SplashScene::Draw(Game* aGame)
    {
        if (aGame == nullptr || aGame->mSpriteBatch == nullptr)
        {
            return;
        }

        aGame->mSpriteBatch->Clear(SDL_Color{0, 0, 0, SDL_ALPHA_OPAQUE});

        if (mCurrentFrameIndex < mFrames.size())
        {
            SplashFrame& splashFrame = mFrames[mCurrentFrameIndex];
            if (splashFrame.image.IsValid())
            {
                aGame->mSpriteBatch->DrawCentered(splashFrame.image.GetSurface());
            }
        }

        const uint8_t overlayAlpha = GetBlackOverlayAlpha(SDL_GetTicks());
        if (overlayAlpha > SDL_ALPHA_TRANSPARENT)
        {
            aGame->mSpriteBatch->FillRect(nullptr, SDL_Color{0, 0, 0, overlayAlpha});
        }

        aGame->mSpriteBatch->Present();
    }

    std::string SplashScene::GetName() const
    {
        return "SplashScene";
    }
} // namespace nuvelocity::frs42
