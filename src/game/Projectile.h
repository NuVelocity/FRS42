#ifndef NVE_PROJECTILE_H
#define NVE_PROJECTILE_H

#include <GameComponent.h>
#include <SDL3/SDL.h>

namespace nuvelocity
{
    class Sequence;
}

namespace nuvelocity::frs42
{
    enum class ProjectileType
    {
        Laser,
        Missile
    };

    class Projectile : public GameComponent
    {
    public:
        Projectile(ProjectileType type, const SDL_FPoint& pos, Sequence* seq);
        virtual ~Projectile() = default;

        void Update(Game* game) override;
        void Draw(Game* game) override;

        const SDL_FPoint& GetPosition() const
        {
            return mPosition;
        }
        ProjectileType GetType() const
        {
            return mType;
        }
        bool IsDead() const
        {
            return mIsDead || mPosition.y < -50.0F;
        }

        void Die()
        {
            mIsDead = true;
        }

    private:
        ProjectileType mType;
        SDL_FPoint mPosition;
        SDL_FPoint mVelocity;
        Sequence* mSequence;
        bool mIsDead = false;
    };
} // namespace nuvelocity::frs42

#endif
