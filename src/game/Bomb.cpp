#include "Bomb.h"
#include <Frame.h>
#include <Game.h>
#include <Sequence.h>
#include <cmath>
#include <system/SpriteBatch.h>

namespace nuvelocity::frs42
{
    Bomb::Bomb(const SDL_FPoint& pos, Sequence* seq)
            : mPosition(pos)
            , mSequence(seq)
    {
        mVelocity = {0.0F, 120.0F}; // Slightly faster than powerup
        mStartTick = SDL_GetTicks();
    }

    void Bomb::Update(Game* game)
    {
        float dt = game->GetDeltaTime();
        mPosition.y += mVelocity.y * dt;
        if (mPosition.y > static_cast<float>(game->mWindowHeight) + 50.0F)
        {
            mIsDead = true;
        }
    }

    void Bomb::Draw(Game* game)
    {
        if (mSequence == nullptr || game == nullptr || game->mSpriteBatch == nullptr)
        {
            return;
        }

        const uint64_t now = SDL_GetTicks();
        const float fps = mSequence->GetFramesPerSecond();
        const std::size_t frameCount = mSequence->GetFrameCount();
        if (frameCount == 0)
        {
            return;
        }
        const std::size_t frameIndex =
            static_cast<std::size_t>((static_cast<double>(now - mStartTick) * fps) / 1000.0) %
            frameCount;

        if (const Frame* frame = mSequence->GetFrame(frameIndex); frame != nullptr)
        {
            SDL_Surface* surface = frame->GetSurface();
            SDL_Rect destRect{
                .x = static_cast<int>(std::lround(mPosition.x)) + frame->GetHotSpot().x,
                .y = static_cast<int>(std::lround(mPosition.y)) + frame->GetHotSpot().y,
                .w = surface->w,
                .h = surface->h};
            game->mSpriteBatch->Draw(surface, &destRect);
        }
    }
} // namespace nuvelocity::frs42
