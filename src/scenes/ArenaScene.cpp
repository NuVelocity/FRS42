#include "ArenaScene.h"
#include "Brick.h"
#include "BrickInfo.h"
#include "BrickLayout.h"
#include "Font.h"
#include "MainMenuScene.h"
#include "PlayfieldBarrier.h"
#include "windows/OptionsWindow.h"
#include "windows/PauseWindow.h"
#include <Colors.h>
#include <Game.h>
#include <SDL3/SDL.h>
#include <Sequence.h>
#include <StandAloneFrame.h>
#include <system/AssetManager.h>
#include <system/AudioManager.h>
#include <system/FontManager.h>
#include <system/InputManager.h>
#include <system/SpriteBatch.h>
#include <system/ui/MdiManager.h>
#include <system/ui/MdiWindow.h>

#include <random>
#include <sstream>
#include <string>
#include <utility>

namespace nuvelocity::frs42
{
    static std::random_device gRd;
    static std::mt19937 gGen(gRd());

    ArenaScene::ArenaScene(std::string roundSetName, int roundIndex)
            : mRoundSetName(std::move(roundSetName))
            , mRoundIndex(static_cast<uint8_t>(roundIndex))
            , mPlayfieldRect({.x = 10, .y = 10, .w = 493, .h = 470})
            , mGameOverFrame(nullptr)
    {
    }

    void ArenaScene::Load(Game* game)
    {
        if (!LoadRoundSet(game))
        {
            return;
        }

        BrickLayout* layout = LoadBrickLayout(game);
        if (layout == nullptr)
        {
            return;
        }

        mMegovision.Load(game);

        mGameOverFrame = game->mAsset->LoadStandAloneFrame("Resources/Interface/Game Over");
        mPlayfield.SetGameOverFrame(mGameOverFrame);
        mPlayfield.Reset(game);
        mPlayfield.LoadBackground(game, layout->GetBackgroundType());
        mPlayfield.SetPowerUpWeights(layout->GetPowerUpWeights());
        mPlayfield.SetMegovision(&mMegovision);
        mPlayfield.SetBounds(mPlayfieldRect);
        mPlayfield.SetRoundName(layout->GetDisplayName());

        // FIXME: power-ups should not be hardcoded here.
        std::vector<std::string> sfxToRegister = {"Lost Ball.ogg",
                                                  "Game Over.ogg",
                                                  "Timeout.ogg",
                                                  "Start Round.ogg",
                                                  "Ball In Play.ogg",
                                                  "Bounce.ogg",
                                                  "Score.ogg",
                                                  "Ship EXPLODE.ogg",
                                                  "Level Ending Tune.ogg",
                                                  "Power Ups/Bomb Dropped.ogg",
                                                  "Power Ups/Slow.ogg",
                                                  "Power Ups/Snap On Larger Shiled.ogg",
                                                  "Power Ups/Catcher.ogg",
                                                  "Power Ups/Fire Ball.ogg",
                                                  "Power Ups/Multiply 3.ogg",
                                                  "Power Ups/Multiply 8.ogg",
                                                  "Power Ups/Gun Deploy.ogg",
                                                  "Power Ups/Big Gun.ogg",
                                                  "Power Ups/Extra Ball.ogg",
                                                  "Power Ups/Rail Ball.ogg",
                                                  "Power Ups/Normal Ball.ogg",
                                                  "Power Ups/Small Ball.ogg",
                                                  "Power Ups/Snap On Smaller Shiled.ogg",
                                                  "Power Ups/Fast.ogg"};

        for (const auto& path : sfxToRegister)
        {
            if (auto* sfx = game->mAsset->LoadSound(path))
            {
                game->mAudio->RegisterSfx(sfx);
            }
        }

        PopulateBricks(game, layout);
        BuildLevelUI(game, layout);

        game->mAudio->PlaySfx("Start Round.ogg");
        game->mAudio->PlaySfx("Ball In Play.ogg");
    }

    bool ArenaScene::LoadRoundSet(Game* game)
    {
        mRoundSet = static_cast<RoundSet*>(
            game->mAsset->LoadPropertyFile("Resources/Rounds/" + mRoundSetName + ".RoundSet"));
        if (mRoundSet == nullptr)
        {
            return false;
        }

        const auto& rounds = mRoundSet->GetRoundList();
        return mRoundIndex < rounds.size();
    }

    BrickLayout* ArenaScene::LoadBrickLayout(Game* game)
    {
        const auto& rounds = mRoundSet->GetRoundList();
        std::string roundPath = "Resources/" + rounds[mRoundIndex] + ".Ricochet";
        SDL_Log("Loading round layout: %s", roundPath.c_str());
        return static_cast<BrickLayout*>(game->mAsset->LoadPropertyFile(roundPath));
    }

