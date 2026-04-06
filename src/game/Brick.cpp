#include "Brick.h"
#include <Game.h>
#include <system/SpriteBatch.h>

namespace nuvelocity::frs42
{
    std::vector<SDL_FPoint> Brick::GetCollisionPolygon() const
    {
        return mInfo.GetCollisionPolygon();
    }

    void Brick::Draw(Game* game) const
    {
        if (game == nullptr || game->mSpriteBatch == nullptr || mSequence == nullptr ||
            mIsDestroyed)
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
            SDL_FRect destRect{.x = mPosition.x,
                               .y = mPosition.y,
                               .w = static_cast<float>(surface->w),
                               .h = static_cast<float>(surface->h)};
            game->mSpriteBatch->Draw(surface, &destRect);
        }
    }
} // namespace nuvelocity::frs42
