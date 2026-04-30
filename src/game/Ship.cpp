#include "Ship.h"
#include "Colors.h"
#include "Playfield.h"
#include "Projectile.h"
#include <Game.h>
#include <Sequence.h>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <numbers>
#include <system/AssetManager.h>
#include <system/InputManager.h>
#include <system/SpriteBatch.h>
#include <utility>

namespace nuvelocity::frs42
{
    Ship::Ship()
    {
        mStartTick = SDL_GetTicks();
    }

    void Ship::Load(Game* game)
    {
        mBaseSequence = game->mAsset->LoadSequence("Resources/Player Ship/RS Ship");
        mExplodeSequence = game->mAsset->LoadSequence("Resources/Player Ship/RS Shipexplode");
        mThrustLeftSequence = game->mAsset->LoadSequence("Resources/Player Ship/Thrust Left");
        mThrustRightSequence = game->mAsset->LoadSequence("Resources/Player Ship/Thrust Right");

        mAvailableShields = {
            {.name = "Small Shield",
             .sequencePath = "Resources/Player Ship/RS Small Shield",
             .collisionPolygon = {{.x = -22, .y = -31}, {.x = -20, .y = -33}, {.x = 0, .y = -34},
                                  {.x = 22, .y = -33},  {.x = 24, .y = -30},  {.x = 18, .y = -26},
                                  {.x = 16, .y = -20},  {.x = 10, .y = -11},  {.x = 11, .y = -3},
                                  {.x = 11, .y = 6},    {.x = 15, .y = 17},   {.x = 15, .y = 17},
                                  {.x = 15, .y = 29},   {.x = 6, .y = 33},    {.x = -8, .y = 33},
                                  {.x = -16, .y = 27},  {.x = -15, .y = 13},  {.x = -9, .y = 8},
                                  {.x = -9, .y = -7},   {.x = -18, .y = -21}, {.x = -20, .y = -28},
                                  {.x = -22, .y = -31}}},
            {.name = "Small Shield 2",
             .sequencePath = "Resources/Player Ship/RS Small Shield 2",
             .collisionPolygon = {{.x = -31, .y = -30}, {.x = -26, .y = -33}, {.x = 0, .y = -34},
                                  {.x = 28, .y = -33},  {.x = 31, .y = -30},  {.x = 26, .y = -25},
                                  {.x = 24, .y = -18},  {.x = 10, .y = -11},  {.x = 11, .y = -3},
                                  {.x = 11, .y = 6},    {.x = 15, .y = 17},   {.x = 15, .y = 17},
                                  {.x = 15, .y = 29},   {.x = 6, .y = 33},    {.x = -8, .y = 33},
                                  {.x = -16, .y = 27},  {.x = -15, .y = 13},  {.x = -9, .y = 8},
                                  {.x = -9, .y = -7},   {.x = -24, .y = -21}, {.x = -26, .y = -26},
                                  {.x = -31, .y = -30}}},
            {.name = "Normal Shield",
             .sequencePath = "Resources/Player Ship/RS Normal Shield",
             .collisionPolygon = {{.x = -45, .y = -15}, {.x = -31, .y = -27}, {.x = -8, .y = -33},
                                  {.x = 16, .y = -31},  {.x = 32, .y = -26},  {.x = 42, .y = -17},
                                  {.x = 43, .y = -12},  {.x = 40, .y = -9},   {.x = 24, .y = -19},
                                  {.x = 17, .y = -18},  {.x = 10, .y = -6},   {.x = 11, .y = 6},
                                  {.x = 15, .y = 17},   {.x = 15, .y = 17},   {.x = 15, .y = 29},
                                  {.x = 6, .y = 33},    {.x = -8, .y = 33},   {.x = -16, .y = 27},
                                  {.x = -15, .y = 13},  {.x = -9, .y = 8},    {.x = -9, .y = -7},
                                  {.x = -19, .y = -18}, {.x = -30, .y = -16}, {.x = -40, .y = -11},
                                  {.x = -44, .y = -12}, {.x = -45, .y = -15}}},
            {.name = "Long Shield",
             .sequencePath = "Resources/Player Ship/RS Long Shield",
             .collisionPolygon = {{.x = -51, .y = -15}, {.x = -32, .y = -29}, {.x = -1, .y = -33},
                                  {.x = 23, .y = -32},  {.x = 39, .y = -27},  {.x = 52, .y = -15},
                                  {.x = 46, .y = -11},  {.x = 34, .y = -17},  {.x = 24, .y = -19},
                                  {.x = 17, .y = -18},  {.x = 10, .y = -6},   {.x = 11, .y = 6},
                                  {.x = 15, .y = 17},   {.x = 15, .y = 17},   {.x = 15, .y = 29},
                                  {.x = 6, .y = 33},    {.x = -8, .y = 33},   {.x = -16, .y = 27},
                                  {.x = -15, .y = 13},  {.x = -9, .y = 8},    {.x = -9, .y = -7},
                                  {.x = -23, .y = -20}, {.x = -36, .y = -16}, {.x = -42, .y = -12},
                                  {.x = -46, .y = -10}, {.x = -51, .y = -15}}},
            {.name = "Biggest Shield",
             .sequencePath = "Resources/Player Ship/RS Biggest Shield",
             .collisionPolygon = {{.x = -63, .y = -16}, {.x = -41, .y = -30}, {.x = -1, .y = -33},
                                  {.x = 33, .y = -31},  {.x = 46, .y = -27},  {.x = 61, .y = -17},
                                  {.x = 57, .y = -12},  {.x = 40, .y = -19},  {.x = 24, .y = -19},
                                  {.x = 17, .y = -18},  {.x = 10, .y = -6},   {.x = 11, .y = 6},
                                  {.x = 15, .y = 17},   {.x = 15, .y = 17},   {.x = 15, .y = 29},
                                  {.x = 6, .y = 33},    {.x = -8, .y = 33},   {.x = -16, .y = 27},
                                  {.x = -15, .y = 13},  {.x = -9, .y = 8},    {.x = -9, .y = -7},
                                  {.x = -23, .y = -20}, {.x = -40, .y = -19}, {.x = -54, .y = -14},
                                  {.x = -60, .y = -12}, {.x = -63, .y = -16}}}};

        for (auto& shield : mAvailableShields)
        {
            shield.sequence = game->mAsset->LoadSequence(shield.sequencePath);
        }

        mShieldSequence = mAvailableShields[mCurrentShieldIndex].sequence;
    }

