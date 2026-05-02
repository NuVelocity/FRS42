#include "Playfield.h"
#include "ArenaScene.h"
#include "BackgroundDefinition.h"
#include "Ball.h"
#include "Bomb.h"
#include "Brick.h"
#include "BrickInfo.h"
#include "Collidable2D.h"
#include "EmitterType.h"
#include "FishAI.h"
#include "FontManager.h"
#include "MainMenuScene.h"
#include "MathUtils.h"
#include "Megovision.h"
#include "ParticleGenerator.h"
#include "ParticleGeneratorInfo.h"
#include "PlanktonAI.h"
#include "PowerUp.h"
#include "Projectile.h"
#include "RandomDebrisAI.h"
#include "RoundSetManager.h"
#include "Ship.h"
#include "StatsManager.h"
#include "TextAlignment.h"
#include <Game.h>
#include <Scene.h>
#include <Sequence.h>
#include <StandAloneFrame.h>
#include <algorithm>
#include <cmath>
#include <numbers>
#include <random>
#include <system/AssetManager.h>
#include <system/AudioManager.h>
#include <system/InputManager.h>
#include <system/SpriteBatch.h>

namespace nuvelocity::frs42
{
    static std::random_device gRd;
    static std::mt19937 gGen(gRd());

    void BackgroundSprite::Update(float deltaTime, int windowWidth, int windowHeight)
    {
        mPosition.x += mVelocity.x * deltaTime;
        mPosition.y += mVelocity.y * deltaTime;

        if (mVelocity.x > 0 && mPosition.x > windowWidth + 100)
        {
            mPosition.x = -100;
        }
        else if (mVelocity.x < 0 && mPosition.x < -100)
        {
            mPosition.x = windowWidth + 100;
        }

        if (mVelocity.y > 0 && mPosition.y > windowHeight + 100)
        {
            mPosition.y = -100;
        }
        else if (mVelocity.y < 0 && mPosition.y < -100)
        {
            mPosition.y = windowHeight + 100;
        }
    }

    void BackgroundSprite::Draw(Game* game, bool drawShadow, const SDL_FPoint& shadowOffset) const
    {
        if (mSequence == nullptr)
        {
            return;
        }

        const std::size_t frameCount = mSequence->GetFrameCount();
        if (frameCount == 0)
        {
            return;
        }

        const uint64_t now = SDL_GetTicks();
        const uint64_t elapsed = now - mStartTick;
        const float fps = mSequence->GetFramesPerSecond();
        const std::size_t frameIndex =
            static_cast<std::size_t>((static_cast<double>(elapsed) * fps) / 1000.0) % frameCount;

        if (drawShadow)
        {
            game->mSpriteBatch->Draw(mSequence,
                                     frameIndex,
                                     mPosition.x + shadowOffset.x,
                                     mPosition.y + shadowOffset.y,
                                     {.r = 0, .g = 0, .b = 0, .a = 128});
        }

        game->mSpriteBatch->Draw(mSequence, frameIndex, mPosition.x, mPosition.y);
    }

    Playfield::Playfield()
            : mMegovision(nullptr)
            , mScore(0)
            , mIonSpheres(2)
            , mLevelTime(0.0F)
            , mIsSuspended(false)
            , mIsGameOver(false)
            , mGameOverTimer(0.0F)
            , mGameOverSfxPlayed(false)
            , mBallWaitingForRelease(false)
            , mMouseBallControl(false)
            , mIsLevelComplete(false)
            , mBallsSuspended(false)
            , mCompletionTimer(0.0F)
            , mCompletionStep(0)
            , mCurrentBallBonusIndex(-1)
            , mBallBonusTimer(0.0F)
            , mRoundEntry(nullptr)
            , mRoundNameTimer(0.0F)
            , mRoundNameAlpha(0)
            , mBackgroundDef(nullptr)
            , mBgImage(nullptr)
            , mMgImage(nullptr)
            , mFgImage(nullptr)
            , mShadowOffset({0, 0})
            , mGameOverFrame(nullptr)
            , mBounds({0, 0, 640, 480})
            , mBoundaryFlags(static_cast<BoundaryFlags>(Left | Top | Right))
    {
    }
    Playfield::~Playfield() = default;

    void Playfield::Reset(Game* game)
    {
        mBalls.clear();
        mTrappedBalls.clear();
        mBallsToAdd.clear();
        mCollidables.clear();
        mProjectiles.clear();
        mProjectilesToAdd.clear();
        mPowerUps.clear();
        mBombs.clear();
        mBgSprites.clear();
        mFgSprites.clear();
        mParticleGenerators.clear();
        mLevelTime = 0.0F;
        mShip = nullptr;
        mIsSuspended = false;
        mIsLevelComplete = false;
        mBallsSuspended = false;
        mCompletionTimer = 0.0F;
        mCompletionStep = 0;
        mCurrentBallBonusIndex = -1;
        mBallBonusTimer = 0.0F;
        mIsGameOver = false;

        if (mIsStandAlone)
        {
            return;
        }

        float baseSpeed = 268.6F;
        int diff = mGameStats.mLevelOfDifficulty;
        if (diff == 0)
        {
            baseSpeed *= 0.8F;
        }
        else if (diff == 2)
        {
            baseSpeed *= 1.2F;
        }
        else if (diff == 3)
        {
            baseSpeed *= 1.5F;
        }

        auto ball = std::make_unique<Ball>();
        ball->SetPlayfield(this);
        ball->SetSpeed(baseSpeed);
        ball->SetDirection({0.371F, -0.928F});
        if (diff == 3)
        {
            ball->SetIsSmall(true);
        }
        ball->AttachSequence(game);
        ball->SetIsAttached(true);
        ball->SetIsAttachedToShield(true);
        ball->SetAttachmentOffset({20.0F, -38.0F});
        AddBall(std::move(ball));
        mBallWaitingForRelease = true;
    }

    void Playfield::LoadBackground(Game* game, const std::string& path)
    {
        if (path.empty() || path.find("!None") != std::string::npos)
        {
            return;
        }

        std::string fullPath = path;
        if (fullPath.find("Resources/") == std::string::npos)
        {
            fullPath = "Resources/" + fullPath;
        }

        mBackgroundDef =
            static_cast<BackgroundDefinition*>(game->mAsset->LoadBackgroundDefinition(fullPath));
        if (mBackgroundDef == nullptr)
        {
            return;
        }

        if (mBackgroundDef->GetBackgroundImage() != "!None")
        {
            std::string bgPath =
                Playfield::MakeImagePath(fullPath, mBackgroundDef->GetBackgroundImage());
            mBgImage = game->mAsset->TryLoadStandAloneFrame(bgPath);
        }
        if (mBackgroundDef->GetMidgroundImage() != "!None")
        {
            std::string mgPath =
                Playfield::MakeImagePath(fullPath, mBackgroundDef->GetMidgroundImage());
            mMgImage = game->mAsset->TryLoadSequence(mgPath);
        }
        if (mBackgroundDef->GetForegroundImage() != "!None")
        {
            std::string fgPath =
                Playfield::MakeImagePath(fullPath, mBackgroundDef->GetForegroundImage());
            mFgImage = game->mAsset->TryLoadStandAloneFrame(fgPath);
        }

        mShadowOffset = mBackgroundDef->mShadowOffset;

        // Load Music
        if (mBackgroundDef->mMusicTracks != nullptr)
        {
            auto* music = mBackgroundDef->mMusicTracks;
            if (!music->mFullMixSong.empty())
            {
                auto* audioData = game->mAsset->LoadMusic(music->mFullMixSong);
                game->mAudio->RegisterBgm(audioData);
                game->mAudio->PlayBgm(audioData->path);
            }
        }

        // Load Ship if requested
        if (!mIsStandAlone)
        {
            mShip = std::make_unique<Ship>();
            mShip->SetPlayfield(this);
            if (auto* player = StatsManager::Get().GetCurrentPlayer())
            {
                mShip->SetShipStyle(player->mShipStyle);
            }
            mShip->Load(game);
            mShip->SetPosition({static_cast<float>(mBounds.x + mBounds.w / 2),
                                static_cast<float>(mBounds.y + mBounds.h - 40)});
        }

        SpawnBackgroundSprites(game);
    }

