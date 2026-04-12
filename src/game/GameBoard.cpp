#include "GameBoard.h"
#include "Ball.h"
#include "Brick.h"
#include "MathUtils.h"
#include <Game.h>
#include <algorithm>
#include <cmath>

namespace nuvelocity::frs42
{
    GameBoard::GameBoard() = default;
    GameBoard::~GameBoard() = default;

    void GameBoard::Draw(Game* aGame)
    {
        for (const auto& brick : mBricks)
        {
            brick->Draw(aGame);
        }
        for (const auto& ball : mBalls)
        {
            ball->Draw(aGame);
        }

        if (aGame != nullptr && aGame->mArgs.get<bool>("--debug-collisions") &&
            aGame->mSpriteBatch != nullptr)
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
                    aGame->mSpriteBatch->DrawLine(
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
                    aGame->mSpriteBatch->DrawLine(center.x + r * SDL_cosf(a0),
                                                  center.y + r * SDL_sinf(a0),
                                                  center.x + r * SDL_cosf(a1),
                                                  center.y + r * SDL_sinf(a1),
                                                  kBallWireColor);
                }
            }
        }
    }

    void GameBoard::Update(Game* aGame)
    {
        const float deltaTime = aGame->GetDeltaTime();

        for (auto& ball : mBalls)
        {
            const float radius = ball->GetRadius();

            // Key: adaptive sub-stepping based on speed and radius
            // Ensures we never move more than half a radius per step
            const float speed = ball->GetSpeed();
            const float maxStepDist =
                radius * 0.5f; // Conservative: move max 0.5 * radius per substep
            const float moveDist = speed * deltaTime;

            int numSteps = std::max(1, static_cast<int>(std::ceil(moveDist / maxStepDist)));
            const float dt = deltaTime / static_cast<float>(numSteps);

            for (int step = 0; step < numSteps; ++step)
            {
                SDL_FPoint pos = ball->GetPosition();
                SDL_FPoint vel = ball->GetVelocity();

                // Move by velocity * dt
                pos.x += vel.x * dt;
                pos.y += vel.y * dt;
                ball->SetPosition(pos);

                // Screen boundary checks (Bounce)
                if (pos.x - radius < 0)
                {
                    pos.x = radius;
                    vel.x = std::abs(vel.x);
                    ball->SetVelocity(vel);
                    ball->SetPosition(pos);
                }
                else if (pos.x + radius > static_cast<float>(aGame->mWindowWidth))
                {
                    pos.x = static_cast<float>(aGame->mWindowWidth) - radius;
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
                else if (pos.y + radius > static_cast<float>(aGame->mWindowHeight))
                {
                    pos.y = static_cast<float>(aGame->mWindowHeight) - radius;
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
                        if (MathUtils::CircleToSegmentCollision(
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

                        // Push out by exact penetration depth + small epsilon to guarantee
                        // separation
                        constexpr float kPushEpsilon = 0.5f;
                        pos.x += normal.x * (penetration + kPushEpsilon);
                        pos.y += normal.y * (penetration + kPushEpsilon);
                        ball->SetPosition(pos);

                        brick->Hit();
                    }
                }
            } // End of sub-step loop
        }

        // Potential brick update logic
        for (auto& brick : mBricks)
        {
            brick->Update(aGame);
        }
    }
} // namespace nuvelocity::frs42