    void Ship::SetWeapon(Game* game, WeaponType type)
    {
        mWeaponType = type;
        if (type == WeaponType::Laser)
        {
            mWeaponSequence = game->mAsset->LoadSequence("Resources/Player Ship/RS Small Guns");
            mRecoilLeftSequence =
                game->mAsset->LoadSequence("Resources/Player Ship/RS Small Guns Recoil Left");
            mRecoilRightSequence =
                game->mAsset->LoadSequence("Resources/Player Ship/RS Small Guns Recoil Right");
            mProjectileSequence = game->mAsset->LoadSequence("Resources/Player Ship/Player Shot");
        }
        else if (type == WeaponType::BigGun)
        {
            mWeaponSequence = game->mAsset->LoadSequence("Resources/Player Ship/RS Big Guns");
            mRecoilLeftSequence =
                game->mAsset->LoadSequence("Resources/Player Ship/RS Big Guns Recoil Left");
            mRecoilRightSequence =
                game->mAsset->LoadSequence("Resources/Player Ship/RS Big Guns Recoil Right");
            mProjectileSequence = game->mAsset->LoadSequence("Resources/Player Ship/Big Shot");
        }
        else
        {
            mWeaponSequence = nullptr;
            mRecoilLeftSequence = nullptr;
            mRecoilRightSequence = nullptr;
            mProjectileSequence = nullptr;
        }
    }

    void Ship::SetShieldSize(Game* game, int sizeIndex)
    {
        (void)game;
        mCurrentShieldIndex =
            std::clamp(sizeIndex, 0, static_cast<int>(mAvailableShields.size()) - 1);
        mShieldSequence = mAvailableShields[mCurrentShieldIndex].sequence;
    }

    void Ship::Explode(Game* game)
    {
        (void)game;
        mIsExploded = true;
        mStartTick = SDL_GetTicks();
    }

    void Ship::Update(Game* game)
    {
        if (game->mInput == nullptr || mIsExploded)
        {
            return;
        }

        SDL_Point mousePos = game->mInput->GetMousePosition();
        float dt = game->GetDeltaTime();
        if (mMouseControlEnabled)
        {
            mPosition.x = static_cast<float>(mousePos.x);
        }

        float targetVelocity = mPosition.x - mPreviousX;
        const float lerpFactor = 0.15F;
        mVelocityX = mVelocityX + ((targetVelocity - mVelocityX) * lerpFactor);
        mPreviousX = mPosition.x;

        // Thruster timers
        if (mLeftThrustTimer > 0)
        {
            mLeftThrustTimer -= dt;
        }
        if (mRightThrustTimer > 0)
        {
            mRightThrustTimer -= dt;
        }

        if (mVelocityX > 0.1F)
        {
            mLeftThrustTimer = 0.5F;
        }
        else if (mVelocityX < -0.1F)
        {
            mRightThrustTimer = 0.5F;
        }

        // Weapon logic
        if (mWeaponType != WeaponType::None)
        {
            if (mLeftRecoilTimer > 0)
            {
                mLeftRecoilTimer -= dt;
            }
            if (mRightRecoilTimer > 0)
            {
                mRightRecoilTimer -= dt;
            }

            if (game->mInput->IsMouseButtonPressed(SDL_BUTTON_LEFT))
            {
                mLastShotSideLeft = !mLastShotSideLeft;
                if (mLastShotSideLeft)
                {
                    mLeftRecoilTimer = 0.2F;
                }
                else
                {
                    mRightRecoilTimer = 0.2F;
                }

                if (mPlayfield != nullptr)
                {
                    float xOff = mLastShotSideLeft ? -32.0F : 32.0F;
                    auto proj = std::make_unique<Projectile>(
                        (mWeaponType == WeaponType::Laser ? ProjectileType::Laser
                                                          : ProjectileType::Missile),
                        SDL_FPoint{mPosition.x + xOff, mPosition.y - 20.0F},
                        mProjectileSequence);
                    mPlayfield->AddProjectile(std::move(proj));
                }
            }
        }
        // Impact recoil timer
        if (mImpactRecoilTimer > 0)
        {
            mImpactRecoilTimer -= dt;
        }
    }

