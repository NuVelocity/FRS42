#ifndef NVE_GAME_BOARD_H
#define NVE_GAME_BOARD_H

#include "Ball.h"
#include "Bomb.h"
#include "Collidable2D.h"
#include "GameStats.h"
#include "PowerUp.h"
#include "Projectile.h"
#include <GameComponent.h>
#include <SDL3/SDL.h>
#include <filesystem>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace nuvelocity
{
    class Sequence;
    class StandAloneFrame;
    class ParticleGeneratorInfo;
    class ParticleGenerator;
    class ParticleType;
} // namespace nuvelocity

namespace nuvelocity::frs42
{
    class BackgroundDefinition;
    class BackgroundSpriteAI;
    class Megovision;
    class Ship;
} // namespace nuvelocity::frs42

namespace nuvelocity
{
    class Game;
}

namespace nuvelocity::frs42
{
    class BackgroundSprite
    {
    public:
        SDL_FPoint mPosition = {0, 0};
        SDL_FPoint mVelocity = {0, 0};
        Sequence* mSequence = nullptr;
        BackgroundSpriteAI* mAI = nullptr;
        uint64_t mStartTick = 0;
        bool mFlipX = false;

        void Update(float deltaTime, int windowWidth, int windowHeight);
        void
        Draw(Game* game, bool drawShadow = false, const SDL_FPoint& shadowOffset = {0, 0}) const;
    };

    class Playfield : public GameComponent
    {
    public:
        enum BoundaryFlags : uint8_t
        {
            None = 0,
            Left = 1 << 0,
            Top = 1 << 1,
            Right = 1 << 2,
            Bottom = 1 << 3,
            All = Left | Top | Right | Bottom
        };

        Playfield();
        virtual ~Playfield();

        void Update(Game* game) override;

        void Draw(Game* game) override;

        void Reset(Game* game);

        void LoadBackground(Game* game, const std::string& path);
        void SetRoundName(const std::string& name)
        {
            mRoundName = name;
            mRoundNameTimer = 3.0F;
            mRoundNameAlpha = 255;
        }

        void AddBall(std::unique_ptr<Ball> ball)
        {
            if (ball->IsTrapped())
            {
                mTrappedBalls.push_back(std::move(ball));
            }
            else
            {
                mBallsToAdd.push_back(std::move(ball));
            }
        }

        void AddCollidable(std::unique_ptr<Collidable2D> collidable)
        {
            mCollidables.push_back(std::move(collidable));
        }

        void AddProjectile(std::unique_ptr<Projectile> projectile)
        {
            mProjectilesToAdd.push_back(std::move(projectile));
        }

        void SpawnPowerUpAt(Game* game, const SDL_FPoint& pos);

        void SetBallsSmall(bool small);

        const std::vector<std::unique_ptr<Ball>>& GetBalls() const
        {
            return mBalls;
        }

        const std::vector<std::unique_ptr<Collidable2D>>& GetCollidables() const
        {
            return mCollidables;
        }

        Ship* GetShip() const
        {
            return mShip.get();
        }

        void SetMegovision(Megovision* mego)
        {
            mMegovision = mego;
        }

        void AddScore(int score)
        {
            mScore += score;
        }
        int GetScore() const
        {
            return mScore;
        }

        void SetIonSpheres(int spheres)
        {
            mIonSpheres = spheres;
        }
        int GetIonSpheres() const
        {
            return mIonSpheres;
        }

        void SetSuspended(bool suspended)
        {
            mIsSuspended = suspended;
        }
        bool IsSuspended() const
        {
            return mIsSuspended;
        }

        void SetIsGameOver(bool gameOver)
        {
            mIsGameOver = gameOver;
        }
        bool IsGameOver() const
        {
            return mIsGameOver;
        }

        bool IsLevelComplete() const
        {
            return mIsLevelComplete;
        }

        bool IsCompletionSequenceFinished() const
        {
            return mCompletionStep >= 5;
        }

        void SetGameOverFrame(StandAloneFrame* frame)
        {
            mGameOverFrame = frame;
        }

        void SetSpawnShip(bool spawn)
        {
            mSpawnShip = spawn;
        }

        void SetBallWaitingForRelease(bool waiting)
        {
            mBallWaitingForRelease = waiting;
        }

        void ApplyPowerUp(Game* game, PowerUpType type);
        void DebugDestroyAllBricks(Game* game);

