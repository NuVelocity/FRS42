#ifndef NVE_BRICK_H
#define NVE_BRICK_H

#include "BrickInfo.h"
#include <Game.h>
#include <SDL3/SDL.h>
#include <Sequence.h>
#include <system/AssetManager.h>

namespace nuvelocity::frs42
{
    class Brick
    {
    public:
        Brick() = default;
        virtual ~Brick() = default;

        virtual void AttachBrickInfo(Game* game, const BrickInfo& info)
        {
            mInfo = info;
            mSequence = game->mAsset->LoadSequence(mInfo.GetPrimarySequencePath());
            mAnimationStartTick = SDL_GetTicks();
            mIsDestroyed = false;
        }

        const BrickInfo& GetInfo() const
        {
            return mInfo;
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

        bool IsDestroyed() const
        {
            return mIsDestroyed;
        }

        virtual void Hit()
        {
            if (mInfo.GetBrickType() == BrickType::Normal)
            {
                mIsDestroyed = true;
            }
            // Indestructible does nothing
        }

        virtual void Update(Game* game, float deltaTime) {}
        virtual void Draw(Game* game) const;

        virtual std::vector<SDL_FPoint> GetCollisionPolygon() const;
        virtual bool IsClosedPolygon() const
        {
            return true;
        }

    protected:
        BrickInfo mInfo;
        Sequence* mSequence = nullptr;
        SDL_FPoint mPosition = {0.0f, 0.0f};
        uint64_t mAnimationStartTick = 0;
        bool mIsDestroyed = false;
    };
} // namespace nuvelocity::frs42

#endif // NVE_BRICK_H