    void ArenaScene::PopulateBricks(Game* game, BrickLayout* layout)
    {
        // Populate bricks from grid
        Frame* frame = layout->GetLayout();
        const auto& palette = layout->GetBrickPalette();
        SDL_Log(
            "Layout frame: %p, Palette size: %zu", reinterpret_cast<void*>(frame), palette.size());

        int bricksLoaded = 0;
        if (frame != nullptr)
        {
            int rows = frame->GetHeight();
            int cols = frame->GetWidth();
            SDL_Log("Grid dimensions: %dx%d", cols, rows);

            for (int y = 0; y < rows; ++y)
            {
                for (int x = 0; x < cols; ++x)
                {
                    uint32_t pixel = frame->GetPixel(x, y);
                    uint8_t index = pixel & 0xFF;
                    if (index > 0 && index < palette.size())
                    {
                        std::string brickPath = "Resources/" + palette[index];
                        BrickInfo* info = GetOrLoadBrickInfo(game, brickPath);
                        if (info != nullptr)
                        {
                            if (info->GetBrickType() == BrickType::TrappedBall)
                            {
                                auto ball = std::make_unique<Ball>();
                                ball->AttachSequence(game);
                                if (info->GetPrimarySequencePath() != "!None")
                                {
                                    ball->SetTrappedSequence(game->mAsset->LoadSequence(
                                        "Resources/" + info->GetPrimarySequencePath()));
                                }
                                ball->SetPosition(
                                    {.x = static_cast<float>(mPlayfieldRect.x + 4 + x * 32),
                                     .y = static_cast<float>(mPlayfieldRect.y + y * 18)});
                                ball->SetIsTrapped(true);
                                ball->SetPlayfield(&mPlayfield);

                                std::uniform_real_distribution<float> angleDis(0, 2.0F * 3.14159F);
                                float angle = angleDis(gGen);
                                ball->SetVelocity(
                                    {std::cos(angle) * 200.0F, std::sin(angle) * 200.0F});

                                mPlayfield.AddBall(std::move(ball));
                            }
                            else
                            {
                                auto brick = std::make_unique<Brick>();
                                brick->AttachBrickInfo(game, info);
                                brick->SetPosition(
                                    {.x = static_cast<float>(mPlayfieldRect.x + 4 + x * 32),
                                     .y = static_cast<float>(mPlayfieldRect.y + y * 18)});

                                brick->SetAnimationStartTick(SDL_GetTicks() - (gGen() % 5000));

                                brick->SetPlayfield(&mPlayfield);
                                mPlayfield.AddCollidable(std::move(brick));
                                bricksLoaded++;
                            }
                        }
                    }
                }
            }
        }
        SDL_Log("Bricks loaded from grid: %d", bricksLoaded);

        // Populate floating bricks
        int floatingBricksLoaded = 0;

        for (auto* floatingBrick : layout->GetFloatingBricks())
        {
            int pIdx = floatingBrick->GetPaletteIndex();
            if (pIdx >= 0 && std::cmp_less(pIdx, palette.size()))
            {
                std::string brickPath = "Resources/" + palette[pIdx];
                BrickInfo* info = GetOrLoadBrickInfo(game, brickPath);
                if (info != nullptr)
                {
                    if (info->GetBrickType() == BrickType::TrappedBall)
                    {
                        auto ball = std::make_unique<Ball>();
                        ball->AttachSequence(game);
                        if (info->GetPrimarySequencePath() != "!None")
                        {
                            ball->SetTrappedSequence(game->mAsset->LoadSequence(
                                "Resources/" + info->GetPrimarySequencePath()));
                        }
                        ball->SetPosition(
                            {.x = static_cast<float>(mPlayfieldRect.x + 4 + floatingBrick->GetX()),
                             .y = static_cast<float>(mPlayfieldRect.y + floatingBrick->GetY())});
                        ball->SetIsTrapped(true);
                        ball->SetPlayfield(&mPlayfield);

                        std::uniform_real_distribution<float> angleDis(0, 2.0F * 3.14159F);
                        float angle = angleDis(gGen);
                        ball->SetVelocity({std::cos(angle) * 200.0F, std::sin(angle) * 200.0F});

                        mPlayfield.AddBall(std::move(ball));
                    }
                    else
                    {
                        auto brick = std::make_unique<Brick>();
                        brick->AttachBrickInfo(game, info);
                        brick->SetPosition(
                            {.x = static_cast<float>(mPlayfieldRect.x + floatingBrick->GetX() + 4),
                             .y = static_cast<float>(mPlayfieldRect.y + floatingBrick->GetY())});
                        brick->SetAnimationStartTick(SDL_GetTicks() - (gGen() % 5000));
                        brick->SetPlayfield(&mPlayfield);
                        mPlayfield.AddCollidable(std::move(brick));
                        floatingBricksLoaded++;
                    }
                }
            }
        }
        SDL_Log("Floating bricks loaded: %d", floatingBricksLoaded);
    }

