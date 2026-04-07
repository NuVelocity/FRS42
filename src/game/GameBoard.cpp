#include "GameBoard.h"
#include "Ball.h"
#include "Brick.h"
#include <Game.h>
#include <algorithm>
#include <cmath>

namespace nuvelocity::frs42
{
    GameBoard::GameBoard() = default;
    GameBoard::~GameBoard() = default;

    void GameBoard::Draw(Game* game) const
    {
        for (const auto& brick : mBricks)
        {
            brick->Draw(game);
        }
        for (const auto& ball : mBalls)
        {
            ball->Draw(game);
        }

        if (mDebugDrawCollisions && game != nullptr && game->mSpriteBatch != nullptr)
        {
            constexpr SDL_Color kWireColor{0, 220, 160, 200};
            for (const auto& brick : mBricks)
            {
                if (brick->IsDestroyed())
                    continue;

                std::vector<SDL_FPoint> poly = brick->GetCollisionPolygon();
                SDL_FPoint brickPos = brick->GetPosition();
                for (auto& p : poly)
                {
                    p.x += brickPos.x;
                    p.y += brickPos.y;
                }
                if (poly.size() < 2)
                    continue;

                const size_t segmentCount =
                    brick->IsClosedPolygon() ? poly.size() : (poly.size() - 1);
                for (size_t i = 0; i < segmentCount; ++i)
                {
                    const size_t next = (i + 1) % poly.size();
                    game->mSpriteBatch->DrawLine(
                        poly[i].x, poly[i].y, poly[next].x, poly[next].y, kWireColor);
                }
            }

            constexpr SDL_Color kBallWireColor{255, 220, 0, 220};
            constexpr int kCircleSegments = 16;
            for (const auto& ball : mBalls)
            {
                const SDL_FPoint center = ball->GetPosition();
                const float r = ball->GetRadius();
                for (int i = 0; i < kCircleSegments; ++i)
                {
                    const float a0 = (static_cast<float>(i) / kCircleSegments) * 2.0f * SDL_PI_F;
                    const float a1 =
                        (static_cast<float>(i + 1) / kCircleSegments) * 2.0f * SDL_PI_F;
                    game->mSpriteBatch->DrawLine(center.x + r * SDL_cosf(a0),
                                                 center.y + r * SDL_sinf(a0),
                                                 center.x + r * SDL_cosf(a1),
                                                 center.y + r * SDL_sinf(a1),
                                                 kBallWireColor);
                }
            }
        }
    }

    // Basic Circle-to-Line collision helper
    // Returns true if colliding; outNormal is the separation normal, outPenetration is how far in
    bool CircleToSegmentCollision(const SDL_FPoint& center,
                                  float radius,
                                  const SDL_FPoint& p1,
                                  const SDL_FPoint& p2,
                                  SDL_FPoint& outNormal,
                                  float& outPenetration)
    {
        float dx = p2.x - p1.x;
        float dy = p2.y - p1.y;
        float lenSq = dx * dx + dy * dy;
        if (lenSq < 0.0001f)
            return false;

        float t = ((center.x - p1.x) * dx + (center.y - p1.y) * dy) / lenSq;
        if (t < 0.0f || t > 1.0f)
            return false; // Only collide with the segment surface, not near endpoints

        float closestX = p1.x + t * dx;
        float closestY = p1.y + t * dy;

        float distDx = center.x - closestX;
        float distDy = center.y - closestY;
        float distSq = distDx * distDx + distDy * distDy;

        if (distSq < radius * radius)
        {
            float dist = std::sqrt(distSq);
            outPenetration = radius - dist;
            if (dist > 0.0001f)
            {
                outNormal.x = distDx / dist;
                outNormal.y = distDy / dist;
            }
            else
            {
                // Degenerate case: center exactly on segment, use segment perpendicular
                outNormal.x = -dy / std::sqrt(lenSq);
                outNormal.y = dx / std::sqrt(lenSq);
            }
            return true;
        }
        return false;
    }

    void GameBoard::Update(Game* game, float deltaTime)
    {
        for (auto& ball : mBalls)
        {
            ball->Update(deltaTime);

            SDL_FPoint pos = ball->GetPosition();
            float radius = ball->GetRadius();
            SDL_FPoint vel = ball->GetVelocity();

            // Screen boundary checks (Bounce)
            if (pos.x - radius < 0)
            {
                pos.x = radius;
                vel.x = std::abs(vel.x);
                ball->SetVelocity(vel);
                ball->SetPosition(pos);
            }
            else if (pos.x + radius > static_cast<float>(game->mWindowWidth))
            {
                pos.x = static_cast<float>(game->mWindowWidth) - radius;
                vel.x = -std::abs(vel.x);
                ball->SetVelocity(vel);
                ball->SetPosition(pos);
            }

            if (pos.y - radius < 0)
            {
                pos.y = radius;
                vel.y = std::abs(vel.y);
                ball->SetVelocity(vel);
                ball->SetPosition(pos);
            }
            else if (pos.y + radius > static_cast<float>(game->mWindowHeight))
            {
                pos.y = static_cast<float>(game->mWindowHeight) - radius;
                vel.y = -std::abs(vel.y);
                ball->SetVelocity(vel);
                ball->SetPosition(pos);
            }

            // Sync values for brick collision logic below
            pos = ball->GetPosition();
            vel = ball->GetVelocity();

            for (auto& brick : mBricks)
            {
                if (brick->IsDestroyed())
                    continue;

                std::vector<SDL_FPoint> poly = brick->GetCollisionPolygon();
                SDL_FPoint brickPos = brick->GetPosition();

                // Offset polygon to world space
                for (auto& p : poly)
                {
                    p.x += brickPos.x;
                    p.y += brickPos.y;
                }

                if (poly.empty())
                    continue;

                SDL_FPoint normal{0, 0};
                float penetration = 0.0f;
                bool collided = false;

                const size_t segmentCount =
                    brick->IsClosedPolygon() ? poly.size() : (poly.size() - 1);
                for (size_t i = 0; i < segmentCount; ++i)
                {
                    size_t next = (i + 1) % poly.size();
                    SDL_FPoint segNormal{0, 0};
                    float segPenetration = 0.0f;
                    if (CircleToSegmentCollision(
                            pos, radius, poly[i], poly[next], segNormal, segPenetration))
                    {
                        // Keep the most penetrating segment — gives correct corner normals
                        if (segPenetration > penetration)
                        {
                            penetration = segPenetration;
                            normal = segNormal;
                            collided = true;
                        }
                    }
                }

                if (collided)
                {
                    // Reflection: v' = v - 2 * (v . n) * n
                    float dot = vel.x * normal.x + vel.y * normal.y;
                    if (dot < 0) // Only reflect if moving toward the surface
                    {
                        vel.x -= 2.0f * dot * normal.x;
                        vel.y -= 2.0f * dot * normal.y;
                        ball->SetVelocity(vel);
                    }

                    // Push out by exact penetration depth + small epsilon to guarantee separation
                    constexpr float kPushEpsilon = 0.5f;
                    pos.x += normal.x * (penetration + kPushEpsilon);
                    pos.y += normal.y * (penetration + kPushEpsilon);
                    ball->SetPosition(pos);

                    brick->Hit();
                }
            }
        }

        // Potential brick update logic
        for (auto& brick : mBricks)
        {
            brick->Update(game, deltaTime);
        }
    }
} // namespace nuvelocity::frs42
