#include "MainMenuScene.h"
#include "ArenaScene.h"
#include "CheckpointButton.h"
#include "Colors.h"
#include "MathUtils.h"
#include "PlayfieldBarrier.h"
#include "RoundSet.h"
#include "StatsManager.h"
#include "windows/LevelSelectWindow.h"
#include "windows/NewGameOptionsWindow.h"
#include "windows/OptionsWindow.h"
#include "windows/SelectPlayerWindow.h"
#include "windows/StatisticsWindow.h"
#include <Game.h>
#include <Image.h>
#include <SDL3/SDL.h>
#include <Sequence.h>
#include <StandAloneFrame.h>
#include <algorithm>
#include <array>
#include <random>
#include <system/AssetManager.h>
#include <system/AudioManager.h>
#include <system/FontManager.h>
#include <system/InputManager.h>
#include <system/SpriteBatch.h>
#include <system/ui/MdiManager.h>
#include <system/ui/skin/JWindowSkin.h>

namespace nuvelocity::frs42
{
    constexpr std::size_t kMenuButtonCount = 5;
    constexpr uint64_t kMenuButtonRevealDelayMs = 150;
    constexpr float kButtonVerticalSpacing = 8;
    constexpr float kButtonRightMargin = 2;
    constexpr SDL_Color kMenuTextColor{.r = 0, .g = 88, .b = 244, .a = 255};
    constexpr int kMenuFontPointSize = MainMenuButton::Style{}.fontPointSize;

    SDL_Point GetWindowSizePixels(Game* game)
    {
        int width = game->mWindowWidth;
        int height = game->mWindowHeight;
        if (game->mWindow != nullptr)
        {
            SDL_GetWindowSizeInPixels(game->mWindow, &width, &height);
        }

        return SDL_Point{.x = width, .y = height};
    }

