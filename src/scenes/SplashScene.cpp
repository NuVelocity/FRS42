#include <Game.h>
#include <SDL3/SDL.h>
#include <system/AssetManager.h>
#include <system/AudioManager.h>
#include <system/InputManager.h>
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

        const float fadeDuration = static_cast<float>(kFadeDurationMs) / 1000.0F;
        const float holdDuration = static_cast<float>(kHoldDurationMs) / 1000.0F;
        const float frameDuration = (fadeDuration * 2.0F) + holdDuration;

        const float clampedElapsed = std::min(elapsedSeconds, frameDuration);

        if (clampedElapsed < fadeDuration)
        {
            const float progress = clampedElapsed / fadeDuration;
            return static_cast<uint8_t>((1.0F - progress) * 255.0F);
        }

        if (clampedElapsed < fadeDuration + holdDuration)
        {
            return SDL_ALPHA_TRANSPARENT;
        }

        const float fadeOutElapsed = clampedElapsed - (fadeDuration + holdDuration);
        const float progress = fadeOutElapsed / fadeDuration;
        return static_cast<uint8_t>(progress * 255.0F);
    }

    void SplashScene::Load(Game* game)
    {
        game->mAudio->PlayBgm("Theme.ogg");

        mFrames[0].assetPath = "Resources/Splash Screens/CompanyLogo";
        mFrames[1].assetPath = "Resources/Splash Screens/Splash";

        for (SplashFrame& splashFrame : mFrames)
        {
            splashFrame.frame = game->mAsset->LoadStandAloneFrame(splashFrame.assetPath);
        }

        mCurrentFrameIndex = 0;
        mElapsedFrameTime = 0.0F;
        mTransitioned = false;
    }

    void SplashScene::Update(Game* game)
    {
        if (mTransitioned)
        {
            return;
        }

        const float deltaTime = game->GetDeltaTime();
        mElapsedFrameTime += deltaTime;

        const float fadeDuration = static_cast<float>(kFadeDurationMs) / 1000.0F;
        const float holdDuration = static_cast<float>(kHoldDurationMs) / 1000.0F;
        const float frameDuration = (fadeDuration * 2.0F) + holdDuration;

        // Clicking while the splash screen is fully visible will skip it.
        if (game->mInput != nullptr && game->mInput->IsMouseButtonPressed(SDL_BUTTON_LEFT) &&
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
            mElapsedFrameTime = 0.0F;
            return;
        }

        mTransitioned = true;
        game->SetScene(new MainMenuScene());
    }

    void SplashScene::Draw(Game* game)
    {
        if (game == nullptr || game->mSpriteBatch == nullptr)
        {
            return;
        }

        game->mSpriteBatch->Clear(SDL_Color{.r = 0, .g = 0, .b = 0, .a = SDL_ALPHA_OPAQUE});

        if (mCurrentFrameIndex < mFrames.size())
        {
            SplashFrame& splashFrame = mFrames[mCurrentFrameIndex];
            if (splashFrame.frame != nullptr)
            {
                game->mSpriteBatch->DrawCentered(splashFrame.frame);
            }
        }

        const uint8_t overlayAlpha = GetBlackOverlayAlpha(mElapsedFrameTime);
        if (overlayAlpha > SDL_ALPHA_TRANSPARENT)
        {
            game->mSpriteBatch->FillRect(nullptr,
                                         SDL_Color{.r = 0, .g = 0, .b = 0, .a = overlayAlpha});
        }
    }

    std::string SplashScene::GetName() const
    {
        return "SplashScene";
    }
} // namespace nuvelocity::frs42
