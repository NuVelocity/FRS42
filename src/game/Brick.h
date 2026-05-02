#ifndef NVE_BRICK_H
#define NVE_BRICK_H

#include "BrickType.h"
#include "Collidable2D.h"

#include <GameComponent.h>
#include <SDL3/SDL.h>

#include <Sequence.h>
namespace nuvelocity
{
    class ParticleGeneratorInfo;
} // namespace nuvelocity

namespace nuvelocity::frs42
{
    class BrickInfo;
    class Playfield;

    class Brick : public Collidable2D
    {
    public:
        Brick() = default;
        ~Brick() override = default;

        virtual void AttachBrickInfo(Game* game, const BrickInfo* info);
        void SetPlayfield(Playfield* playfield)
        {
            mPlayfield = playfield;
        }

        void SetAnimationStartTick(uint64_t tick)
        {
            mAnimationStartTick = tick;
        }

        const BrickInfo* GetInfo() const
        {
            return mInfo;
        }

        Sequence* GetSequence() const
        {
            return mSequence;
        }

        void Update(Game* game) override;
        void Draw(Game* game) override;

        SDL_FPoint GetPosition() const override
        {
            SDL_Point anchor = GetSequence() ? GetSequence()->GetAnchor() : SDL_Point{0, 0};
            return {mPosition.x + (static_cast<float>(anchor.x) / 2.0F),
                    mPosition.y + (static_cast<float>(anchor.y) / 2.0F)};
        }

        std::vector<SDL_FPoint> GetCollisionPolygon() const override;

        void OnHit(Game* game, const SDL_Rect& bounds) override;

        static bool IsIndestructibleType(BrickType type);

        bool IsDestroyed() const override
        {
            return mHitsRemaining <= 0 || mIsDestroyed;
        }

    protected:
        const BrickInfo* mInfo = nullptr;
        Sequence* mSequence = nullptr;
        uint64_t mAnimationStartTick = 0;
        bool mIsDestroyed = false; // Set to true after destruction animation finishes.
        bool mIsCompleted = false;

        int mHitsRemaining = 1;
        bool mIsPlayingDestroyedAnimation = false;
        Playfield* mPlayfield = nullptr;
    };
} // namespace nuvelocity::frs42

#endif // NVE_BRICK_H