    void MainMenuScene::Load(Game* game)
    {
        game->mAudio->PlayBgm("Theme.ogg");
        mBarriers.clear();
        mPlayfield.SetIsStandAlone(true);
        mPlayfield.Reset(game);

        mBackgroundImage =
            game->mAsset->LoadStandAloneFrame("Resources/Interface/Main Menu Extreme");

        if (AudioData* bounceSound = game->mAsset->LoadSound("UI/Menu Ball Bounce.ogg"))
        {
            game->mAudio->RegisterSfx(bounceSound);
        }

        mMenuAssets.armNormal =
            game->mAsset->LoadSequence("Resources/Interface/Main Menu Button/Arm Normal");
        mMenuAssets.armHover =
            game->mAsset->LoadSequence("Resources/Interface/Main Menu Button/Arm Hover");
        mMenuAssets.panelFlip =
            game->mAsset->LoadSequence("Resources/Interface/Main Menu Button/Panel Flip");
        mMenuAssets.panelNormal =
            game->mAsset->LoadSequence("Resources/Interface/Main Menu Button/Panel Normal");
        mMenuAssets.panelHover =
            game->mAsset->LoadSequence("Resources/Interface/Main Menu Button/Panel Hover");
        mMenuAssets.panelPressed =
            game->mAsset->LoadSequence("Resources/Interface/Main Menu Button/Panel Pressed");

        // Initialization helper for each button
        auto initButton = [&](MainMenuButton& button, const char* caption, auto clickHandler)
        {
            button.SetAssets(mMenuAssets);
            button.SetCaption(caption);
            button.SetStyle(MainMenuButton::Style{.textColor = kMenuTextColor,
                                                  .fontPointSize = kMenuFontPointSize});
            button.SetOnClick(std::bind(clickHandler, this, std::placeholders::_1));
        };

        initButton(mPlayButton, "&Play Game", &MainMenuScene::OnPlayClick);
        initButton(mStatsButton, "&Statistics", &MainMenuScene::OnStatsClick);
        initButton(mFriendButton, "&Tell A Friend", &MainMenuScene::OnFriendClick);
        initButton(mOptionsButton, "&Options", &MainMenuScene::OnOptionsClick);
        initButton(mExitButton, "E&xit Game", &MainMenuScene::OnExitClick);

        mMenuButtonPointers[0] = &mPlayButton;
        mMenuButtonPointers[1] = &mStatsButton;
        mMenuButtonPointers[2] = &mFriendButton;
        mMenuButtonPointers[3] = &mOptionsButton;
        mMenuButtonPointers[4] = &mExitButton;

        mFocusContainer = std::make_unique<nuvelocity::FocusContainer>(mMenuButtonPointers.data(),
                                                                       mMenuButtonPointers.size());

        SDL_Point windowSize = GetWindowSizePixels(game);
        const SDL_Point buttonSize = mPlayButton.GetSize();
        if (buttonSize.x <= 0 || buttonSize.y <= 0)
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                        "Main menu arm logical size is unavailable; menu buttons disabled.");
            return;
        }

        const int startY = 30;
        const int buttonX = windowSize.x - buttonSize.x - static_cast<int>(kButtonRightMargin);

        const uint64_t nowTick = SDL_GetTicks();
        for (std::size_t index = 0; index < mMenuButtonPointers.size(); ++index)
        {
            MainMenuButton* button = static_cast<MainMenuButton*>(mMenuButtonPointers[index]);
            const SDL_Rect buttonRect{
                .x = buttonX,
                .y = startY + (static_cast<int>(index) *
                               (buttonSize.y + static_cast<int>(kButtonVerticalSpacing))),
                .w = buttonSize.x,
                .h = buttonSize.y};
            button->SetRect(buttonRect);
            button->ResetAnimation(nowTick, kMenuButtonRevealDelayMs * index);
        }

        // Geography and spawn constants (shared between barriers and spawning logic)
        static constexpr SDL_FPoint kSpawnPolygon[] = {{.x = 49, .y = 51},
                                                       {.x = 49, .y = 414},
                                                       {.x = 130, .y = 414},
                                                       {.x = 130, .y = 260},
                                                       {.x = 197, .y = 260},
                                                       {.x = 197, .y = 414},
                                                       {.x = 275, .y = 414},
                                                       {.x = 275, .y = 275},
                                                       {.x = 213, .y = 232},
                                                       {.x = 275, .y = 190},
                                                       {.x = 275, .y = 100},
                                                       {.x = 214, .y = 51}};
        constexpr int kSpawnPolyCount = static_cast<int>(SDL_arraysize(kSpawnPolygon));
        constexpr SDL_FRect kInnerBox = {.x = 129, .y = 107, .w = 71, .h = 93};
        const std::vector<SDL_FRect> excludeRects = {kInnerBox};

        // Initialize Playfield barriers — outer playfield boundary + inner button-area box
        {
            auto barrier = std::make_unique<PlayfieldBarrier>(
                std::vector<SDL_FPoint>(kSpawnPolygon, kSpawnPolygon + kSpawnPolyCount));
            barrier->SetPosition({.x = 0.0F, .y = 0.0F});
            barrier->SetHoverColor({.r = 120, .g = 124, .b = 120, .a = 255}); // #787c78
            barrier->SetAttractionEnabled(true);
            mBarriers.push_back(barrier.get());
            mPlayfield.AddCollidable(std::move(barrier));
        }

        {
            // Inner box derived from kInnerBox
            auto barrier = std::make_unique<PlayfieldBarrier>(std::vector<SDL_FPoint>{
                {.x = kInnerBox.x, .y = kInnerBox.y},
                {.x = kInnerBox.x + kInnerBox.w, .y = kInnerBox.y},
                {.x = kInnerBox.x + kInnerBox.w, .y = kInnerBox.y + kInnerBox.h},
                {.x = kInnerBox.x, .y = kInnerBox.y + kInnerBox.h}});
            barrier->SetPosition({.x = 0.0F, .y = 0.0F});
            barrier->SetShowHoverEffect(false);
            mBarriers.push_back(barrier.get());
            mPlayfield.AddCollidable(std::move(barrier));
        }

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> disV(-150.0F, 150.0F);

        const SDL_FRect spawnBounds{
            .x = 50.0F, .y = 52.0F, .w = 224.0F, .h = 361.0F}; // Covers (50,52) to (274,413)

        auto* menuBallSequence = game->mAsset->LoadSequence("Resources/Ball/Menu Ball");

        for (int i = 0; i < 8; ++i)
        {
            auto ball = std::make_unique<Ball>();
            ball->AttachSequence(game, menuBallSequence);
            const float ballRadius = ball->GetRadius();

            SDL_FPoint spawnPos =
                MathUtils::GetRandomPointInPolygon(
                    gen, kSpawnPolygon, kSpawnPolyCount, spawnBounds, ballRadius, excludeRects)
                    .value_or(SDL_FPoint{.x = 130.0F, .y = 276.0F});

            ball->SetPosition(spawnPos);
            ball->SetVelocity({.x = disV(gen), .y = disV(gen)});
            mPlayfield.AddBall(std::move(ball));
        }

        mEntryFadeStartTick = nowTick;
    }

    void MainMenuScene::Update(Game* game)
    {
        if (mFocusContainer == nullptr || game->mInput == nullptr)
        {
            return;
        }

        if (!mHideMenuButtons)
        {
            mFocusContainer->Update(game);
        }
        UpdatePlayfield(game);
    }

    void MainMenuScene::UpdatePlayfield(Game* game)
    {
        // Apply gravity-like follow behavior if any barrier is hovered
        const SDL_Point mousePosition = game->mInput->GetMousePosition();
        bool anyBarrierHovered = false;
        for (auto* barrier : mBarriers)
        {
            if (barrier->IsHovered())
            {
                anyBarrierHovered = true;
                break;
            }
        }

        if (anyBarrierHovered)
        {
            for (auto* barrier : mBarriers)
            {
                if (barrier->IsAttractionEnabled())
                {
                    for (const auto& ball : mPlayfield.GetBalls())
                    {
                        barrier->ApplyAttraction(game,
                                                 ball.get(),
                                                 {.x = static_cast<float>(mousePosition.x),
                                                  .y = static_cast<float>(mousePosition.y)});
                    }
                }
            }
        }

        mPlayfield.Update(game);
    }

    void MainMenuScene::Draw(Game* game)
    {
        if (game == nullptr || game->mSpriteBatch == nullptr)
        {
            return;
        }

        game->mSpriteBatch->Clear(SDL_Color{.r = 0, .g = 0, .b = 0, .a = SDL_ALPHA_OPAQUE});

        if (mBackgroundImage != nullptr)
        {
            game->mSpriteBatch->DrawCentered(mBackgroundImage);
        }

        mPlayfield.Draw(game);

        if (!mHideMenuButtons)
        {
            mFocusContainer->Draw(game);
        }

        if (mEntryFadeStartTick != 0)
        {
            const uint64_t nowTick = SDL_GetTicks();
            const uint64_t elapsedMs = nowTick - mEntryFadeStartTick;
            const uint64_t clampedElapsedMs = SDL_min(elapsedMs, kEntryFadeDurationMs);
            const float progress =
                static_cast<float>(clampedElapsedMs) / static_cast<float>(kEntryFadeDurationMs);
            const uint8_t overlayAlpha =
                static_cast<uint8_t>((1.0F - progress) * static_cast<float>(SDL_ALPHA_OPAQUE));

            if (overlayAlpha > SDL_ALPHA_TRANSPARENT)
            {
                game->mSpriteBatch->FillRect(nullptr,
                                             SDL_Color{.r = 0, .g = 0, .b = 0, .a = overlayAlpha});
            }
        }

        std::string versionFont = "Small Blue";
        std::string versionText = "1.01 Build 50";
        int versionTextWidth = 0;
        int versionTextHeight = 0;
        game->mFont->MeasureStringWithFont(
            versionFont, versionText, 8, versionTextWidth, versionTextHeight);

        game->mFont->DrawStringWithFontAt(versionFont,
                                          game->mSpriteBatch,
                                          versionText,
                                          game->mWindowWidth - versionTextWidth,
                                          game->mWindowHeight - versionTextHeight - 12,
                                          Colors::White,
                                          8);
    }

    std::string MainMenuScene::GetName() const
    {
        return "MainMenuScene";
    }

    void MainMenuScene::OnPlayClick(Game* game)
    {
        if (game == nullptr || game->mMdi == nullptr)
        {
            return;
        }

        mHideMenuButtons = true;
        auto playWindow = std::make_shared<SelectPlayerWindow>(game);
        playWindow->SetOnClose([this](nuvelocity::MdiWindow& window)
                               { this->mHideMenuButtons = false; });
        game->mMdi->AddCenteredWindow(game, playWindow);
    }

    void MainMenuScene::OnStatsClick(Game* game)
    {
        if (game == nullptr || game->mMdi == nullptr)
        {
            return;
        }

        mHideMenuButtons = true;
        auto statsWindow = std::make_shared<StatisticsWindow>(game);
        statsWindow->SetOnClose([this](nuvelocity::MdiWindow& window)
                                { this->mHideMenuButtons = false; });
        game->mMdi->AddWindow(statsWindow);
    }

    void MainMenuScene::OnFriendClick(Game* game)
    {
        (void)game;
    }

    void MainMenuScene::OnOptionsClick(Game* game)
    {
        if (game == nullptr || game->mMdi == nullptr)
        {
            return;
        }

        mHideMenuButtons = true;
        auto optionsWindow = std::make_shared<OptionsWindow>(game);
        optionsWindow->SetOnClose([this](nuvelocity::MdiWindow& window)
                                  { this->mHideMenuButtons = false; });
        game->mMdi->AddCenteredWindow(game, optionsWindow);
    }

    void MainMenuScene::OnExitClick(Game* game)
    {
        SDL_Event quit_event;
        quit_event.type = SDL_EVENT_QUIT;
        SDL_PushEvent(&quit_event);
    }
} // namespace nuvelocity::frs42