    void Playfield::SpawnBackgroundSprites(Game* game)
    {
        mBgSprites.clear();
        mFgSprites.clear();

        if (mBackgroundDef == nullptr)
        {
            return;
        }

        for (auto* gen : mBackgroundDef->GetSpriteGenerators())
        {
            if (gen == nullptr || gen->mCount <= 0 || gen->mType == nullptr)
            {
                continue;
            }

            auto& layer = (gen->mLayer == "Foreground Sprites") ? mFgSprites : mBgSprites;

            for (int i = 0; i < gen->mCount; ++i)
            {
                auto sprite = std::make_unique<BackgroundSprite>();
                sprite->mAI = gen->mType;
                sprite->mStartTick = SDL_GetTicks() + (gGen() % 5000);

                std::uniform_real_distribution<float> distWidth(
                    0.0F, static_cast<float>(game->mWindowWidth));
                std::uniform_real_distribution<float> distHeight(
                    0.0F, static_cast<float>(game->mWindowHeight));
                sprite->mPosition = {.x = distWidth(gGen), .y = distHeight(gGen)};

                if (auto* fish = dynamic_cast<FishAI*>(gen->mType))
                {
                    std::uniform_real_distribution<float> distSpeed(fish->mMinSpeed,
                                                                    fish->mMaxSpeed);
                    float speed = distSpeed(gGen);
                    if (gGen() % 2 == 0)
                    {
                        sprite->mVelocity = {.x = speed, .y = 0};
                        sprite->mSequence = game->mAsset->LoadSequence("Resources/Environments/" +
                                                                       fish->mRightSequence);
                    }
                    else
                    {
                        sprite->mVelocity = {.x = -speed, .y = 0};
                        sprite->mSequence = game->mAsset->LoadSequence("Resources/Environments/" +
                                                                       fish->mLeftSequence);
                    }
                }
                else if (auto* plankton = dynamic_cast<PlanktonAI*>(gen->mType))
                {
                    std::uniform_real_distribution<float> distSpeed(plankton->mMinSpeed,
                                                                    plankton->mMaxSpeed);
                    std::uniform_real_distribution<float> distAngle(
                        0, 2.0F * std::numbers::pi_v<float>);
                    float speed = distSpeed(gGen);
                    float angle = distAngle(gGen);
                    sprite->mVelocity = {.x = speed * std::cos(angle),
                                         .y = speed * std::sin(angle)};
                    sprite->mSequence =
                        game->mAsset->LoadSequence("Resources/Environments/" + plankton->mSequence);
                }
                else if (auto* debris = dynamic_cast<RandomDebrisAI*>(gen->mType))
                {
                    std::uniform_real_distribution<float> distSpeed(debris->mMinSpeed,
                                                                    debris->mMaxSpeed);
                    std::uniform_real_distribution<float> distAngle(debris->mMinAngle,
                                                                    debris->mMaxAngle);
                    float speed = distSpeed(gGen);
                    float angle = distAngle(gGen);
                    sprite->mVelocity = {.x = speed * std::cos(angle),
                                         .y = speed * std::sin(angle)};
                    sprite->mSequence =
                        game->mAsset->LoadSequence("Resources/Environments/" + debris->mSequence);
                }

                layer.push_back(std::move(sprite));
            }
        }
    }

    void Playfield::UpdateBackgroundLayer(std::vector<std::unique_ptr<BackgroundSprite>>& layer,
                                          float deltaTime,
                                          int windowWidth,
                                          int windowHeight)
    {
        for (auto& sprite : layer)
        {
            sprite->Update(deltaTime, windowWidth, windowHeight);
        }
    }

    void Playfield::DrawBackground(Game* game)
    {
        if (mBgImage != nullptr)
        {
            game->mSpriteBatch->Draw(mBgImage, 0, 0);
        }

        if (mBackgroundDef != nullptr)
        {
            DrawBackgroundLayer(mBgSprites, game, mBackgroundDef->mDrawShadows);
        }
    }

    void Playfield::DrawMidground(Game* game)
    {
        if (mMgImage != nullptr)
        {
            game->mSpriteBatch->Draw(mMgImage, 0, 0, 0);
        }
    }

    void Playfield::DrawForeground(Game* game)
    {
        if (mBackgroundDef != nullptr)
        {
            DrawBackgroundLayer(mFgSprites, game, mBackgroundDef->mDrawShadows);
        }

        if (mFgImage != nullptr)
        {
            game->mSpriteBatch->Draw(mFgImage, 0, 0);
        }
    }

    void Playfield::DrawBackgroundLayer(const std::vector<std::unique_ptr<BackgroundSprite>>& layer,
                                        Game* game,
                                        bool drawShadows)
    {
        for (const auto& sprite : layer)
        {
            sprite->Draw(game, drawShadows, mShadowOffset);
        }
    }

