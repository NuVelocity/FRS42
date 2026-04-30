#ifndef NVE_SHIP_H
#define NVE_SHIP_H

#include <GameComponent.h>
#include <SDL3/SDL.h>
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

        void CycleShield(Game* game);
        const std::vector<SDL_FPoint>& GetCollisionPolygon() const;
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
    };
} // namespace nuvelocity::frs42

#endif // NVE_SHIP_H
