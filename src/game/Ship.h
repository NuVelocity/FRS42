#ifndef NVE_SHIP_H
#define NVE_SHIP_H

#include <GameComponent.h>
#include <SDL3/SDL.h>
#include <deque>
#include <string>
#include <vector>

namespace nuvelocity
{
    class Sequence;
}

namespace nuvelocity::frs42
{
    class Playfield;

    enum class WeaponType
    {
        None,
        Laser,
        BigGun
    };

    struct ShieldInfo
    {
        std::string name;
        std::string sequencePath;
        std::vector<SDL_FPoint> collisionPolygon;
        Sequence* sequence = nullptr;
    };

    class Ship : public GameComponent
    {
    public:
        Ship();
        virtual ~Ship() = default;

        void Load(Game* game);
        void Update(Game* game) override;
        void Draw(Game* game) override;

        void SetPosition(const SDL_FPoint& position);
        const SDL_FPoint& GetPosition() const;

        void SetShipStyle(int style)
        {
            mShipStyle = style;
        }

        int GetShipStyle() const
        {
            return mShipStyle;
        }

        void SetVelocityX(float velocity)
        {
            mVelocityX = velocity;
        }

        float GetVelocityX() const
        {
            return mVelocityX;
        }

        void CycleShield(Game* game);
        const std::vector<SDL_FPoint>& GetCollisionPolygon() const;
        const std::vector<SDL_FPoint>& GetShipCollisionPolygon() const;
        SDL_FPoint GetShieldPosition() const;
        Sequence* GetBaseSequence() const
        {
            return mBaseSequence;
        }

        void SetMouseControlEnabled(bool enabled)
        {
            mMouseControlEnabled = enabled;
        }

        void SetPlayfield(Playfield* playfield)
        {
            mPlayfield = playfield;
        }

        void SetWeapon(Game* game, WeaponType type);

        void SetCatchMode(bool enabled)
        {
            mCatchMode = enabled;
        }

        bool GetCatchMode() const
        {
            return mCatchMode;
        }

        void SetShieldSize(Game* game, int sizeIndex);

        int GetShieldSize() const
        {
            return mCurrentShieldIndex;
        }

        void Explode(Game* game);
        void ImpactRecoil()
        {
            mImpactRecoilTimer = 0.15F;
        }

        SDL_FPoint GetElectricShipOffset1() const
        {
            return mElectricShipOffset1;
        }
        SDL_FPoint GetElectricShipOffset2() const
        {
            return mElectricShipOffset2;
        }
        SDL_FPoint GetElectricShieldOffset1() const
        {
            return mElectricShieldOffset1;
        }
        SDL_FPoint GetElectricShieldOffset2() const
        {
            return mElectricShieldOffset2;
        }

        void DrawElectricLine(Game* game,
                              SDL_FPoint p1,
                              SDL_FPoint p2,
                              SDL_Color color = {.r = 0x88, .g = 0x88, .b = 0xF0, .a = 255});

        static const std::vector<std::string>& GetShipSequencePaths();

    private:
        Sequence* mBaseSequence = nullptr;
        Sequence* mShieldSequence = nullptr;
        Sequence* mThrustLeftSequence = nullptr;
        Sequence* mThrustRightSequence = nullptr;

        SDL_FPoint mPosition = {.x = 0.0F, .y = 0.0F};
        float mPreviousX = 0.0F;
        float mVelocityX = 0.0F;
        float mThrustPowerLeft = 0.0F;
        float mThrustPowerRight = 0.0F;
        uint64_t mStartTick = 0;
        int mShipStyle = 0;

        std::vector<ShieldInfo> mAvailableShields;
        int mCurrentShieldIndex = 2; // Default to Normal Shield
        bool mMouseControlEnabled = true;

        WeaponType mWeaponType = WeaponType::None;
        bool mCatchMode = false;
        bool mLastShotSideLeft = false; // Alternating

        Sequence* mWeaponSequence = nullptr;
        Sequence* mRecoilLeftSequence = nullptr;
        Sequence* mRecoilRightSequence = nullptr;
        float mLeftRecoilTimer = 0.0F;
        float mRightRecoilTimer = 0.0F;
        Sequence* mExplodeSequence = nullptr;
        Sequence* mProjectileSequence = nullptr;

        Playfield* mPlayfield = nullptr;
        bool mIsExploded = false;
        float mImpactRecoilTimer = 0.0F;
        float mLeftThrustTimer = 0.0F;
        float mRightThrustTimer = 0.0F;

        float mTargetY = 0.0F;
        struct PositionSample
        {
            uint64_t timestamp;
            SDL_FPoint position;
        };
        std::deque<PositionSample> mPositionHistory;

        // Electric line attachment offsets
        SDL_FPoint mElectricShipOffset1 = {-11.0F, -8.0F};
        SDL_FPoint mElectricShieldOffset1 = {-15.0F, -15.0F};
        SDL_FPoint mElectricShipOffset2 = {11.0F, -8.0F};
        SDL_FPoint mElectricShieldOffset2 = {15.0F, -15.0F};

        uint64_t mShieldDelayMs = 40;
        std::vector<SDL_FPoint> mShipCollisionPolygon;
    };
} // namespace nuvelocity::frs42

#endif // NVE_SHIP_H