    void Playfield::Draw(Game* game)
    {
        DrawBackground(game);
        DrawMidground(game);

        for (const auto& collidable : mCollidables)
        {
            collidable->Draw(game);
        }
        for (const auto& proj : mProjectiles)
        {
            proj->Draw(game);
        }
        for (const auto& pu : mPowerUps)
        {
            pu->Draw(game);
        }
        for (const auto& bomb : mBombs)
        {
            bomb->Draw(game);
        }
        if (!mIsStandAlone)
        {
            for (const auto& gen : mParticleGenerators)
            {
                gen->Draw(game);
            }
        }

        if (mShip)
        {
            mShip->Draw(game);
        }

        bool catchMode = (mShip != nullptr) && mShip->GetCatchMode();
        SDL_FPoint shieldPos = {0, 0};
        SDL_FPoint shipPos = {0, 0};
        SDL_FPoint shipOff1 = {0, 0};
        SDL_FPoint shipOff2 = {0, 0};
        SDL_FPoint shieldOff1 = {0, 0};
        SDL_FPoint shieldOff2 = {0, 0};

        if (catchMode)
        {
            shieldPos = mShip->GetShieldPosition();
            shipPos = mShip->GetPosition();
            shipOff1 = mShip->GetElectricShipOffset1();
            shipOff2 = mShip->GetElectricShipOffset2();
            shieldOff1 = mShip->GetElectricShieldOffset1();
            shieldOff2 = mShip->GetElectricShieldOffset2();
        }

        for (const auto& ball : mBalls)
        {
            if (catchMode)
            {
                bool shouldDrawLines = ball->IsAttached();
                if (!shouldDrawLines)
                {
                    SDL_FPoint bPos = ball->GetPosition();
                    float dx = bPos.x - shieldPos.x;
                    float dy = bPos.y - shieldPos.y;
                    float distSq = dx * dx + dy * dy;
                    if (distSq < mElectricCatcherRadius * mElectricCatcherRadius)
                    {
                        shouldDrawLines = true;
                    }
                }

                if (shouldDrawLines)
                {
                    SDL_FPoint ballCenter = ball->GetPosition();
                    mShip->DrawElectricLine(
                        game, {shipPos.x + shipOff1.x, shipPos.y + shipOff1.y}, ballCenter);
                    mShip->DrawElectricLine(
                        game, {shipPos.x + shipOff2.x, shipPos.y + shipOff2.y}, ballCenter);
                    mShip->DrawElectricLine(
                        game, {shieldPos.x + shieldOff1.x, shieldPos.y + shieldOff1.y}, ballCenter);
                    mShip->DrawElectricLine(
                        game, {shieldPos.x + shieldOff2.x, shieldPos.y + shieldOff2.y}, ballCenter);
                }
            }
            ball->Draw(game);
        }

        for (const auto& ball : mTrappedBalls)
        {
            ball->Draw(game);
        }

        DrawForeground(game);

        if (mIsGameOver && (mGameOverFrame != nullptr))
        {
            constexpr int rxYAdjustment = -6;
            const int centerX = mBounds.x + (mBounds.w / 2) - (mGameOverFrame->GetWidth() / 2);
            const int centerY =
                mBounds.y + (mBounds.h / 2) - (mGameOverFrame->GetHeight() / 2) + rxYAdjustment;

            game->mSpriteBatch->Draw(mGameOverFrame, centerX, centerY);
        }

        if (mRoundNameTimer > 0.0F && mRoundEntry != nullptr && mRoundEntry->globalIndex >= 0)
        {
            const auto& roundName = mRoundDisplayName;
            SDL_Color textColor = {.r = 255, .g = 255, .b = 255, .a = mRoundNameAlpha};
            SDL_Color bgColor = {
                .r = 0, .g = 0, .b = 0, .a = static_cast<uint8_t>(mRoundNameAlpha / 2)};

            int centerX = mBounds.x + mBounds.w / 2;
            int centerY = mBounds.y + mBounds.h / 2;

            int tw = 0;
            int th = 0;
            game->mFont->MeasureStringWithFont("Big White", roundName, -1, tw, th);

            const int padding = 5;
            SDL_Rect bgRect = {.x = centerX - (tw / 2) - padding,
                               .y = centerY - (th / 2) - padding,
                               .w = tw + padding * 2,
                               .h = th + padding * 2};
            game->mSpriteBatch->FillRect(&bgRect, bgColor);

            game->mFont->DrawStringWithFontAt("Big White",
                                              game->mSpriteBatch,
                                              roundName,
                                              centerX,
                                              centerY,
                                              textColor,
                                              -1,
                                              TextAlignment::Center);
        }

        if (mBounds.w > 0 && mBounds.h > 0 && game->mSpriteBatch->IsDrawBoundsEnabled())
        {
            if ((mBoundaryFlags & Left) != 0)
            {
                game->mSpriteBatch->DrawLine(
                    mBounds.x, mBounds.y, mBounds.x, mBounds.y + mBounds.h, Colors::Green);
            }
            if ((mBoundaryFlags & Top) != 0)
            {
                game->mSpriteBatch->DrawLine(
                    mBounds.x, mBounds.y, mBounds.x + mBounds.w, mBounds.y, Colors::Green);
            }
            if ((mBoundaryFlags & Right) != 0)
            {
                game->mSpriteBatch->DrawLine(mBounds.x + mBounds.w,
                                             mBounds.y,
                                             mBounds.x + mBounds.w,
                                             mBounds.y + mBounds.h,
                                             Colors::Green);
            }
            if ((mBoundaryFlags & Bottom) != 0)
            {
                game->mSpriteBatch->DrawLine(mBounds.x,
                                             mBounds.y + mBounds.h,
                                             mBounds.x + mBounds.w,
                                             mBounds.y + mBounds.h,
                                             Colors::Green);
            }
        }
    }

    void Playfield::Update(Game* game)
    {
        float deltaTime = game->GetDeltaTime();

        if (!mBallsToAdd.empty())
        {
            for (auto& ball : mBallsToAdd)
            {
                mBalls.push_back(std::move(ball));
            }
            mBallsToAdd.clear();
        }
        if (!mProjectilesToAdd.empty())
        {
            for (auto& proj : mProjectilesToAdd)
            {
                mProjectiles.push_back(std::move(proj));
            }
            mProjectilesToAdd.clear();
        }

        mInfectionTimer -= deltaTime;
        if (mInfectionTimer <= 0.0F && !mInfectionQueue.empty())
        {
            PendingInfection next = mInfectionQueue.front();
            mInfectionQueue.pop();

            if (!next.target->IsDestroyed() && next.target->GetBrickInfoPath() == next.lookLike)
            {
                ChangeBrickType(game, next.target, next.changeTo);

                // Add neighbors of this newly transformed brick
                SDL_FPoint pos = next.target->GetPosition();
                const float searchDistX = 34.0F;
                const float searchDistY = 20.0F;

                for (const auto& collidable : mCollidables)
                {
                    Brick* neighbor = dynamic_cast<Brick*>(collidable.get());
                    if (!neighbor || neighbor->IsDestroyed() || mInfectedBricks.count(neighbor))
                    {
                        continue;
                    }

                    SDL_FPoint nPos = neighbor->GetPosition();
                    float dx = std::abs(nPos.x - pos.x);
                    float dy = std::abs(nPos.y - pos.y);

                    if (dx <= searchDistX && dy <= searchDistY && (dx > 0.1F || dy > 0.1F))
                    {
                        if (neighbor->GetBrickInfoPath() == next.lookLike)
                        {
                            mInfectedBricks.insert(neighbor);
                            mInfectionQueue.push({neighbor, next.lookLike, next.changeTo});
                        }
                    }
                }
            }
            mInfectionTimer = 0.05F;
        }

        if (mInfectionQueue.empty() && !mInfectedBricks.empty())
        {
            mInfectedBricks.clear();
        }

        if (mIsGameOver)
        {
            mGameOverTimer += deltaTime;
            if (mGameOverTimer >= 3.0F)
            {
                auto arena = dynamic_cast<ArenaScene*>(game->GetScene());
                if (arena)
                {
                    arena->EndGame(game, true);
                }
                else
                {
                    StatsManager::Get().UpdateCareerStats(&mGameStats, true);
                    game->SetScene(new MainMenuScene());
                }
                return;
            }
        }

        if (mIsLevelComplete)
        {
            mCompletionTimer += deltaTime;
            switch (mCompletionStep)
            {
            case 0: // All bricks cleared
                game->mAudio->PlaySfx("Level Ending Tune.ogg");
                mCompletionStep = 1;
                mCompletionTimer = 0.0F;
                break;
            case 1: // Wait 0.25s then stop balls
                if (mCompletionTimer >= 0.25F)
                {
                    mBallsSuspended = true;
                    mCompletionStep = 3; // Skip to bonus
                    mCompletionTimer = 0.0F;
                }
                break;
            case 3: // Pop balls for bonus
                if (!mBalls.empty())
                {
                    if (mCurrentBallBonusIndex == -1)
                    {
                        mCurrentBallBonusIndex = 0;
                        mBallBonusTimer = 0.0F;
                    }

                    if (mCurrentBallBonusIndex < static_cast<int>(mBalls.size()))
                    {
                        mBallBonusTimer += deltaTime;
                        if (mBallBonusTimer >= 0.5F)
                        {
                            auto& ball = mBalls[mCurrentBallBonusIndex];
                            game->mAudio->PlaySfx("Score.ogg");
                            ball->SetSequence(
                                game->mAsset->LoadSequence("Resources/Effects/Small Gore"));
                            if (mMegovision)
                            {
                                std::vector<std::unique_ptr<Label>> labels;
                                auto label = std::make_unique<Label>("Bonus Points", "Megovision");
                                label->SetWrap(true);
                                labels.push_back(std::move(label));
                                labels.push_back(
                                    std::make_unique<Label>("500 Points", "Small Blue"));
                                mMegovision->ShowMessage(std::move(labels), 0.5F, false);
                            }
                            AddScore(500);
                            mCurrentBallBonusIndex++;
                            mBallBonusTimer = 0.0F;
                        }
                    }
                    else
                    {
                        // Sequence finished
                        StatsManager::Get().UpdateCheckpoint(mRoundEntry->globalIndex);
                        StatsManager::Get().UpdateCareerStats(&mGameStats, false);
                        mCompletionStep = 5;
                    }
                }
                else
                {
                    StatsManager::Get().UpdateCheckpoint(mRoundEntry->globalIndex);
                    StatsManager::Get().UpdateCareerStats(&mGameStats, false);
                    mCompletionStep = 5;
                }
                break;
            }
        }

        if (mIsSuspended && !mIsLevelComplete)
        {
            return;
        }

        // Check for level completion
        bool hasDestructible = false;
        for (const auto& collidable : mCollidables)
        {
            if (auto* brick = dynamic_cast<Brick*>(collidable.get()))
            {
                if (!brick->IsDestroyed() &&
                    !Brick::IsIndestructibleType(brick->GetInfo()->GetBrickType()))
                {
                    hasDestructible = true;
                    break;
                }
            }
        }
        if (!hasDestructible && !mIsLevelComplete && !mIsStandAlone)
        {
            mIsLevelComplete = true;
            mCompletionStep = 0;
            mCompletionTimer = 0.0F;
        }

        if (mMouseBallControl)
        {
            SDL_Point mousePos = game->mInput->GetMousePosition();
            for (auto& ball : mBalls)
            {
                ball->SetPosition({static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)});
                ball->SetVelocity({0, 0});
            }
        }

