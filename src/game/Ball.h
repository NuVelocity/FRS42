#ifndef NVE_BALL_H
#define NVE_BALL_H

#include <GameComponent.h>
#include <SDL3/SDL.h>
#include <cmath>

namespace nuvelocity
{
    class Sequence;
}

namespace nuvelocity::frs42
{
    class Playfield;

    enum class BallType
    {
        Normal,
        Fire,
        Rail
    };

    class Ball : public GameComponent
    {
    public:
        Ball();

        void AttachSequence(Game* game, Sequence* sequence = nullptr);

        void SetPlayfield(Playfield* playfield)
        {
            mPlayfield = playfield;
        }

        void SetSequence(Sequence* sequence)
        {
            mSequence = sequence;
            mAnimationStartTick = SDL_GetTicks();
        }
        Sequence* GetSequence() const
        {
            return mSequence;
        }

        void SetPosition(const SDL_FPoint& pos)
        {
            mPosition = pos;
        }
        const SDL_FPoint& GetPosition() const
        {
            return mPosition;
        }

        void SetVelocity(const SDL_FPoint& vel)
        {
            const float len = std::sqrt(vel.x * vel.x + vel.y * vel.y);
            if (len > 0.0001f)
            {
                mDirection = {vel.x / len, vel.y / len};
                mSpeed = len;
            }
            else
            {
                mDirection = {0.0f, 0.0f};
                mSpeed = 0.0f;
            }
        }
        SDL_FPoint GetVelocity() const
        {
            return {mDirection.x * mSpeed, mDirection.y * mSpeed};
        }

        void SetSpeed(float speed)
        {
            mSpeed = speed;
        }
        float GetSpeed() const
        {
            return mSpeed;
        }

        void SetDirection(const SDL_FPoint& direction)
        {
            const float len = std::sqrt(direction.x * direction.x + direction.y * direction.y);
            if (len > 0.0001f)
            {
                mDirection = {direction.x / len, direction.y / len};
            }
            else
            {
                mDirection = {0.0f, 0.0f};
            }
        }
        const SDL_FPoint& GetDirection() const
        {
            return mDirection;
        }

        float GetRadius() const
        {
            return mIsSmall ? 4.0F : 8.0F;
        }

        void SetIsAttached(bool attached)
        {
            mIsAttached = attached;
        }

        bool IsAttached() const
        {
            return mIsAttached;
        }

        void SetIsSmall(bool small);

        bool IsSmall() const
        {
            return mIsSmall;
        }

        void SetType(BallType type);
        BallType GetType() const
        {
            return mType;
        }

        void SetIsTrapped(bool trapped)
        {
            if (mIsTrapped != trapped)
            {
                mIsTrapped = trapped;
                UpdateSequence(nullptr);
            }
        }
        bool IsTrapped() const
        {
            return mIsTrapped;
        }

        void SetTrappedSequence(Sequence* sequence)
        {
            mTrappedSequence = sequence;
            UpdateSequence(nullptr);
        }

        void SpeedUp();

        void SetLastHitPosition(const SDL_FPoint& pos)
        {
            mLastHitPosition = pos;
        }
        const SDL_FPoint& GetLastHitPosition() const
        {
            return mLastHitPosition;
        }

        void Update(Game* game) override;
        void Draw(Game* game) override;

    private:
        void UpdateSequence(Game* game);

        Sequence* mSequence = nullptr;
        Sequence* mNormalSequence = nullptr;
        Sequence* mSmallSequence = nullptr;
        Sequence* mFireSequence = nullptr;
        Sequence* mRailSequence = nullptr;
        Sequence* mTrappedSequence = nullptr;

        SDL_FPoint mPosition = {0.0f, 0.0f};
        SDL_FPoint mDirection = {0.0f, 0.0f};
        SDL_FPoint mLastHitPosition = {0.0f, 0.0f};
        float mSpeed = 79.0f;
        uint64_t mAnimationStartTick = 0;
        bool mIsAttached = false;
        bool mIsSmall = false;
        bool mIsTrapped = false;
        BallType mType = BallType::Normal;

        float mTrailTimer = 0.0F;
        Playfield* mPlayfield = nullptr;
    };
} // namespace nuvelocity::frs42

#endif // NVE_BALL_H
