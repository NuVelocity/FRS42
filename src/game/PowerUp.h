#ifndef NVE_POWERUP_H
#define NVE_POWERUP_H

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
    enum class PowerUpType
    {
        Slow,
        ExpandPaddle,
        Catch,
        FireBall,
        Multiply3,
        Multiply8,
        Gun,
        BigGun,
        ExtraBall,
        RailBall,
        NormalBall,
        SmallBall,
        ShrinkPaddle,
        Fast,
        TheBomb
    };

    class PowerUp : public GameComponent
    {
    public:
        PowerUp(PowerUpType type, const SDL_FPoint& pos, Sequence* seq);
        virtual ~PowerUp() = default;

        void Update(Game* game) override;
        void Draw(Game* game) override;

        PowerUpType GetType() const
        {
            return mType;
        }
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

        static std::string GetSequencePath(PowerUpType type);
        static std::string GetSoundPath(PowerUpType type);
        static std::string GetName(PowerUpType type);
        static std::string GetDescription(PowerUpType type);
        static PowerUpType TypeFromString(const std::string& name);

    private:
        PowerUpType mType;
        SDL_FPoint mPosition;
        SDL_FPoint mVelocity;
        Sequence* mSequence;
        uint64_t mStartTick;
        bool mIsDead = false;
    };
} // namespace nuvelocity::frs42

#endif