        for (auto& gen : mParticleGenerators)
        {
            gen->Update(game);
        }
        std::erase_if(mParticleGenerators, [](const auto& g) { return g->IsFinished(); });

        UpdateBackgroundLayer(mBgSprites, deltaTime, game->mWindowWidth, game->mWindowHeight);
        UpdateBackgroundLayer(mFgSprites, deltaTime, game->mWindowWidth, game->mWindowHeight);

        if (mShip)
        {
            mShip->Update(game);

            SDL_FPoint shipPos = mShip->GetPosition();
            Sequence* seq = mShip->GetBaseSequence();
            float minX = 0.0F;
            float maxX = 0.0F;
            if ((seq != nullptr) && seq->GetFrameCount() > 0)
            {
                Frame* frame = seq->GetFrame(0);
                if ((frame != nullptr) && (frame->GetSurface() != nullptr))
                {
                    minX = static_cast<float>(frame->GetHotSpot().x);
                    maxX = minX + static_cast<float>(frame->GetSurface()->w);
                }
            }

            if (shipPos.x + minX < static_cast<float>(mBounds.x))
            {
                shipPos.x = static_cast<float>(mBounds.x) - minX;
            }
            if (shipPos.x + maxX > static_cast<float>(mBounds.x + mBounds.w))
            {
                shipPos.x = static_cast<float>(mBounds.x + mBounds.w) - maxX;
            }
            mShip->SetPosition(shipPos);
        }

        // Projectiles
        for (auto& proj : mProjectiles)
        {
            proj->Update(game);
            // Check collision with bricks
            for (const auto& collidable : mCollidables)
            {
                if (collidable->IsDestroyed())
                {
                    continue;
                }
                SDL_FPoint tempPos = proj->GetPosition();
                SDL_FPoint tempVel = {0, 0};
                if (MathUtils::ResolveCirclePolygonCollision(collidable->GetCollisionPolygon(),
                                                             collidable->GetPosition(),
                                                             tempPos,
                                                             4.0F,
                                                             tempVel))
                {
                    collidable->OnHit(game, mBounds);
                    proj->Die();

                    if (proj->GetType() == ProjectileType::Missile)
                    {
                        // Splash damage: find adjacent bricks (horiz or vert)
                        for (const auto& other : mCollidables)
                        {
                            if (other.get() == collidable.get() || other->IsDestroyed())
                            {
                                continue;
                            }

                            if (auto* otherBrick = dynamic_cast<Brick*>(other.get()))
                            {
                                float dx = std::abs(otherBrick->GetPosition().x -
                                                    collidable->GetPosition().x);
                                float dy = std::abs(otherBrick->GetPosition().y -
                                                    collidable->GetPosition().y);

                                // Adjacent: Same row (dy < 8) and touching horizontally (dx <= 33)
                                // OR same column (dx < 8) and touching vertically (dy <= 20)
                                if ((dy < 8.0F && dx <= 33.0F) || (dx < 8.0F && dy <= 20.0F))
                                {
                                    otherBrick->OnHit(game, mBounds);
                                }
                            }
                        }
                    }
                    break;
                }
            }
        }
        std::erase_if(mProjectiles, [](const auto& p) { return p->IsDead(); });

        // PowerUps
        for (auto& pu : mPowerUps)
        {
            pu->Update(game);
            SDL_FPoint tempPos = pu->GetPosition();
            SDL_FPoint tempVel = {0, 0};
            if (mShip &&
                MathUtils::ResolveCirclePolygonCollision(
                    mShip->GetCollisionPolygon(), mShip->GetPosition(), tempPos, 10.0F, tempVel))
            {
                // Pickup!
                AddScore(100);

                PowerUpType type = pu->GetType();
                if (mMegovision)
                {
                    std::vector<std::unique_ptr<Label>> labels;
                    labels.push_back(std::make_unique<Label>(PowerUp::GetName(type), "Megovision"));
                    labels.push_back(std::make_unique<Label>("+100 Points", "Small Blue"));
                    mMegovision->ShowMessage(std::move(labels), 4.0F, false);
                }

                ApplyPowerUp(game, type);
                pu->SetDead(true);
            }
        }
        std::erase_if(mPowerUps, [](const auto& pu) { return pu->IsDead(); });

