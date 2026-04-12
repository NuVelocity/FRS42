#include "BarrierBrick.h"
#include "Ball.h"
#include <Game.h>
#include <algorithm>
#include <cmath>

namespace nuvelocity::frs42
{
    void BarrierBrick::Update(Game* aGame)
    {
        // Track mouse position for gravity effect and hover state.
        mMousePosition = aGame->mInput->GetMousePosition();
        Intersects(mMousePosition);
    }

    void BarrierBrick::Draw(Game* aGame)
    {
        if (aGame == nullptr || aGame->mSpriteBatch == nullptr || !mShowHoverEffect ||
            mHoveredSegment < 0 || mVertices.size() < 2)
        {
            return;
        }

        const size_t i = static_cast<size_t>(mHoveredSegment);
        const size_t next = (i + 1) % mVertices.size();

        aGame->mSpriteBatch->DrawLine(mPosition.x + mVertices[i].x,
                                      mPosition.y + mVertices[i].y,
                                      mPosition.x + mVertices[next].x,
                                      mPosition.y + mVertices[next].y,
                                      mHoverColor);
    }

    bool BarrierBrick::Intersects(const SDL_FPoint& point)
    {
        if (mVertices.size() < 2)
        {
            mHoveredSegment = -1;
            return false;
        }

        const float px = point.x - mPosition.x;
        const float py = point.y - mPosition.y;
        constexpr float kThresholdSq = 10.0f * 10.0f;

        const size_t segmentCount =
            (mVertices.size() == 2) ? 1 : mVertices.size(); // closed polygon wraps

        float bestDistSq = kThresholdSq;
        int bestSegment = -1;

        for (size_t i = 0; i < segmentCount; ++i)
        {
            const size_t next = (i + 1) % mVertices.size();
            const SDL_FPoint& a = mVertices[i];
            const SDL_FPoint& b = mVertices[next];

            const float dx = b.x - a.x;
            const float dy = b.y - a.y;
            const float lenSq = dx * dx + dy * dy;
            if (lenSq < 0.0001f)
                continue;

            const float t =
                std::max(0.0f, std::min(1.0f, ((px - a.x) * dx + (py - a.y) * dy) / lenSq));

            const float cx = a.x + t * dx;
            const float cy = a.y + t * dy;
            const float distSq = (px - cx) * (px - cx) + (py - cy) * (py - cy);

            if (distSq < bestDistSq)
            {
                bestDistSq = distSq;
                bestSegment = static_cast<int>(i);
            }
        }

        mHoveredSegment = bestSegment;
        return mHoveredSegment >= 0;
    }

    void BarrierBrick::ApplyAttraction(Game* aGame, Ball* ball, const SDL_FPoint& mousePos) const
    {
        if (!mAttractionEnabled || ball == nullptr || aGame == nullptr)
        {
            return;
        }

        const float deltaTime = aGame->GetDeltaTime();
        const SDL_FPoint pos = ball->GetPosition();
        const float dx = mousePos.x - pos.x;
        const float dy = mousePos.y - pos.y;
        const float distSq = dx * dx + dy * dy;
        const float dist = std::sqrt(distSq);

        if (dist < 0.0001f)
        {
            return;
        }

        const SDL_FPoint dir = {dx / dist, dy / dist};

        // Gravity parameters
        constexpr float kGravityStrength = 600.0f;
        constexpr float kMinSafeDist = 5.0f;
        constexpr float kDrag = 0.8f;

        SDL_FPoint velocity = ball->GetVelocity();

        float force = 0.0f;
        if (dist > kMinSafeDist)
        {
            // Attraction force
            force = kGravityStrength;
        }
        else
        {
            // Repulsion force to prevent convergence
            const float t = 1.0f - (dist / kMinSafeDist);
            force = -kGravityStrength * t * 3.0f;
        }

        velocity.x += dir.x * force * deltaTime;
        velocity.y += dir.y * force * deltaTime;

        // Apply drag to keep movement manageable
        velocity.x *= (1.0f - kDrag * deltaTime);
        velocity.y *= (1.0f - kDrag * deltaTime);

        ball->SetVelocity(velocity);
    }
} // namespace nuvelocity::frs42
