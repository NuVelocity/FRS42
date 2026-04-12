#ifndef NVE_BALL_H
#define NVE_BALL_H

#include <Game.h>
#include <GameComponent.h>
#include <SDL3/SDL.h>
#include <Sequence.h>
#include <cmath>
#include <system/AssetManager.h>

namespace nuvelocity::frs42
{
    class Ball : public GameComponent
    {
    public:
        Ball() = default;

        void AttachSequence(Game* game, Sequence* sequence = nullptr)
        {
            if (sequence != nullptr)
            {
                mSequence = sequence;
            }
            else
            {
                mSequence = game->mAsset->LoadSequence("Resources/Ball/Ball");
            }
            mAnimationStartTick = SDL_GetTicks();
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
            return 8.0f;
        }

        void Update(Game* aGame) override {};
        void Draw(Game* aGame) override;

    private:
        Sequence* mSequence = nullptr;
        SDL_FPoint mPosition = {0.0f, 0.0f};
        SDL_FPoint mDirection = {0.0f, 0.0f};
        float mSpeed = 0.0f;
        uint64_t mAnimationStartTick = 0;
    };
} // namespace nuvelocity::frs42

#endif // NVE_BALL_H