        // Bombs
        for (auto& bomb : mBombs)
        {
            bomb->Update(game);
            for (auto& ball : mBalls)
            {
                SDL_FPoint ballPos = ball->GetPosition();
                SDL_FPoint ballVel = ball->GetVelocity();
                if (MathUtils::ResolveCirclePolygonCollision(
                        std::vector<SDL_FPoint>{{-10, -10}, {10, -10}, {10, 10}, {-10, 10}},
                        bomb->GetPosition(),
                        ballPos,
                        ball->GetRadius(),
                        ballVel))
                {
                    ball->SetPosition(ballPos);
                    ball->SetVelocity(ballVel);
                    bomb->SetDead(true);
                    AddScore(1000);
                    if (mMegovision)
                    {
                        std::vector<std::unique_ptr<Label>> labels;
                        auto label = std::make_unique<Label>("Bomb Destroyed", "Megovision");
                        label->SetWrap(true);
                        labels.push_back(std::move(label));
                        labels.push_back(std::make_unique<Label>("+1000 Points", "Small Blue"));
                        mMegovision->ShowMessage(std::move(labels), 1.5F, false);
                    }
                    break;
                }
            }
            // Ship-Bomb
            SDL_FPoint bombPos = bomb->GetPosition();
            SDL_FPoint bombVel = {0, 0};
            if (mShip && !bomb->IsDead() &&
                MathUtils::ResolveCirclePolygonCollision(
                    mShip->GetCollisionPolygon(), mShip->GetPosition(), bombPos, 10.0F, bombVel))
            {
                bomb->SetDead(true);
                game->mAudio->PlaySfx("Ship EXPLODE.ogg");
                if (mMegovision)
                {
                    auto label = std::make_unique<Label>("Ship Destroyed", "Megovision");
                    label->SetWrap(true);
                    mMegovision->ShowMessage(std::move(label), 1.5F, false);
                }
                mShip->Explode(game);
                HandleBallOut(game); // Lose a life
            }
        }
        std::erase_if(mBombs, [](const auto& b) { return b->IsDead(); });

        // Check for released trapped balls
        for (auto it = mTrappedBalls.begin(); it != mTrappedBalls.end();)
        {
            if (!(*it)->IsTrapped())
            {
                mBalls.push_back(std::move(*it));
                it = mTrappedBalls.erase(it);
            }
            else
            {
                ++it;
            }
        }

        UpdateBalls(game, mBalls);
        UpdateBalls(game, mTrappedBalls);

        if (!mIsSuspended)
        {
            mLevelTime += deltaTime;
            mGameStats.mSecondsPlayed += deltaTime;
        }

        // Push stats to Megovision
        if (mMegovision != nullptr)
        {
            mMegovision->SetScore(mScore);
            mMegovision->SetLevelTime(mLevelTime);
            mMegovision->SetIonSpheres(mIonSpheres);

            float maxSpeed = 0.0F;
            for (const auto& ball : mBalls)
            {
                if (ball->IsAttached())
                {
                    continue;
                }
                SDL_FPoint v = ball->GetVelocity();
                float s = std::sqrt((v.x * v.x) + (v.y * v.y));
                maxSpeed = std::max(s, maxSpeed);
            }
            mMegovision->SetBallSpeed(static_cast<int>(maxSpeed));
        }

        // Check for balls out
        auto [rmFirst, rmLast] =
            std::ranges::remove_if(mBalls,
                                   [&](const auto& ball)
                                   {
                                       if (ball->GetPosition().y - ball->GetRadius() >
                                           static_cast<float>(game->mWindowHeight))
                                       {
                                           HandleBallOut(game);
                                           return true;
                                       }
                                       return false;
                                   });
        mBalls.erase(rmFirst, rmLast);

        for (auto& collidable : mCollidables)
        {
            collidable->Update(game);
        }

        if (mRoundNameTimer > 0.0F)
        {
            mRoundNameTimer -= deltaTime;
            if (mRoundNameTimer < 1.0F)
            {
                mRoundNameAlpha = static_cast<uint8_t>(mRoundNameTimer * 255.0F);
            }
            else
            {
                mRoundNameAlpha = 255;
            }
        }

        if (mBallWaitingForRelease)
        {
            if (mShip)
            {
                bool releaseRequested = game->mInput->IsMouseButtonPressed(SDL_BUTTON_LEFT);
                SDL_FPoint shieldPos = mShip->GetShieldPosition();
                SDL_FPoint shipPos = mShip->GetPosition();

                for (auto& ball : mBalls)
                {
                    if (ball->IsAttached())
                    {
                        if (releaseRequested)
                        {
                            ball->SetIsAttached(false);
                            SDL_FPoint v = ball->GetVelocity();
                            if (v.y > 0)
                            {
                                v.y = -v.y;
                                ball->SetVelocity(v);
                            }
                        }
                        else
                        {
                            SDL_FPoint offset = ball->GetAttachmentOffset();
                            SDL_FPoint parentPos = ball->IsAttachedToShield() ? shieldPos : shipPos;
                            ball->SetPosition({parentPos.x + offset.x, parentPos.y + offset.y});
                        }
                    }
                }

                if (releaseRequested)
                {
                    mBallWaitingForRelease = false;
                }
            }
        }