    void ArenaScene::BuildLevelUI(Game* game, BrickLayout* layout)
    {
        int roundSetNumber = 1;
        size_t firstDigit = mRoundSetName.find_first_of("0123456789");
        if (firstDigit != std::string::npos)
        {
            roundSetNumber = std::atoi(mRoundSetName.c_str() + firstDigit);
        }
        int roundNumber = mRoundIndex + 1;

        const std::string& roundTitle = layout->GetDisplayName();
        char tickerBuf[256];
        SDL_snprintf(tickerBuf,
                     sizeof(tickerBuf),
                     "Round %d-%d: %s",
                     roundSetNumber,
                     roundNumber,
                     roundTitle.c_str());
        mMegovision.SetTickerText(tickerBuf, game);

        // Megovision round message
        std::vector<std::unique_ptr<Label>> megoLabels;
        char roundBuf[32];
        SDL_snprintf(roundBuf, sizeof(roundBuf), "Round %d-%d", roundSetNumber, roundNumber);
        megoLabels.push_back(std::make_unique<Label>(roundBuf, "Megovision"));

        // Word wrap round title for line 2+ (90px width)
        const std::string& title = layout->GetDisplayName();
        std::stringstream ss(title);
        std::string word;
        std::string currentLine;
        while (ss >> word)
        {
            std::string testLine = currentLine.empty() ? word : currentLine + " " + word;
            int w = 0;
            int h = 0;
            game->mFont->MeasureStringWithFont("Small Blue", testLine, 8, w, h);
            if (w > 90 && !currentLine.empty())
            {
                megoLabels.push_back(std::make_unique<Label>(currentLine, "Small Blue"));
                currentLine = word;
            }
            else
            {
                currentLine = testLine;
            }
        }
        if (!currentLine.empty())
        {
            megoLabels.push_back(std::make_unique<Label>(currentLine, "Small Blue"));
        }
        mMegovision.ShowMessage(std::move(megoLabels), 0.0F, false);
    }

    void ArenaScene::Unload(Game* game)
    {
        (void)game;
    }

    void ArenaScene::Update(Game* game)
    {
        const bool escapePressed = game->mInput->IsKeyPressed(SDL_SCANCODE_ESCAPE);
        const bool rightPressed = game->mInput->IsMouseButtonPressed(SDL_BUTTON_RIGHT);

        if (game->mMdi->GetWindows().empty() && (escapePressed || rightPressed))
        {
            if (escapePressed)
            {
                game->mInput->ConsumeKey(SDL_SCANCODE_ESCAPE);
            }
            if (rightPressed)
            {
                game->mInput->ConsumeMouseButton(SDL_BUTTON_RIGHT);
            }
            ShowPauseMenu(game);
            return;
        }

        // Handle cheats
        for (const auto& event : game->mInput->GetFrameEvents())
        {
            if (event.type == SDL_EVENT_KEY_DOWN)
            {
                SDL_Keycode key = event.key.key;

                if (!mIsPaused && key >= SDLK_A && key <= SDLK_Z)
                {
                    mCheatBuffer += static_cast<char>('a' + (key - SDLK_A));
                    if (mCheatBuffer.length() > 32)
                    {
                        mCheatBuffer.erase(0, 1);
                    }

                    if (mCheatBuffer.find("flipad") != std::string::npos)
                    {
                        mMegovision.ForceAdTransition();
                        mCheatBuffer.clear();
                    }
                    else if (mCheatBuffer.find("cheatnext") != std::string::npos)
                    {
                        mRoundIndex++;
                        Load(game);
                        mCheatBuffer.clear();
                        return; // Load re-initialized everything, stop current update
                    }
                    else if (mCheatBuffer.find("cheatmouse") != std::string::npos)
                    {
                        mPlayfield.ToggleMouseBallControl();
                        mCheatBuffer.clear();
                    }
                    else if (mCheatBuffer.find("cheatsmall") != std::string::npos)
                    {
                        mPlayfield.ApplyPowerUp(game, PowerUpType::SmallBall);
                        mCheatBuffer.clear();
                    }
                    else if (mCheatBuffer.find("cheatnormal") != std::string::npos)
                    {
                        mPlayfield.ApplyPowerUp(game, PowerUpType::NormalBall);
                        mCheatBuffer.clear();
                    }
                    else if (mCheatBuffer.find("cheatsplit") != std::string::npos)
                    {
                        mPlayfield.ApplyPowerUp(game, PowerUpType::Multiply8);
                        mCheatBuffer.clear();
                    }
                    else if (mCheatBuffer.find("cheatfire") != std::string::npos)
                    {
                        mPlayfield.ApplyPowerUp(game, PowerUpType::FireBall);
                        mCheatBuffer.clear();
                    }
                    else if (mCheatBuffer.find("cheatrail") != std::string::npos)
                    {
                        mPlayfield.ApplyPowerUp(game, PowerUpType::RailBall);
                        mCheatBuffer.clear();
                    }
                    else if (mCheatBuffer.find("cheatgun") != std::string::npos)
                    {
                        mPlayfield.ApplyPowerUp(game, PowerUpType::Gun);
                        mCheatBuffer.clear();
                    }
                    else if (mCheatBuffer.find("cheatmiss") != std::string::npos)
                    {
                        mPlayfield.ApplyPowerUp(game, PowerUpType::BigGun);
                        mCheatBuffer.clear();
                    }
                    else if (mCheatBuffer.find("cheatcatch") != std::string::npos)
                    {
                        mPlayfield.ApplyPowerUp(game, PowerUpType::Catch);
                        mCheatBuffer.clear();
                    }
                    else if (mCheatBuffer.find("cheatgrow") != std::string::npos ||
                             mCheatBuffer.find("cheatexpand") != std::string::npos)
                    {
                        mPlayfield.ApplyPowerUp(game, PowerUpType::ExpandPaddle);
                        mCheatBuffer.clear();
                    }
                    else if (mCheatBuffer.find("cheatshrink") != std::string::npos)
                    {
                        mPlayfield.ApplyPowerUp(game, PowerUpType::ShrinkPaddle);
                        mCheatBuffer.clear();
                    }
                    else if (mCheatBuffer.find("debugfinishlevel") != std::string::npos)
                    {
                        mPlayfield.DebugDestroyAllBricks(game);
                        mCheatBuffer.clear();
                    }
                }
                else if (key == SDLK_BACKSPACE)
                {
                    mCheatBuffer.clear();
                }
            }
        }

        if (!mIsPaused)
        {
            mPlayfield.Update(game);
            if (mPlayfield.IsCompletionSequenceFinished())
            {
                mRoundIndex++;
                Load(game);
                return;
            }
        }
        mMegovision.Update(game);
    }

