#include "Brick.h"
#include "BrickInfo.h"
#include "ParticleGeneratorInfo.h"
#include "Playfield.h"
#include <Game.h>
#include <Sequence.h>
#include <cmath>
#include <system/AssetManager.h>
#include <system/AudioManager.h>
#include <system/SpriteBatch.h>

namespace nuvelocity::frs42
{
    void Brick::AttachBrickInfo(Game* game, const BrickInfo* info)
    {
        mInfo = info;
        if (mInfo->GetPrimarySequencePath() != "!None")
        {
            mSequence = game->mAsset->LoadSequence("Resources/" + mInfo->GetPrimarySequencePath());
        }
        mAnimationStartTick = SDL_GetTicks();
        mIsDestroyed = false;
        mIsPlayingDestroyedAnimation = false;

        const BrickType type = mInfo->GetBrickType();
        if (type == BrickType::ThreeHit || type == BrickType::ThreeHitBottom ||
            type == BrickType::ThreeHitTop)
        {
            mHitsRemaining = 3;
        }
        else
        {
            mHitsRemaining = 1;
        }

        auto registerSfx = [&](const std::string& path)
        {
            if (path != "!None")
            {
                if (AudioData* data = game->mAsset->LoadSound(path))
                {
                    game->mAudio->RegisterSfx(data);
                }
            }
        };

        registerSfx(mInfo->GetDamagedSoundPath());
        registerSfx(mInfo->GetIndestructibleSoundPath());
        registerSfx(mInfo->GetDestroyedSoundPath());
    }

    void Brick::Update(Game* game)
    {
        (void)game;
        if (mIsPlayingDestroyedAnimation && mSequence != nullptr)
        {
            const uint64_t now = SDL_GetTicks();
            const uint64_t elapsed = now - mAnimationStartTick;
            const float fps = mSequence->GetFramesPerSecond();
            const std::size_t frameCount = mSequence->GetFrameCount();

            if (frameCount > 0)
            {
                const std::size_t frameIndex =
                    static_cast<std::size_t>((static_cast<double>(elapsed) * fps) / 1000.0);
                if (frameIndex >= frameCount)
                {
                    mIsPlayingDestroyedAnimation = false;
                }
            }
            else
            {
                mIsPlayingDestroyedAnimation = false;
            }
        }
    }

    void Brick::Draw(Game* game)
    {
        Collidable2D::Draw(game);

        if (game == nullptr || (mIsDestroyed && !mIsPlayingDestroyedAnimation))
        {
            return;
        }

        if (game->mSpriteBatch != nullptr && mSequence != nullptr)
        {
            const std::size_t frameCount = mSequence->GetFrameCount();
            if (frameCount == 0)
            {
                return;
            }

            const uint64_t now = SDL_GetTicks();
            const uint64_t elapsed = now - mAnimationStartTick;
            const float fps = mSequence->GetFramesPerSecond();

            std::size_t frameIndex = 0;
            if (mIsPlayingDestroyedAnimation)
            {
                frameIndex =
                    static_cast<std::size_t>((static_cast<double>(elapsed) * fps) / 1000.0);
                if (frameIndex >= frameCount)
                {
                    frameIndex = frameCount - 1;
                }
            }
            else
            {
                frameIndex =
                    static_cast<std::size_t>((static_cast<double>(elapsed) * fps) / 1000.0) %
                    frameCount;
            }

            game->mSpriteBatch->Draw(mSequence,
                                     frameIndex,
                                     static_cast<int>(GetPosition().x),
                                     static_cast<int>(GetPosition().y));
        }
    }

    std::vector<SDL_FPoint> Brick::GetCollisionPolygon() const
    {
        if (GetInfo() == nullptr)
        {
            return {};
        }

        return GetInfo()->GetCollisionPolygon();
    }

