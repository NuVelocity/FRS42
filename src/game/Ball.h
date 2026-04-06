#ifndef NVE_BALL_H
#define NVE_BALL_H

#include <Game.h>
#include <SDL3/SDL.h>
#include <Sequence.h>
#include <system/AssetManager.h>

namespace nuvelocity::frs42
{
    class Ball
    {
    public:
        Ball(Sequence* sequence = nullptr)
        {
            if (sequence != nullptr)
            {
                mSequence = sequence;
            }
            else
            {
                mSequence = AssetManager::LoadSequence("Resources/Ball/Ball");
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
            mVelocity = vel;
        }
        const SDL_FPoint& GetVelocity() const
        {
            return mVelocity;
        }

        float GetRadius() const
        {
            return 8.0f;
        }

        void Update(float deltaTime)
        {
            mPosition.x += mVelocity.x * deltaTime;
            mPosition.y += mVelocity.y * deltaTime;
        }

        void Draw(Game* game) const;

    private:
        Sequence* mSequence = nullptr;
        SDL_FPoint mPosition = {0.0f, 0.0f};
        SDL_FPoint mVelocity = {0.0f, 0.0f};
        uint64_t mAnimationStartTick = 0;
    };
} // namespace nuvelocity::frs42

#endif // NVE_BALL_H
