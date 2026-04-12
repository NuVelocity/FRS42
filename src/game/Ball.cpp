#include "Ball.h"
#include <system/SpriteBatch.h>

namespace nuvelocity::frs42
{
    void Ball::Update(Game* aGame)
    {
        float deltaTime = aGame->GetDeltaTime();
        mPosition.x += mDirection.x * mSpeed * deltaTime;
        mPosition.y += mDirection.y * mSpeed * deltaTime;
    }

    void Ball::Draw(Game* aGame)
    {
        if (mSequence == nullptr || aGame == nullptr || aGame->mSpriteBatch == nullptr)
        {
            return;
        }

        const std::size_t frameCount = mSequence->GetFrameCount();
        if (frameCount == 0)
        {
            return;
        }

        const uint64_t now = SDL_GetTicks();
        const uint64_t elapsed = now - mAnimationStartTick;
        const float fps = mSequence->GetFramesPerSecond();
        const std::size_t frameIndex =
            static_cast<std::size_t>((static_cast<double>(elapsed) * fps) / 1000.0) % frameCount;

        SDL_Surface* surface = mSequence->GetSurface(frameIndex);
        if (surface != nullptr)
        {
            const float hw = static_cast<float>(surface->w) * 0.5f;
            const float hh = static_cast<float>(surface->h) * 0.5f;
            SDL_FRect destRect{.x = mPosition.x - hw,
                               .y = mPosition.y - hh,
                               .w = static_cast<float>(surface->w),
                               .h = static_cast<float>(surface->h)};
            aGame->mSpriteBatch->Draw(surface, &destRect);
        }
    }
} // namespace nuvelocity::frs42
