#ifndef NVE_BOMB_H
#define NVE_BOMB_H

#include <GameComponent.h>
#include <SDL3/SDL.h>

namespace nuvelocity
{
    class Sequence;
}

namespace nuvelocity::frs42
{
    class Bomb : public GameComponent
    {
    public:
        Bomb(const SDL_FPoint& pos, Sequence* seq);
        virtual ~Bomb() = default;

        void Update(Game* game) override;
        void Draw(Game* game) override;

        const SDL_FPoint& GetPosition() const
        {
            return mPosition;
        }
        bool IsDead() const
        {
            return mIsDead;
        }
        void SetDead(bool dead)
        {
            mIsDead = dead;
        }

    private:
        SDL_FPoint mPosition;
        SDL_FPoint mVelocity;
        Sequence* mSequence;
        uint64_t mStartTick;
        bool mIsDead = false;
    };
} // namespace nuvelocity::frs42

#endif
