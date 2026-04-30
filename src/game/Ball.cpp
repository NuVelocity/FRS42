#include "Ball.h"
#include "Playfield.h"
#include <Game.h>
#include <Sequence.h>
#include <cctype>
#include <cmath>
#include <numbers>
#include <system/AssetManager.h>
#include <system/SpriteBatch.h>

namespace nuvelocity::frs42
{
    Ball::Ball()
            : mSpeed(268.6F)
    {
    }

    void Ball::AttachSequence(Game* game, Sequence* sequence)
    {
        if (sequence != nullptr)
        {
            mNormalSequence = sequence;
        }
        else
        {
            mNormalSequence = game->mAsset->LoadSequence("Resources/Ball/Ball");
        }

        mSmallSequence = game->mAsset->LoadSequence("Resources/Ball/Small Ball");
        mFireSequence = game->mAsset->LoadSequence("Resources/Ball/Ball Fire");
        mRailSequence = game->mAsset->LoadSequence("Resources/Ball/Ball Rail");

        UpdateSequence(game);
        mAnimationStartTick = SDL_GetTicks();
    }

    void Ball::SetIsSmall(bool small)
    {
        if (mType != BallType::Normal)
        {
            small = false;
        }
        if (mIsSmall != small)
        {
            mIsSmall = small;
            UpdateSequence(nullptr);
        }
    }

    void Ball::SetType(BallType type)
    {
        if (mType != type)
        {
            mType = type;
            if (mType != BallType::Normal)
            {
                mIsSmall = false;
            }
            UpdateSequence(nullptr);
        }
    }

    void Ball::SpeedUp()
    {
        if (mIsTrapped)
        {
            return;
        }
        // Add a small increment on every bounce
        mSpeed += 1.5F;
    }

    void Ball::Update(Game* game)
    {
        if (mIsAttached || mType == BallType::Normal)
        {
            return;
        }

        // Handle particle trails for Fire/Rail ball
        mTrailTimer -= game->GetDeltaTime();
        if (mTrailTimer <= 0.0F)
        {
            mTrailTimer = 0.05F; // Spawn every 50ms

            if (mPlayfield != nullptr)
            {
                std::string pgen;
                if (mType == BallType::Fire)
                {
                    pgen = "Particle Generators/Balls/Fire Ball";
                }
                else if (mType == BallType::Rail)
                {
                    pgen = "Particle Generators/Balls/Rail Ball";
                }

                if (!pgen.empty())
                {
                    // Particles spawn at ball position, angle doesn't matter for trails usually
                    mPlayfield->SpawnParticleBurst(game, pgen, mPosition, 0.0F);
                }
            }
        }
    }

    void Ball::UpdateSequence(Game* game)
    {
        (void)game;
        if (mIsTrapped && mTrappedSequence != nullptr)
        {
            mSequence = mTrappedSequence;
        }
        else if (mType == BallType::Fire)
        {
            mSequence = mFireSequence;
        }
        else if (mType == BallType::Rail)
        {
            mSequence = mRailSequence;
        }
        else
        {
            mSequence = mIsSmall ? mSmallSequence : mNormalSequence;
        }
        mAnimationStartTick = SDL_GetTicks();
    }

    constexpr int kCircleSegments = 16;

    void Ball::Draw(Game* game)
    {
        if (mSequence == nullptr || game == nullptr || game->mSpriteBatch == nullptr)
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

        if (game->mSpriteBatch->IsDrawBoundsEnabled())
        {
            const float r = GetRadius();
            for (int i = 0; i < kCircleSegments; ++i)
            {
                const float a0 =
                    (static_cast<float>(i) / kCircleSegments) * 2.0F * std::numbers::pi_v<float>;
                const float a1 = (static_cast<float>(i + 1) / kCircleSegments) * 2.0F *
                                 std::numbers::pi_v<float>;
                game->mSpriteBatch->DrawLine(mPosition.x + (r * SDL_cosf(a0)),
                                             mPosition.y + (r * SDL_sinf(a0)),
                                             mPosition.x + (r * SDL_cosf(a1)),
                                             mPosition.y + (r * SDL_sinf(a1)),
                                             Colors::Green);
            }
        }
    }
} // namespace nuvelocity::frs42