    void Ship::Draw(Game* game)
    {
        if (game == nullptr || game->mSpriteBatch == nullptr)
        {
            return;
        }

        auto drawFrame =
            [&](Sequence* seq, SDL_FPoint pos, int index, SDL_Color color = Colors::White)
        {
            if (seq == nullptr)
            {
                return;
            }
            if (index < 0 || static_cast<std::size_t>(index) >= seq->GetFrameCount())
            {
                return;
            }

            Frame* frame = seq->GetFrame(index);
            if (frame != nullptr)
            {
                SDL_Surface* surface = frame->GetSurface();
                SDL_Rect destRect{.x = static_cast<int>(std::lround(pos.x)) + frame->GetHotSpot().x,
                                  .y = static_cast<int>(std::lround(pos.y)) + frame->GetHotSpot().y,
                                  .w = surface->w,
                                  .h = surface->h};
                game->mSpriteBatch->Draw(surface, &destRect, nullptr, color);
            }
        };

        auto drawAnimated = [&](Sequence* seq, SDL_FPoint pos)
        {
            if (seq == nullptr)
            {
                return;
            }
            const std::size_t frameCount = seq->GetFrameCount();
            if (frameCount == 0)
            {
                return;
            }

            const uint64_t now = SDL_GetTicks();
            const uint64_t elapsed = now - mStartTick;
            const float fps = seq->GetFramesPerSecond();
            const std::size_t frameIndex =
                static_cast<std::size_t>((static_cast<double>(elapsed) * fps) / 1000.0) %
                frameCount;

            drawFrame(seq, pos, static_cast<int>(frameIndex));
        };

        auto drawAnimatedTilt = [&](Sequence* seq,
                                    SDL_FPoint pos,
                                    int tiltIdx,
                                    SDL_Color color = {.r = 255, .g = 255, .b = 255, .a = 255})
        {
            if (seq == nullptr || seq->GetFrameCount() == 0)
            {
                return;
            }

            const uint64_t now = SDL_GetTicks();
            const uint64_t elapsed = now - mStartTick;
            const float fps = seq->GetFramesPerSecond();
            const std::size_t totalFrames = seq->GetFrameCount();
            const int tiltStates = 21;
            const std::size_t numAnims = totalFrames / tiltStates;

            if (numAnims == 0)
            {
                if (totalFrames > static_cast<std::size_t>(tiltIdx))
                {
                    drawFrame(seq, pos, tiltIdx, color);
                }
                return;
            }

            const std::size_t animIdx =
                static_cast<std::size_t>((static_cast<double>(elapsed) * fps) / 1000.0) % numAnims;

            drawFrame(seq, pos, static_cast<int>(animIdx * tiltStates + tiltIdx), color);
        };

        float recoilY = 0;
        if (mImpactRecoilTimer > 0)
        {
            // 2px down-up animation over 0.15s
            if (mImpactRecoilTimer > 0.075F)
            {
                recoilY = (0.15F - mImpactRecoilTimer) / 0.075F * 2.0F;
            }
            else
            {
                recoilY = mImpactRecoilTimer / 0.075F * 2.0F;
            }
        }
        SDL_FPoint drawPos = {mPosition.x, mPosition.y + recoilY};

        if (mIsExploded)
        {
            drawAnimated(mExplodeSequence, drawPos);
            return;
        }

        // Tilt computation
        int tiltFrameIndex = 10;
        if (mBaseSequence != nullptr && mBaseSequence->GetFrameCount() >= 21)
        {
            float speed = std::abs(mVelocityX);
            int tiltAmount = static_cast<int>(speed * 2.5F); // Adjust sensitivity
            tiltAmount = std::min(tiltAmount, 10);
            if (tiltAmount < 1)
            {
                tiltAmount = 0;
            }

            if (mVelocityX > 0.1F)
            {
                tiltFrameIndex = 10 - tiltAmount;
            } // 0-9
            else if (mVelocityX < -0.1F)
            {
                tiltFrameIndex = 10 + tiltAmount;
            } // 11-20
        }

        const int thrusterXOffset = 23;
        const int thrusterYOffset = 22;

        // Thruster particles
        if (std::abs(mVelocityX) > 0.1F && mPlayfield != nullptr)
        {
            float sideOffset =
                (mVelocityX < 0) ? thrusterXOffset : -thrusterXOffset; // Left move -> spawn right
            float angle = (mVelocityX < 0) ? 0.0F : std::numbers::pi_v<float>;
            SDL_FPoint thrusterPos = {.x = mPosition.x + sideOffset,
                                      .y = mPosition.y + thrusterYOffset};
            mPlayfield->SpawnParticleBurst(
                game, "Particle Generators/Ship Thruster", thrusterPos, angle, 3.0F, 2.0F);
        }

        // Thrusters
        if (mLeftThrustTimer > 0.0F && mThrustLeftSequence != nullptr)
        {
            SDL_FPoint thrusterPos = {.x = mPosition.x, .y = mPosition.y + 12};
            drawAnimated(mThrustLeftSequence, thrusterPos);
        }
        if (mRightThrustTimer > 0.0F && mThrustRightSequence != nullptr)
        {
            SDL_FPoint thrusterPos = {.x = mPosition.x, .y = mPosition.y + 12};
            drawAnimated(mThrustRightSequence, thrusterPos);
        }

        // Draw base ship
        drawAnimatedTilt(mBaseSequence, mPosition, tiltFrameIndex);

        // Weapon
        if (mWeaponType != WeaponType::None)
        {
            float leftRecoilY = 0.0F;
            if (mLeftRecoilTimer > 0.0F)
            {
                if (mLeftRecoilTimer > 0.1F)
                {
                    leftRecoilY = (0.2F - mLeftRecoilTimer) / 0.1F * 2.0F;
                }
                else
                {
                    leftRecoilY = mLeftRecoilTimer / 0.1F * 2.0F;
                }
            }
            drawAnimatedTilt(
                mRecoilLeftSequence, {mPosition.x, mPosition.y + leftRecoilY}, tiltFrameIndex);

            float rightRecoilY = 0.0F;
            if (mRightRecoilTimer > 0.0F)
            {
                if (mRightRecoilTimer > 0.1F)
                {
                    rightRecoilY = (0.2F - mRightRecoilTimer) / 0.1F * 2.0F;
                }
                else
                {
                    rightRecoilY = mRightRecoilTimer / 0.1F * 2.0F;
                }
            }
            drawAnimatedTilt(
                mRecoilRightSequence, {mPosition.x, mPosition.y + rightRecoilY}, tiltFrameIndex);

            drawAnimatedTilt(mWeaponSequence, mPosition, tiltFrameIndex);
        }

        // Shield remains time-animated with recoil
        drawAnimated(mShieldSequence, drawPos);

        // Draw debug polygon
        if (game->mSpriteBatch->IsDrawBoundsEnabled())
        {
            const auto& poly = GetCollisionPolygon();
            for (size_t i = 0; i < poly.size(); ++i)
            {
                size_t next = (i + 1) % poly.size();
                game->mSpriteBatch->DrawLine(
                    static_cast<int>(std::lround(mPosition.x + poly[i].x)),
                    static_cast<int>(std::lround(mPosition.y + poly[i].y)),
                    static_cast<int>(std::lround(mPosition.x + poly[next].x)),
                    static_cast<int>(std::lround(mPosition.y + poly[next].y)),
                    Colors::Green);
            }
        }
    }

    void Ship::SetPosition(const SDL_FPoint& position)
    {
        mPosition = position;
    }

    const SDL_FPoint& Ship::GetPosition() const
    {
        return mPosition;
    }

    void Ship::CycleShield(Game* game)
    {
        if (mAvailableShields.empty())
        {
            return;
        }
        mCurrentShieldIndex =
            (mCurrentShieldIndex + 1) % static_cast<int>(mAvailableShields.size());
        mShieldSequence = mAvailableShields[mCurrentShieldIndex].sequence;
    }

    const std::vector<SDL_FPoint>& Ship::GetCollisionPolygon() const
    {
        static const std::vector<SDL_FPoint> empty;
        if (mCurrentShieldIndex < 0 ||
            static_cast<size_t>(mCurrentShieldIndex) >= mAvailableShields.size())
        {
            return empty;
        }
        return mAvailableShields[mCurrentShieldIndex].collisionPolygon;
    }
} // namespace nuvelocity::frs42
