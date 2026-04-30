#include "PlayfieldBarrier.h"
#include "Ball.h"
#include <Game.h>
#include <algorithm>
#include <cmath>
#include <system/AudioManager.h>
#include <system/InputManager.h>
#include <system/SpriteBatch.h>

namespace nuvelocity::frs42
{
    PlayfieldBarrier::PlayfieldBarrier()
            : mMousePosition({.x = 0, .y = 0})
            , mHoveredSegment(-1) // index of hovered edge, -1 = none
            , mShowHoverEffect(true)
            , mHoverColor(Colors::White)
            , mAttractionEnabled(false)
    {
    }

    void PlayfieldBarrier::Update(Game* game)
    {
        // Track mouse position for gravity effect and hover state.
        mMousePosition = game->mInput->GetMousePosition();
        IntersectsMouse(mMousePosition);
    }

    void PlayfieldBarrier::Draw(Game* game)
    {
        Collidable2D::Draw(game);

        if (!mShowHoverEffect || mHoveredSegment < 0 || mCollisionPolygon.size() < 2)
        {
            return;
        }

        if (game != nullptr && game->mSpriteBatch != nullptr)
        {
            const size_t i = static_cast<size_t>(mHoveredSegment);
            const size_t next = (i + 1) % mCollisionPolygon.size();

            game->mSpriteBatch->DrawLine(
                static_cast<int>(std::lround(mPosition.x + mCollisionPolygon[i].x)),
                static_cast<int>(std::lround(mPosition.y + mCollisionPolygon[i].y)),
                static_cast<int>(std::lround(mPosition.x + mCollisionPolygon[next].x)),
                static_cast<int>(std::lround(mPosition.y + mCollisionPolygon[next].y)),
                mHoverColor);
        }
    }

    bool PlayfieldBarrier::IntersectsMouse(const SDL_Point& point)
    {
        if (mCollisionPolygon.size() < 2)
        {
            mHoveredSegment = -1;
            return false;
        }

        const float px = static_cast<float>(point.x) - mPosition.x;
        const float py = static_cast<float>(point.y) - mPosition.y;
        constexpr float kThresholdSq = 10.0F * 10.0F;

        if (mCollisionPolygon.size() < 2)
        {
            return false;
        }

        const size_t segmentCount =
            (mCollisionPolygon.size() == 2) ? 1 : mCollisionPolygon.size(); // closed polygon wraps

        float bestDistSq = kThresholdSq;
        int bestSegment = -1;

        for (size_t i = 0; i < segmentCount; ++i)
        {
            const size_t next = (i + 1) % mCollisionPolygon.size();
            const SDL_FPoint& a = mCollisionPolygon[i];
            const SDL_FPoint& b = mCollisionPolygon[next];

            const float dx = b.x - a.x;
            const float dy = b.y - a.y;
            const float lenSq = (dx * dx) + (dy * dy);
            if (lenSq < 0.0001F)
            {
                continue;
            }

            const float t =
                std::max(0.0F, std::min(1.0F, (((px - a.x) * dx) + ((py - a.y) * dy)) / lenSq));

            const float cx = a.x + (t * dx);
            const float cy = a.y + (t * dy);
            const float distSq = ((px - cx) * (px - cx)) + ((py - cy) * (py - cy));

            if (distSq < bestDistSq)
            {
                bestDistSq = distSq;
                bestSegment = static_cast<int>(i);
            }
        }

        mHoveredSegment = bestSegment;
        return mHoveredSegment >= 0;
    }

    void PlayfieldBarrier::ApplyAttraction(Game* game, Ball* ball, const SDL_FPoint& mousePos) const
    {
        if (!mAttractionEnabled || ball == nullptr || game == nullptr)
        {
            return;
        }

        const float deltaTime = game->GetDeltaTime();
        const SDL_FPoint pos = ball->GetPosition();
        const float dx = mousePos.x - pos.x;
        const float dy = mousePos.y - pos.y;
        const float distSq = (dx * dx) + (dy * dy);
        const float dist = std::sqrt(distSq);

        if (dist < 0.0001F)
        {
            return;
        }

        const SDL_FPoint dir = {.x = dx / dist, .y = dy / dist};

        // Gravity parameters
        constexpr float kGravityStrength = 600.0F;
        constexpr float kMinSafeDist = 5.0F;
        constexpr float kDrag = 0.8F;

        SDL_FPoint velocity = ball->GetVelocity();

        float force = 0.0F;
        if (dist > kMinSafeDist)
        {
            // Attraction force
            force = kGravityStrength;
        }
        else
        {
            // Repulsion force to prevent convergence
            const float t = 1.0F - (dist / kMinSafeDist);
            force = -kGravityStrength * t * 3.0F;
        }

        velocity.x += dir.x * force * deltaTime;
        velocity.y += dir.y * force * deltaTime;

        // Apply drag to keep movement manageable
        velocity.x *= (1.0F - (kDrag * deltaTime));
        velocity.y *= (1.0F - (kDrag * deltaTime));

        ball->SetVelocity(velocity);
    }

    void PlayfieldBarrier::OnHit(Game* game, const SDL_Rect& bounds)
    {
        (void)bounds;
        game->mAudio->PlaySfx("UI/Menu Ball Bounce.ogg");
    }
} // namespace nuvelocity::frs42