    void Brick::OnHit(Game* game, const SDL_Rect& bounds)
    {
        const BrickType type = mInfo->GetBrickType();

        const bool isIndestructible = IsIndestructibleType(type);
        if (isIndestructible)
        {
            if (mInfo->GetIndestructibleSoundPath() != "!None")
            {
                game->mAudio->PlaySfx(mInfo->GetIndestructibleSoundPath());
            }

            // Nudge logic
            if (type == BrickType::NudgeUp || type == BrickType::NudgeDown ||
                type == BrickType::NudgeLeft || type == BrickType::NudgeRight)
            {
                const auto poly = GetCollisionPolygon();
                float minX = 0;
                float maxX = 0;
                float minY = 0;
                float maxY = 0;
                if (!poly.empty())
                {
                    minX = maxX = poly[0].x;
                    minY = maxY = poly[0].y;
                    for (const auto& p : poly)
                    {
                        minX = std::min(minX, p.x);
                        maxX = std::max(maxX, p.x);
                        minY = std::min(minY, p.y);
                        maxY = std::max(maxY, p.y);
                    }
                }
                const float width = maxX - minX;
                const float height = maxY - minY;

                bool hitBounds = false;
                if (type == BrickType::NudgeUp)
                {
                    mPosition.y -= height;
                    if (GetPosition().y + minY < static_cast<float>(bounds.y))
                    {
                        hitBounds = true;
                    }
                }
                else if (type == BrickType::NudgeDown)
                {
                    mPosition.y += height;
                    if (GetPosition().y + maxY > static_cast<float>(bounds.y + bounds.h))
                    {
                        hitBounds = true;
                    }
                }
                else if (type == BrickType::NudgeLeft)
                {
                    mPosition.x -= width;
                    if (GetPosition().x + minX < static_cast<float>(bounds.x))
                    {
                        hitBounds = true;
                    }
                }
                else if (type == BrickType::NudgeRight)
                {
                    mPosition.x += width;
                    if (GetPosition().x + maxX > static_cast<float>(bounds.x + bounds.w))
                    {
                        hitBounds = true;
                    }
                }

                if (hitBounds)
                {
                    mHitsRemaining = 0; // Trigger destruction after move
                }
                else
                {
                    return; // Indestructible nudge moved but didn't die
                }
            }
            else
            {
                return; // Other indestructible types just play sound
            }
        }

        if (mHitsRemaining > 0)
        {
            if (mInfo->GetDamagedSoundPath() != "!None")
            {
                game->mAudio->PlaySfx(mInfo->GetDamagedSoundPath());
            }

            // Damage logic
            mHitsRemaining--;

            // Visual updates for 3-hit bricks
            if (type == BrickType::ThreeHit || type == BrickType::ThreeHitBottom ||
                type == BrickType::ThreeHitTop)
            {
                if (mHitsRemaining == 2 && mInfo->GetSequence2Path() != "!None")
                {
                    mSequence =
                        game->mAsset->LoadSequence("Resources/" + mInfo->GetSequence2Path());
                    mAnimationStartTick = SDL_GetTicks();
                }
                else if (mHitsRemaining == 1 && mInfo->GetSequence3Path() != "!None")
                {
                    mSequence =
                        game->mAsset->LoadSequence("Resources/" + mInfo->GetSequence3Path());
                    mAnimationStartTick = SDL_GetTicks();
                }
            }
        }

        // Destruction trigger
        if (mHitsRemaining <= 0)
        {
            mIsDestroyed = true;
            mIsPlayingDestroyedAnimation = true;
            if (mInfo->GetDestroyedSoundPath() != "!None")
            {
                game->mAudio->PlaySfx(mInfo->GetDestroyedSoundPath());
            }

            if (mPlayfield != nullptr && mInfo->GetBreakParticleGen() != nullptr)
            {
                mPlayfield->SpawnParticleBurst(
                    mInfo->GetBreakParticleGen(), GetPosition(), &mInfo->GetBreakParticleTypes());
            }

            if (mInfo->GetDestroyedSeqPath() != "!None")
            {
                mSequence =
                    game->mAsset->LoadSequence("Resources/Effects/" + mInfo->GetDestroyedSeqPath());
                mAnimationStartTick = SDL_GetTicks();
            }
        }
    }

    bool Brick::IsIndestructibleType(BrickType type)
    {
        return (type == BrickType::Obstacle || type == BrickType::Indestructible ||
                type == BrickType::IndestructibleTop || type == BrickType::IndestructibleBottom ||
                type == BrickType::PushAway || type == BrickType::NudgeUp ||
                type == BrickType::NudgeDown || type == BrickType::NudgeLeft ||
                type == BrickType::NudgeRight || type == BrickType::BrickShredder);
    }
} // namespace nuvelocity::frs42
