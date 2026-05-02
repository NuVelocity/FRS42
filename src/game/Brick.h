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
        void UpdateBrickDestroyed();
        void UpdateBrickMovement(nuvelocity::Game* game);
        void Draw(Game* game) override;

        SDL_FPoint GetPosition() const override
        {
            SDL_Point anchor = GetSequence() ? GetSequence()->GetAnchor() : SDL_Point{0, 0};
            return {mPosition.x + (static_cast<float>(anchor.x) / 2.0F),
                    mPosition.y + (static_cast<float>(anchor.y) / 2.0F)};
        }

        std::vector<SDL_FPoint> GetCollisionPolygon() const override;

        void OnHit(Game* game,
                   const SDL_Rect& bounds,
                   bool hitFromTop = false,
                   bool ignoreDirection = true) override;
        void OnDestroy(Game* game, const SDL_Rect& bounds);

        static bool IsIndestructibleType(BrickType type);

        bool IsDestroyed() const override
        {
            return mHitsRemaining <= 0 || mIsDestroyed;
        }

        void SetInitialPosition(const SDL_FPoint& pos)
        {
            mInitialPosition = pos;
            mPosition = pos;
        }

        void SetMovement(float speed, int range1, int range2, int direction)
        {
            mSpeed = speed;
            mRange1 = range1;
            mRange2 = range2;
            mDirection = direction;
        }

        void SetChangeBrickSequences(const std::string& from,
                                     const std::string& to,
                                     Sequence* fromSeq,
                                     Sequence* toSeq)
        {
            mBrickToChangeFrom = from;
            mBrickToChangeTo = to;
            mChangeFromSequence = fromSeq;
            mChangeToSequence = toSeq;
        }

        void SetForcePowerUp(const std::string& powerUp)
        {
            mForcePowerUp = powerUp;
        }

        void SetBrickToLookLike(const std::string& path)
        {
            mBrickToLookLike = path;
        }

        const std::string& GetBrickInfoPath() const
        {
            return mBrickInfoPath;
        }

        void SetBrickInfoPath(const std::string& path)
        {
            mBrickInfoPath = path;
        }

        void SetCanMoveThroughOtherBricks(bool canMove)
        {
            mCanMoveThroughOtherBricks = canMove;
        }

        bool IsChangeBrick() const
        {
            return mChangeFromSequence != nullptr && mChangeToSequence != nullptr &&
                   mBrickToChangeFrom != "Bricks/!None" && mBrickToChangeTo != "Bricks/!None";
        }

    protected:
        const BrickInfo* mInfo = nullptr;
        Sequence* mSequence = nullptr;
        Sequence* mHitSequence = nullptr;
        uint64_t mAnimationStartTick = 0;
        bool mIsDestroyed = false; // Set to true after destruction animation finishes.
        bool mIsCompleted = false;

        int mHitsRemaining = 1;
        bool mIsPlayingDestroyedAnimation = false;
        Sequence* mDestroyedSequence = nullptr;
        Playfield* mPlayfield = nullptr;

        // Movement
        float mSpeed = 0.0F;
        int mRange1 = 0;
        int mRange2 = 0;
        int mDirection = 0;
        SDL_FPoint mInitialPosition = {0.0F, 0.0F};
        int mMoveTarget = 1;

        // Change Brick sequences
        std::string mBrickToChangeFrom = "Bricks/!None";
        std::string mBrickToChangeTo = "Bricks/!None";
        Sequence* mChangeFromSequence = nullptr;
        Sequence* mChangeToSequence = nullptr;

        std::string mBrickToLookLike = "Bricks/!None";
        std::string mBrickInfoPath = "Bricks/!None";

        // Power-up
        std::string mForcePowerUp = "No Power-Up";

        bool mCanMoveThroughOtherBricks = true;

    private:
        static SDL_FRect GetCollidableBounds(Collidable2D* c);
    };
} // namespace nuvelocity::frs42

#endif // NVE_BRICK_H