        void ToggleMouseBallControl();
        bool IsMouseBallControlEnabled() const
        {
            return mMouseBallControl;
        }

        void SpawnParticleBurst(Game* game,
                                const std::string& path,
                                const SDL_FPoint& pos,
                                float baseAngle = 0.0F,
                                float posVariation = 0.0F,
                                float lifeMultiplier = 1.0F);
        void SpawnParticleBurst(const nuvelocity::ParticleGeneratorInfo* info,
                                const SDL_FPoint& pos,
                                const std::vector<nuvelocity::ParticleType*>* customTypes = nullptr,
                                float baseAngle = 0.0F,
                                float posVariation = 0.0F,
                                float lifeMultiplier = 1.0F);

        void SetBounds(const SDL_Rect& rect)
        {
            mBounds = rect;
        }

        const SDL_Rect& GetBounds() const
        {
            return mBounds;
        }

        void SetBoundaryFlags(BoundaryFlags flags)
        {
            mBoundaryFlags = flags;
        }

        void SetPowerUpWeights(const std::map<std::string, int>& weights)
        {
            mPowerUpWeights = weights;
        }

        static inline std::string MakeImagePath(const std::string& base, const std::string& image)
        {
            std::filesystem::path full(base);
            std::filesystem::path parent = full.parent_path();
            return (parent / image).string();
        }

    private:
        void HandleBallOut(Game* game, Ball* ball);

        std::vector<std::unique_ptr<Ball>> mBalls;
        std::vector<std::unique_ptr<Ball>> mTrappedBalls;
        std::vector<std::unique_ptr<Ball>> mBallsToAdd;
        std::vector<std::unique_ptr<Collidable2D>> mCollidables;
        std::vector<std::unique_ptr<Projectile>> mProjectiles;
        std::vector<std::unique_ptr<Projectile>> mProjectilesToAdd;
        std::vector<std::unique_ptr<PowerUp>> mPowerUps;
        std::vector<std::unique_ptr<Bomb>> mBombs;

        Megovision* mMegovision;
        int mScore;
        int mIonSpheres;
        float mLevelTime;
        bool mIsSuspended;
        bool mIsGameOver;
        float mGameOverTimer;
        bool mGameOverSfxPlayed;
        bool mBallWaitingForRelease;
        bool mMouseBallControl;

        nuvelocity::GameStats mGameStats;
        bool mIsLevelComplete;
        bool mBallsSuspended;
        float mCompletionTimer;
        int mCompletionStep;
        int mCurrentBallBonusIndex;
        float mBallBonusTimer;

    private:
        std::string mRoundName;
        float mRoundNameTimer;
        uint8_t mRoundNameAlpha;

        // Background state
        BackgroundDefinition* mBackgroundDef;
        StandAloneFrame* mBgImage;
        Sequence* mMgImage;
        StandAloneFrame* mFgImage;
        std::vector<std::unique_ptr<BackgroundSprite>> mBgSprites;
        std::vector<std::unique_ptr<BackgroundSprite>> mFgSprites;
        SDL_FPoint mShadowOffset;

        // Ship state
        std::unique_ptr<Ship> mShip;
        bool mSpawnShip;
        StandAloneFrame* mGameOverFrame;
        SDL_Rect mBounds;
        BoundaryFlags mBoundaryFlags;

        void SpawnBackgroundSprites(Game* game);

        void UpdateBackgroundLayer(std::vector<std::unique_ptr<BackgroundSprite>>& layer,
                                   float deltaTime,
                                   int windowWidth,
                                   int windowHeight);

        void DrawBackgroundLayer(const std::vector<std::unique_ptr<BackgroundSprite>>& layer,
                                 Game* game,
                                 bool drawShadows);

        void CheckScreenBoundary(Game* game,
                                 SDL_FPoint& pos,
                                 const float radius,
                                 SDL_FPoint& vel,
                                 std::unique_ptr<nuvelocity::frs42::Ball>& ball);

        void DrawBackground(Game* game);

        void DrawMidground(Game* game);

        void DrawForeground(Game* game);

        std::vector<std::unique_ptr<ParticleGenerator>> mParticleGenerators;
        std::map<std::string, int> mPowerUpWeights;
        void ApplyBallSpeedUp(Ball* ball, const SDL_FPoint& hitPos, bool isBrick);
    };
} // namespace nuvelocity::frs42

#endif // NVE_GAME_BOARD_H
