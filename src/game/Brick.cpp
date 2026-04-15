#include "Brick.h"
#include <Game.h>
#include <system/SpriteBatch.h>

namespace nuvelocity::frs42
{
    std::vector<SDL_FPoint> Brick::GetCollisionPolygon() const
    {
        return mInfo.GetCollisionPolygon();
    }

    void Brick::Draw(Game* aGame)
    {
        if (aGame == nullptr || aGame->mSpriteBatch == nullptr || mSequence == nullptr ||
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

        aGame->mSpriteBatch->Draw(mSequence, frameIndex, mPosition.x, mPosition.y);
    }
} // namespace nuvelocity::frs42