        // Respawn ball if needed
        if (mIonSpheres > 0 && mBalls.empty() && !mIsStandAlone && !mIsLevelComplete)
        {
            mIonSpheres--;
            game->mAudio->PlaySfx("Lost Ball.ogg");
            if (mMegovision != nullptr)
            {
                auto label = std::make_unique<Label>("Out of bounds penalty", "Megovision");
                label->SetWrap(true);
                mMegovision->ShowMessage(std::move(label), 3.5F);
            }
            mShip->SetShieldSize(game, 2); // Reset to Normal Shield
            mShip->SetWeapon(game, WeaponType::None);
            mShip->SetCatchMode(false);
            auto newBall = std::make_unique<Ball>();
            newBall->SetPlayfield(this);
            newBall->SetDirection({0.371F, -0.928F});
            newBall->AttachSequence(game);
            newBall->SetIsAttached(true);
            newBall->SetIsAttachedToShield(true);
            newBall->SetAttachmentOffset({20.0F, -38.0F});
            mBalls.push_back(std::move(newBall));
            mBallWaitingForRelease = true;
        }
    }

    void Playfield::CheckScreenBoundary(
        Game* game, SDL_FPoint& pos, const float radius, SDL_FPoint& vel, Ball* ball)
    {
        bool hitWall = false;
        if (((mBoundaryFlags & Left) != 0) && pos.x - radius < static_cast<float>(mBounds.x))
        {
            pos.x = static_cast<float>(mBounds.x) + radius;
            vel.x = std::abs(vel.x);
            hitWall = true;
        }
        else if (((mBoundaryFlags & Right) != 0) &&
                 pos.x + radius > static_cast<float>(mBounds.x + mBounds.w))
        {
            pos.x = static_cast<float>(mBounds.x + mBounds.w) - radius;
            vel.x = -std::abs(vel.x);
            hitWall = true;
        }

        if (((mBoundaryFlags & Top) != 0) && pos.y - radius < static_cast<float>(mBounds.y))
        {
            pos.y = static_cast<float>(mBounds.y) + radius;
            vel.y = std::abs(vel.y);
            hitWall = true;
        }
        else if (((mBoundaryFlags & Bottom) != 0) &&
                 pos.y + radius > static_cast<float>(mBounds.y + mBounds.h))
        {
            pos.y = static_cast<float>(mBounds.y + mBounds.h) - radius;
            vel.y = -std::abs(vel.y);
            hitWall = true;
        }

        if (hitWall)
        {
            game->mAudio->PlaySfx("Bounce.ogg");
            ball->SpeedUp();
            float angle = std::atan2(vel.y, vel.x);
            {
                std::string pgen;
                if (ball->IsTrapped())
                {
                    pgen = "Particle Generators/Bouce/Ball Hit Wall Small";
                }
                else
                {
                    pgen = ball->IsSmall() ? "Particle Generators/Bouce/Ball Hit Wall Small"
                                           : "Particle Generators/Bouce/Ball Hit Wall";
                }
                SpawnParticleBurst(game, pgen, pos, angle);
            }
        }
    }

    void Playfield::UpdateBalls(Game* game, std::vector<std::unique_ptr<Ball>>& balls)
    {
        for (auto& ball : balls)
        {
            UpdateBall(game, ball.get());
        }
    }

    void Playfield::UpdateBall(Game* game, Ball* ball)
    {
        const float deltaTime = game->GetDeltaTime();
        if (!mBallsSuspended)
        {
            ball->Update(game);
        }
        if (ball->IsAttached() || mBallsSuspended)
        {
            return;
        }

        const float radius = ball->GetRadius();
        const float speed = ball->GetSpeed();
        const float maxStepDist = radius * 0.5F;
        const float moveDist = speed * deltaTime;

        int numSteps = std::max(1, static_cast<int>(std::ceil(moveDist / maxStepDist)));
        const float dt = deltaTime / static_cast<float>(numSteps);

        for (int step = 0; step < numSteps; ++step)
        {
            SDL_FPoint pos = ball->GetPosition();
            SDL_FPoint vel = ball->GetVelocity();

            pos.x += vel.x * dt;
            pos.y += vel.y * dt;
            ball->SetPosition(pos);

            if (mBounds.w > 0 && mBounds.h > 0)
            {
                CheckScreenBoundary(game, pos, radius, vel, ball);
                ball->SetVelocity(vel);
                ball->SetPosition(pos);

                if (ball->GetType() == BallType::Fire)
                {
                    SpawnParticleBurst(
                        game, "Particle Generators/Balls/Fire Ball", pos, 0.0F, 6.0F);
                }
                else if (ball->GetType() == BallType::Rail)
                {
                    SpawnParticleBurst(
                        game, "Particle Generators/Balls/Rail Ball", pos, 0.0F, 2.0F);
                }
            }

            pos = ball->GetPosition();
            vel = ball->GetVelocity();

            // Ship shield collision
            if (mShip)
            {
                SDL_FPoint shieldPos = mShip->GetShieldPosition();

                if (MathUtils::ResolveCirclePolygonCollision(
                        mShip->GetCollisionPolygon(), shieldPos, pos, radius, vel))
                {
                    ball->SetIsTrapped(false);
                    ball->SetVelocity(vel);
                    ball->SetPosition(pos);
                    game->mAudio->PlaySfx("Bounce.ogg");
                    mShip->ImpactRecoil();
                    ApplyBallSpeedUp(ball, pos, false);

                    float angle = std::atan2(vel.y, vel.x);
                    std::string pgen = ball->IsSmall()
                                           ? "Particle Generators/Bouce/Ball Hit Ship Small"
                                           : "Particle Generators/Bouce/Ball Hit Ship";
                    SpawnParticleBurst(game, pgen, pos, angle);

                    if (mShip->GetCatchMode() && !mBallWaitingForRelease)
                    {
                        mBallWaitingForRelease = true;
                        ball->SetIsAttached(true);
                        ball->SetIsAttachedToShield(true);
                        ball->SetAttachmentOffset({pos.x - shieldPos.x, pos.y - shieldPos.y});
                    }
                }
            }

            for (const auto& collidable : mCollidables)
            {
                HandleBallCollidableCollision(game, ball, collidable.get(), pos, vel, radius);
            }
            ball->SetPosition(pos);
        }
    }

    void Playfield::HandleBallCollidableCollision(Game* game,
                                                  Ball* ball,
                                                  Collidable2D* collidable,
                                                  SDL_FPoint& pos,
                                                  SDL_FPoint& vel,
                                                  float radius)
    {
        // 0. Ignore destroyed collidables.
        if (collidable->IsDestroyed())
        {
            return;
        }

        // 1. Check if we collide with the collidable's polygon. If not, skip.
        if (!MathUtils::ResolveCirclePolygonCollision(
                collidable->GetCollisionPolygon(), collidable->GetPosition(), pos, radius, vel))
        {
            return;
        }

        // 2. Bounce the ball and spawn particles. If it's a rail ball, don't apply speed up or hit
        // effects.
        if (ball->GetType() != BallType::Rail)
        {
            ball->SetVelocity(vel);
            ball->SetPosition(pos);
            ApplyBallSpeedUp(ball, pos, true);
        }

        // 3. If it's a fire ball, apply hit effects to up tos 2 adjacent bricks.
        if (ball->GetType() == BallType::Fire)
        {
            int splashCount = 0;
            for (const auto& other : mCollidables)
            {
                if (other.get() == collidable || other->IsDestroyed())
                {
                    continue;
                }

                if (auto* otherBrick = dynamic_cast<Brick*>(other.get()))
                {
                    float dx = std::abs(otherBrick->GetPosition().x - collidable->GetPosition().x);
                    float dy = std::abs(otherBrick->GetPosition().y - collidable->GetPosition().y);
                    // Adjacent: Within 1 row (dy < 20) and touching
                    // horizontally (dx <= 33)
                    if (dy < 20.0F && dx <= 33.0F)
                    {
                        otherBrick->OnHit(game, mBounds);
                        if (!otherBrick->IsDestroyed())
                        {
                            otherBrick->OnHit(game, mBounds);
                        }
                        if (!otherBrick->IsDestroyed())
                        {
                            otherBrick->OnHit(game, mBounds);
                        }
                        splashCount++;
                        if (splashCount >= 2)
                        {
                            break;
                        }
                    }
                }
            }
        }

        auto* brick = dynamic_cast<Brick*>(collidable);
        auto* brickInfo = (brick != nullptr) ? brick->GetInfo() : nullptr;
        // 4.1. If the ball is trapped, just bounce off bricks and do nothing.
        if (ball->IsTrapped())
        {
            if (brick)
            {
                if (Brick::IsIndestructibleType(brickInfo->GetBrickType()))
                {
                    float angle = std::atan2(vel.y, vel.x);
                    SpawnParticleBurst(
                        game, "Particle Generators/Bouce/Ball Hit Wall Small", pos, angle);
                }
            }
            return;
        }
        // 4.2. Apply hit effects if it's not a trapped ball.
        else
        {
            collidable->OnHit(game, mBounds);
            if (ball->GetType() == BallType::Fire)
            {
                if (!collidable->IsDestroyed())
                {
                    collidable->OnHit(game, mBounds);
                }
                if (!collidable->IsDestroyed())
                {
                    collidable->OnHit(game, mBounds);
                }
            }
        }

        // 5.1. If it's not destroyed, spawn ball hit brick particles.
        if (!collidable->IsDestroyed())
        {
            float angle = std::atan2(vel.y, vel.x);
            std::string pgen = ball->IsSmall() ? "Particle Generators/Bouce/Ball Hit Brick Small"
                                               : "Particle Generators/Bouce/Ball Hit Brick";
            SpawnParticleBurst(game, pgen, pos, angle);
        }
        // 5.2. If it's destroyed, add score and spawn brick destroy particles.
        else if (brickInfo != nullptr)
        {
            // Brick already handled scoring and other effects.
        }
        else
        {
            SDL_LogWarn(LogCategory::NVE_LOG_CATEGORY_PROPSYS,
                        "Collidable destroyed but no brick info found!");
            mGameStats.mBricksDestroyed++;
        }
    }

    void Playfield::HandleBallOut(Game* game)
    {
        mGameStats.mBallsLost++;

        if (mMegovision == nullptr || mIonSpheres > 0)
        {
            return;
        }

        game->mAudio->PlaySfx("Lost Ball.ogg");
        mIsGameOver = true;
        mIsSuspended = true;
        game->mAudio->PlaySfx("Game Over.ogg");
        auto label = std::make_unique<Label>("Game Over", "Megovision");
        label->SetWrap(true);
        mMegovision->ShowMessage(std::move(label), 1.75F, true);
    }

    void Playfield::SpawnPowerUpAt(Game* game, const SDL_FPoint& pos)
    {
        if (mPowerUpWeights.empty())
        {
            return;
        }

        int totalWeight = 0;
        for (const auto& [name, weight] : mPowerUpWeights)
        {
            totalWeight += weight;
        }
        if (totalWeight <= 0)
        {
            return;
        }

        std::uniform_int_distribution<> dis(0, totalWeight - 1);
        int r = dis(gGen);

        int current = 0;
        PowerUpType type = PowerUpType::NormalBall;
        for (const auto& [name, weight] : mPowerUpWeights)
        {
            current += weight;
            if (r < current)
            {
                type = PowerUp::TypeFromString(name);
                break;
            }
        }

        SpawnPowerUp(game, pos, type);
    }

    void Playfield::SpawnPowerUp(Game* game, const SDL_FPoint& pos, PowerUpType type)
    {
        std::string path = PowerUp::GetSequencePath(type);
        if (path.empty())
        {
            return;
        }

        Sequence* seq = game->mAsset->LoadSequence(path);
        if (type == PowerUpType::TheBomb)
        {
            game->mAudio->PlaySfx("Power Ups/Bomb Dropped.ogg");
            auto bomb = std::make_unique<Bomb>(pos, seq);
            mBombs.push_back(std::move(bomb));
        }
        else
        {
            auto pu = std::make_unique<PowerUp>(type, pos, seq);
            mPowerUps.push_back(std::move(pu));
        }
    }

    void Playfield::TriggerChainReaction(Game* game,
                                         Brick* source,
                                         const std::string& lookLike,
                                         const std::string& changeTo)
    {
        if (lookLike == "Bricks/!None" || changeTo == "Bricks/!None")
        {
            return;
        }

        // Search neighbors and add matching ones to queue
        SDL_FPoint pos = source->GetPosition();
        const float searchDistX = 34.0F;
        const float searchDistY = 20.0F;

        for (const auto& collidable : mCollidables)
        {
            Brick* neighbor = dynamic_cast<Brick*>(collidable.get());
            if (!neighbor || neighbor->IsDestroyed() || mInfectedBricks.count(neighbor))
            {
                continue;
            }

            SDL_FPoint nPos = neighbor->GetPosition();
            float dx = std::abs(nPos.x - pos.x);
            float dy = std::abs(nPos.y - pos.y);

            if (dx <= searchDistX && dy <= searchDistY && (dx > 0.1F || dy > 0.1F))
            {
                if (neighbor->GetBrickInfoPath() == lookLike)
                {
                    mInfectedBricks.insert(neighbor);
                    mInfectionQueue.push({neighbor, lookLike, changeTo});
                }
            }
        }
    }

    void Playfield::ChangeBrickType(Game* game, Brick* brick, const std::string& infoPath)
    {
        const BrickInfo* info =
            static_cast<const BrickInfo*>(game->mAsset->LoadBrickInfo("Resources/" + infoPath));
        if (info)
        {
            brick->AttachBrickInfo(game, info);
            brick->SetBrickInfoPath(infoPath);

            // Ensure particle gen and types are loaded/resolved (Logic from
            // ArenaScene::GetOrLoadBrickInfo)
            BrickInfo* mutableInfo = const_cast<BrickInfo*>(info);
            if (mutableInfo->GetBreakParticleGen() == nullptr &&
                mutableInfo->GetBreakParticleGenPath() != "!None")
            {
                mutableInfo->SetBreakParticleGen(game->mAsset->LoadParticleGeneratorInfo(
                    "Resources/Effects/" + mutableInfo->GetBreakParticleGenPath()));

                for (auto* pt : mutableInfo->GetBreakParticleTypes())
                {
                    pt->SetSequence(game->mAsset->ResolveParticleSequence(pt->GetParticleType()));
                }
            }

            SpawnParticleBurst(
                game, "Particle Generators/Bricks/Brick Change Type", brick->GetPosition());
        }
    }

    void Playfield::SetBallsSmall(bool small)
    {
        for (auto& ball : mBalls)
        {
            ball->SetIsSmall(small);
        }
    }

    void Playfield::ToggleMouseBallControl()
    {
        mMouseBallControl = !mMouseBallControl;
        if (mShip)
        {
            mShip->SetMouseControlEnabled(!mMouseBallControl);
        }

        if (!mMouseBallControl)
        {
            for (auto& ball : mBalls)
            {
                ball->SetVelocity({100.0F, -250.0F});
            }
        }
    }

    ParticleGenerator* Playfield::SpawnParticleBurst(Game* game,
                                                     const std::string& path,
                                                     const SDL_FPoint& pos,
                                                     float baseAngle,
                                                     float posVariation,
                                                     float lifeMultiplier,
                                                     bool doFadeOut)
    {
        std::string fullPath = "Resources/Effects/" + path;
        auto info = game->mAsset->LoadParticleGeneratorInfo(fullPath);
        return SpawnParticleBurst(
            info, pos, nullptr, baseAngle, posVariation, lifeMultiplier, doFadeOut);
    }

    ParticleGenerator*
    Playfield::SpawnParticleBurst(const nuvelocity::ParticleGeneratorInfo* info,
                                  const SDL_FPoint& pos,
                                  const std::vector<nuvelocity::ParticleType*>* customTypes,
                                  float baseAngle,
                                  float posVariation,
                                  float lifeMultiplier,
                                  bool doFadeOut)
    {
        if (info == nullptr)
        {
            return nullptr;
        }

        auto gen = std::make_unique<ParticleGenerator>();
        gen->Burst(pos, info, customTypes, baseAngle, posVariation, lifeMultiplier, doFadeOut);
        auto* result = gen.get();
        mParticleGenerators.push_back(std::move(gen));
        return result;
    }

    ParticleGenerator*
    Playfield::SpawnParticleGenerator(const nuvelocity::ParticleGeneratorInfo* info,
                                      const SDL_FPoint& pos,
                                      nuvelocity::EmitterType type,
                                      nuvelocity::EmitterShape shape,
                                      float rateOrInterval,
                                      float duration,
                                      float width,
                                      float height,
                                      const std::vector<nuvelocity::ParticleType*>* customTypes,
                                      float baseAngle,
                                      float posVariation,
                                      float lifeMultiplier,
                                      bool doFadeOut)
    {
        if (info == nullptr)
        {
            return nullptr;
        }

        auto gen = std::make_unique<ParticleGenerator>();
        gen->Start(pos,
                   info,
                   type,
                   shape,
                   rateOrInterval,
                   duration,
                   width,
                   height,
                   customTypes,
                   baseAngle,
                   posVariation,
                   lifeMultiplier,
                   doFadeOut);
        auto* result = gen.get();
        mParticleGenerators.push_back(std::move(gen));
        return result;
    }

    ParticleGenerator*
    Playfield::SpawnParticleGenerator(Game* game,
                                      const std::string& path,
                                      const SDL_FPoint& pos,
                                      nuvelocity::EmitterType type,
                                      nuvelocity::EmitterShape shape,
                                      float rateOrInterval,
                                      float duration,
                                      float width,
                                      float height,
                                      const std::vector<nuvelocity::ParticleType*>* customTypes,
                                      float baseAngle,
                                      float posVariation,
                                      float lifeMultiplier,
                                      bool doFadeOut)
    {
        std::string fullPath = "Resources/Effects/" + path;
        auto info = game->mAsset->LoadParticleGeneratorInfo(fullPath);
        return SpawnParticleGenerator(info,
                                      pos,
                                      type,
                                      shape,
                                      rateOrInterval,
                                      duration,
                                      width,
                                      height,
                                      customTypes,
                                      baseAngle,
                                      posVariation,
                                      lifeMultiplier,
                                      doFadeOut);
    }

    ParticleGenerator* Playfield::SpawnParticleContinuous(Game* game,
                                                          const std::string& path,
                                                          const SDL_FPoint& pos,
                                                          float rate,
                                                          float duration,
                                                          float baseAngle,
                                                          float posVariation,
                                                          float lifeMultiplier,
                                                          bool doFadeOut)
    {
        return SpawnParticleGenerator(game,
                                      path,
                                      pos,
                                      nuvelocity::EmitterType::Continuous,
                                      nuvelocity::EmitterShape::Cone,
                                      rate,
                                      duration,
                                      0.0F,
                                      0.0F,
                                      nullptr,
                                      baseAngle,
                                      posVariation,
                                      lifeMultiplier,
                                      doFadeOut);
    }

    ParticleGenerator* Playfield::SpawnParticlePulse(Game* game,
                                                     const std::string& path,
                                                     const SDL_FPoint& pos,
                                                     float interval,
                                                     float duration,
                                                     float baseAngle,
                                                     float posVariation,
                                                     float lifeMultiplier,
                                                     bool doFadeOut)
    {
        return SpawnParticleGenerator(game,
                                      path,
                                      pos,
                                      nuvelocity::EmitterType::Pulse,
                                      nuvelocity::EmitterShape::Cone,
                                      interval,
                                      duration,
                                      0.0F,
                                      0.0F,
                                      nullptr,
                                      baseAngle,
                                      posVariation,
                                      lifeMultiplier,
                                      doFadeOut);
    }
    void Playfield::ApplyBallSpeedUp(Ball* ball, const SDL_FPoint& hitPos, bool isBrick)
    {
        if (mIsStandAlone || ball->IsTrapped())
        {
            return;
        }
        float dist = std::sqrt(std::pow(ball->GetLastHitPosition().x - hitPos.x, 2) +
                               std::pow(ball->GetLastHitPosition().y - hitPos.y, 2));
        ball->SetLastHitPosition(hitPos);

        float boost = 1.0F; // Display-relative boost

        // 1. repeated hits with small distances (prevents trapping)
        if (dist < 64.0F)
        {
            boost += 0.5F;
        }

        // 2. diagonals (steeper angles / shallow angles get more boost to escape traps)
        SDL_FPoint vel = ball->GetVelocity();
        float angle = std::abs(std::atan2(vel.y, vel.x));
        // Near horizontal (0, PI) or near vertical (PI/2)
        if (angle < 0.25F || angle > (std::numbers::pi_v<float> - 0.25F))
        {
            boost += 0.25F;
        }

        // 3. very small on brick hits
        if (isBrick)
        {
            boost *= 0.15F;
        }

        // Apply scaled to physics (0.5)
        ball->SetSpeed(ball->GetSpeed() + (boost * 0.5F));
    }

    void Playfield::ApplyPowerUp(Game* game, PowerUpType type)
    {
        game->mAudio->PlaySfx(PowerUp::GetSoundPath(type));

        switch (type)
        {
        case PowerUpType::Slow:
            for (auto& b : mBalls)
            {
                b->SetSpeed(std::max(50.0F, b->GetSpeed() - 71.4F));
            }
            break;
        case PowerUpType::Fast:
            for (auto& b : mBalls)
            {
                b->SetSpeed(b->GetSpeed() + 95.2F);
            }
            break;
        case PowerUpType::ExpandPaddle:
            mShip->SetShieldSize(game, mShip->GetShieldSize() + 1);
            break;
        case PowerUpType::ShrinkPaddle:
            mShip->SetShieldSize(game, mShip->GetShieldSize() - 1);
            break;
        case PowerUpType::ExtraBall:
            mIonSpheres++;
            break;
        case PowerUpType::FireBall:
        {
            for (auto& ball : mBalls)
            {
                ball->SetType(BallType::Fire);
                ball->SetIsSmall(false);
            }
            break;
        }
        case PowerUpType::RailBall:
        {
            for (auto& ball : mBalls)
            {
                ball->SetType(BallType::Rail);
                ball->SetIsSmall(false);
            }
            break;
        }
        case PowerUpType::NormalBall:
            for (auto& b : mBalls)
            {
                b->SetType(BallType::Normal);
                b->SetIsSmall(false);
            }
            break;
        case PowerUpType::SmallBall:
            for (auto& b : mBalls)
            {
                b->SetIsSmall(true);
            }
            break;
        case PowerUpType::Gun:
            mShip->SetWeapon(game, WeaponType::Laser);
            break;
        case PowerUpType::BigGun:
            mShip->SetWeapon(game, WeaponType::BigGun);
            break;
        case PowerUpType::Catch:
            mShip->SetCatchMode(true);
            break;
        case PowerUpType::Multiply3:
        {
            if (!mBalls.empty())
            {
                SDL_FPoint pos = mBalls[0]->GetPosition();
                SDL_FPoint vel = mBalls[0]->GetVelocity();
                for (int i = 0; i < 2; ++i)
                {
                    auto nb = std::make_unique<Ball>();
                    nb->SetPlayfield(this);
                    nb->AttachSequence(game);
                    nb->SetPosition(pos);
                    nb->SetVelocity({vel.x + (i == 0 ? 50 : -50), vel.y});
                    AddBall(std::move(nb));
                }
            }
            break;
        }
        case PowerUpType::Multiply8:
        {
            if (!mBalls.empty())
            {
                SDL_FPoint pos = mBalls[0]->GetPosition();
                SDL_FPoint vel = mBalls[0]->GetVelocity();
                for (int i = 0; i < 7; ++i)
                {
                    auto nb = std::make_unique<Ball>();
                    nb->SetPlayfield(this);
                    nb->AttachSequence(game);
                    nb->SetPosition(pos);
                    // Spread them out
                    float angle = (static_cast<float>(i) / 7.0F) * 2.0F * std::numbers::pi_v<float>;
                    float s = std::sqrt(vel.x * vel.x + vel.y * vel.y);
                    nb->SetVelocity({s * std::cos(angle), s * std::sin(angle)});
                    AddBall(std::move(nb));
                }
            }
            break;
        }
        default:
            break;
        }
    }

    void Playfield::DebugDestroyAllBricks(Game* game)
    {
        for (auto& collidable : mCollidables)
        {
            if (auto* brick = dynamic_cast<Brick*>(collidable.get()))
            {
                if (!Brick::IsIndestructibleType(brick->GetInfo()->GetBrickType()))
                {
                    brick->OnHit(game,
                                 mBounds); // This might not destroy it if it has multiple hits
                    // Actually, let's force it
                    while (!brick->IsDestroyed())
                    {
                        brick->OnHit(game, mBounds);
                    }
                }
            }
        }
    }
} // namespace nuvelocity::frs42