    void ArenaScene::Draw(Game* game)
    {
        mPlayfield.Draw(game);
        mMegovision.Draw(game);
    }

    BrickInfo* ArenaScene::GetOrLoadBrickInfo(Game* game, const std::string& path)
    {
        BrickInfo* info = static_cast<BrickInfo*>(game->mAsset->LoadBrickInfo(path));
        if (info != nullptr)
        {
            // Ensure particle gen and types are loaded/resolved once per BrickInfo
            if (info->GetBreakParticleGen() == nullptr &&
                info->GetBreakParticleGenPath() != "!None")
            {
                info->SetBreakParticleGen(game->mAsset->LoadParticleGeneratorInfo(
                    "Resources/Effects/" + info->GetBreakParticleGenPath()));

                for (auto* pt : info->GetBreakParticleTypes())
                {
                    pt->SetSequence(game->mAsset->ResolveParticleSequence(pt->GetParticleType()));
                }
            }
        }
        return info;
    }

    void ArenaScene::ShowPauseMenu(Game* game)
    {
        if (mIsPaused || game == nullptr || game->mMdi == nullptr)
        {
            return;
        }

        auto pauseWindow = std::make_shared<PauseWindow>(game);
        pauseWindow->SetOnClose(
            [this](nuvelocity::MdiWindow& window)
            {
                this->mIsPaused = false;
                this->mPlayfield.SetSuspended(false);
                this->mMegovision.ShowMessage(
                    std::vector<std::string>{}, 0.0F, false); // Clear message
            });

        game->mMdi->AddCenteredWindow(game, pauseWindow);

        game->mAudio->PlaySfx("Timeout.ogg");

        mIsPaused = true;
        mPlayfield.SetSuspended(true);

        // Show Pause Message on Megovision
        std::vector<std::unique_ptr<nuvelocity::Label>> labels;
        labels.push_back(std::make_unique<nuvelocity::Label>("Timeout", "Megovision"));
        labels.push_back(std::make_unique<nuvelocity::Label>("Click to continue", "Small Blue"));
        mMegovision.ShowMessage(std::move(labels), 1.75F, true);
    }

    void ArenaScene::ShowOptionsDialog(Game* game)
    {
        if (game == nullptr || game->mMdi == nullptr)
        {
            return;
        }

        game->mMdi->AddCenteredWindow(game, std::make_shared<OptionsWindow>(game));
    }
} // namespace nuvelocity::frs42
