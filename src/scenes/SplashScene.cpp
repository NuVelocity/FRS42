#include <Game.h>
#include <SDL3/SDL.h>
#include <system/SpriteBatch.h>

#include "MainMenuScene.h"
#include "SplashScene.h"

namespace nuvelocity::frs42
{
    uint8_t SplashScene::GetBlackOverlayAlpha(float elapsedSeconds) const
    {
        if (mCurrentFrameIndex >= mFrames.size())
        {
            return SDL_ALPHA_OPAQUE;
        }

        const float fadeDuration = static_cast<float>(kFadeDurationMs) / 1000.0f;
        const float holdDuration = static_cast<float>(kHoldDurationMs) / 1000.0f;
        const float frameDuration = (fadeDuration * 2.0f) + holdDuration;

        const float clampedElapsed = std::min(elapsedSeconds, frameDuration);

        if (clampedElapsed < fadeDuration)
        {
            const float progress = clampedElapsed / fadeDuration;
            return static_cast<uint8_t>((1.0f - progress) * 255.0f);
        }

        if (clampedElapsed < fadeDuration + holdDuration)
        {
            return SDL_ALPHA_TRANSPARENT;
        }

        const float fadeOutElapsed = clampedElapsed - (fadeDuration + holdDuration);
        const float progress = fadeOutElapsed / fadeDuration;
        return static_cast<uint8_t>(progress * 255.0f);
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
        mElapsedFrameTime = 0.0f;
        mTransitioned = false;
    }

    void SplashScene::Update(Game* aGame)
    {
        if (mTransitioned)
        {
            return;
        }

        const float deltaTime = aGame->GetDeltaTime();
        mElapsedFrameTime += deltaTime;

        const float fadeDuration = static_cast<float>(kFadeDurationMs) / 1000.0f;
        const float holdDuration = static_cast<float>(kHoldDurationMs) / 1000.0f;
        const float frameDuration = (fadeDuration * 2.0f) + holdDuration;

        // Clicking while the splash screen is fully visible will skip it.
        if (aGame->mInput != nullptr && aGame->mInput->IsMouseButtonPressed(SDL_BUTTON_LEFT) &&
            mElapsedFrameTime >= fadeDuration && mElapsedFrameTime < fadeDuration + holdDuration)
        {
            mElapsedFrameTime = fadeDuration + holdDuration;
            return;
        }

        if (mElapsedFrameTime < frameDuration)
        {
            return;
        }

        if (mCurrentFrameIndex + 1 < mFrames.size())
        {
            ++mCurrentFrameIndex;
            mElapsedFrameTime = 0.0f;
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

        const uint8_t overlayAlpha = GetBlackOverlayAlpha(mElapsedFrameTime);
        if (overlayAlpha > SDL_ALPHA_TRANSPARENT)
        {
            aGame->mSpriteBatch->FillRect(nullptr, SDL_Color{0, 0, 0, overlayAlpha});
        }
    }

    std::string SplashScene::GetName() const
    {
        return "SplashScene";
    }
} // namespace nuvelocity::frs42
