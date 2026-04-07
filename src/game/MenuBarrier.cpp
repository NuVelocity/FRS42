#include "MenuBarrier.h"
#include "Ball.h"
#include <Game.h>
#include <algorithm>
#include <cmath>

namespace nuvelocity::frs42
{
    void MenuBarrier::Update(float deltaTime)
    {
        // Menu barrier is usually static
    }

    void MenuBarrier::Draw(Game* game) const
    {
        if (game == nullptr || game->mSpriteBatch == nullptr || !mShowHoverEffect ||
            mHoveredSegment < 0 || mVertices.size() < 2)
        {
            return;
        }

        const size_t i = static_cast<size_t>(mHoveredSegment);
        const size_t next = (i + 1) % mVertices.size();

        game->mSpriteBatch->DrawLine(mPosition.x + mVertices[i].x,
                                     mPosition.y + mVertices[i].y,
                                     mPosition.x + mVertices[next].x,
                                     mPosition.y + mVertices[next].y,
                                     mHoverColor);
    }

    bool MenuBarrier::Intersects(const SDL_FPoint& point)
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

    void MenuBarrier::AttractBall(Ball* ball) const
    {
        if (ball == nullptr || mVertices.size() < 2)
            return;

        SDL_FPoint target = mPosition;
        if (mVertices.size() == 2)
        {
            // Attract to midpoint for line segments
            target.x += (mVertices[0].x + mVertices[1].x) * 0.5f;
            target.y += (mVertices[0].y + mVertices[1].y) * 0.5f;
        }

        // Vector from ball to target
        float dx = target.x - ball->GetPosition().x;
        float dy = target.y - ball->GetPosition().y;
        float dist = std::sqrt(dx * dx + dy * dy);

        if (dist > 0.001f)
        {
            float speed = 150.0f; // Force speed toward target
            ball->SetVelocity(SDL_FPoint{(dx / dist) * speed, (dy / dist) * speed});
        }
    }

    void MenuBarrier::OnClick(const std::vector<Ball*>& balls) const
    {
        for (auto* ball : balls)
        {
            AttractBall(ball);
        }
    }
} // namespace nuvelocity::frs42
