#include "Brick.h"
#include "BrickInfo.h"
#include "ParticleGeneratorInfo.h"
#include "Playfield.h"
#include <Game.h>
#include <Sequence.h>
#include <cmath>
#include <numbers>
#include <system/AssetManager.h>
#include <system/AudioManager.h>
#include <system/SpriteBatch.h>

namespace nuvelocity::frs42
{
    SDL_FRect Brick::GetCollidableBounds(Collidable2D* c)
    {
        const auto& poly = c->GetCollisionPolygon();
        const auto pos = c->GetPosition();
        if (poly.empty())
        {
            return SDL_FRect{pos.x, pos.y, 0, 0};
        }
        float minX = poly[0].x, maxX = poly[0].x;
        float minY = poly[0].y, maxY = poly[0].y;
        for (const auto& p : poly)
        {
            minX = std::min(minX, p.x);
            maxX = std::max(maxX, p.x);
            minY = std::min(minY, p.y);
            maxY = std::max(maxY, p.y);
        }
        return SDL_FRect{pos.x + minX, pos.y + minY, maxX - minX, maxY - minY};
    }

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
        UpdateBrickMovement(game);
        UpdateBrickDestroyed();
    }

    void Brick::UpdateBrickDestroyed()
    {
        if (mIsPlayingDestroyedAnimation && mDestroyedSequence != nullptr)
        {
            const uint64_t now = SDL_GetTicks();
            const uint64_t elapsed = now - mAnimationStartTick;
            const float fps = mDestroyedSequence->GetFramesPerSecond();
            const std::size_t frameCount = mDestroyedSequence->GetFrameCount();

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

    void Brick::UpdateBrickMovement(nuvelocity::Game* game)
    {
        if (mSpeed <= 0.0F)
        {
            return;
        }

        const float rad = static_cast<float>(mDirection) * std::numbers::pi_v<float> / 180.0F;
        const SDL_FPoint unit = {std::cos(rad), std::sin(rad)};
        const SDL_FPoint t1 = {mInitialPosition.x - (static_cast<float>(mRange1) * unit.x),
                               mInitialPosition.y - (static_cast<float>(mRange1) * unit.y)};
        const SDL_FPoint t2 = {t1.x + (static_cast<float>(mRange1 + mRange2) * unit.x),
                               t1.y + (static_cast<float>(mRange1 + mRange2) * unit.y)};

        const SDL_FPoint target = (mMoveTarget == 1) ? t1 : t2;
        const float dx = target.x - mPosition.x;
        const float dy = target.y - mPosition.y;
        const float dist = std::sqrt(dx * dx + dy * dy);
        const float moveStep = mSpeed * game->GetDeltaTime();

        if (dist <= moveStep)
        {
            mPosition = target;
            mMoveTarget = (mMoveTarget == 1) ? 2 : 1;
        }
        else if (dist > 0.001F)
        {
            const SDL_FPoint oldPos = mPosition;
            mPosition.x += (dx / dist) * moveStep;
            mPosition.y += (dy / dist) * moveStep;

            // Collision check with other bricks
            if (!mCanMoveThroughOtherBricks && mPlayfield != nullptr)
            {
                const SDL_FRect r1 = Brick::GetCollidableBounds(this);
                bool collided = false;
                for (const auto& other : mPlayfield->GetCollidables())
                {
                    if (other.get() == this || other->IsDestroyed())
                    {
                        continue;
                    }

                    const SDL_FRect r2 = Brick::GetCollidableBounds(other.get());
                    if (r1.x < r2.x + r2.w && r1.x + r1.w > r2.x && r1.y < r2.y + r2.h &&
                        r1.y + r1.h > r2.y)
                    {
                        collided = true;
                        break;
                    }
                }

                if (collided)
                {
                    mPosition = oldPos;
                }
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

        Sequence* seqToDraw = mIsPlayingDestroyedAnimation ? mDestroyedSequence : mSequence;
        if (game->mSpriteBatch == nullptr || seqToDraw == nullptr)
        {
            return;
        }

        const std::size_t frameCount = seqToDraw->GetFrameCount();
        if (frameCount == 0)
        {
            return;
        }

        const uint64_t now = SDL_GetTicks();
        const uint64_t elapsed = now - mAnimationStartTick;
        const float fps = seqToDraw->GetFramesPerSecond();

        std::size_t frameIndex = 0;
        if (mIsPlayingDestroyedAnimation)
        {
            frameIndex = static_cast<std::size_t>((static_cast<double>(elapsed) * fps) / 1000.0);
            if (frameIndex >= frameCount)
            {
                frameIndex = frameCount - 1;
            }
            return;
        }

        frameIndex =
            static_cast<std::size_t>((static_cast<double>(elapsed) * fps) / 1000.0) % frameCount;

        int x = static_cast<int>(GetPosition().x);
        int y = static_cast<int>(GetPosition().y);

        if (IsChangeBrick())
        {
            // XXX: RX does not clip the Change From sequence.
            frameIndex = static_cast<std::size_t>((static_cast<double>(elapsed) *
                                                   mChangeFromSequence->GetFramesPerSecond()) /
                                                  1000.0) %
                         mChangeFromSequence->GetFrameCount();
            game->mSpriteBatch->Draw(mChangeFromSequence, frameIndex, x, y);

            Frame* f = mChangeToSequence->GetFrame(frameIndex);
            int changeToX = x + f->GetHotSpot().x;
            int changeToY = y + f->GetHotSpot().y;
            int changeWidth = f->GetWidth();
            int changeHeight = f->GetHeight();
            const SDL_Rect clipRight = {.x = changeToX + changeWidth / 2,
                                        .y = changeToY,
                                        .w = changeWidth - (changeWidth / 2),
                                        .h = changeHeight};
            game->mSpriteBatch->SetClipRect(&clipRight);
            frameIndex = static_cast<std::size_t>((static_cast<double>(elapsed) *
                                                   mChangeToSequence->GetFramesPerSecond()) /
                                                  1000.0) %
                         mChangeToSequence->GetFrameCount();

            game->mSpriteBatch->Draw(mChangeToSequence, frameIndex, x, y);

            game->mSpriteBatch->SetClipRect(nullptr);
        }
        else
        {
            game->mSpriteBatch->Draw(seqToDraw, frameIndex, x, y);
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

            if (mInfo->GetDestroyedSeqPath() != "!None")
            {
                mDestroyedSequence =
                    game->mAsset->LoadSequence("Resources/Effects/" + mInfo->GetDestroyedSeqPath());
                mAnimationStartTick = SDL_GetTicks();
            }

            if (mPlayfield != nullptr)
            {
                if (mInfo->GetBreakParticleGen() != nullptr)
                {
                    mPlayfield->SpawnParticleBurst(mInfo->GetBreakParticleGen(),
                                                   GetPosition(),
                                                   &mInfo->GetBreakParticleTypes());
                }

                if (mIsCompleted)
                {
                    return;
                }

                mPlayfield->AddScore(mInfo->GetScoreValue());
                if (mBrickToLookLike != "Bricks/!None")
                {
                    mPlayfield->TriggerChainReaction(
                        game, this, mBrickToLookLike, mBrickToChangeTo);
                }
                if (mForcePowerUp != "No Power-Up")
                {
                    mPlayfield->SpawnPowerUp(
                        game, GetPosition(), PowerUp::TypeFromString(mForcePowerUp));
                }
                else if (type == BrickType::PowerUp)
                {
                    mPlayfield->SpawnPowerUpAt(game, GetPosition());
                }

                if (type == BrickType::Exploding)
                {
                    mPlayfield->TriggerExplosion(game, this);
                }

                mPlayfield->GetGameStats().mBricksDestroyed++;

                mIsCompleted = true;
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
