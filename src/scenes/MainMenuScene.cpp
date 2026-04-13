#include <Game.h>
#include <Image.h>
#include <SDL3/SDL.h>
#include <system/SpriteBatch.h>

#include <array>
#include <limits>
#include <random>

#include "BrickInfo.h"
#include "MainMenuScene.h"
#include "MathUtils.h"

namespace nuvelocity::frs42
{
    constexpr std::size_t kMenuButtonCount = 5;
    constexpr uint64_t kMenuButtonRevealDelayMs = 150;
    constexpr float kButtonVerticalSpacing = 8;
    constexpr float kButtonRightMargin = 0;
    constexpr SDL_Color kMenuTextColor{.r = 0, .g = 88, .b = 244, .a = 255};
    constexpr int kMenuFontPointSize = MainMenuButton::Style{}.fontPointSize;

    SDL_FPoint GetWindowSizePixels(Game* aGame)
    {
        int width = aGame->mWindowWidth;
        int height = aGame->mWindowHeight;
        if (aGame->mWindow != nullptr)
        {
            SDL_GetWindowSizeInPixels(aGame->mWindow, &width, &height);
        }

        return SDL_FPoint{.x = static_cast<float>(width), .y = static_cast<float>(height)};
    }

    void MainMenuScene::Load(Game* aGame)
    {
        aGame->mAudio->PlayBgm("Rock Fast");

        auto* backgroundFrame =
            aGame->mAsset->LoadStandAloneFrame("Resources/Interface/Main Menu Extreme");
        if (backgroundFrame != nullptr)
        {
            mBackgroundImage = Image(*backgroundFrame);
        }

        mMenuAssets.armNormal =
            aGame->mAsset->LoadSequence("Resources/Interface/Main Menu Button/Arm Normal");
        mMenuAssets.armHover =
            aGame->mAsset->LoadSequence("Resources/Interface/Main Menu Button/Arm Hover");
        mMenuAssets.panelFlip =
            aGame->mAsset->LoadSequence("Resources/Interface/Main Menu Button/Panel Flip");
        mMenuAssets.panelNormal =
            aGame->mAsset->LoadSequence("Resources/Interface/Main Menu Button/Panel Normal");
        mMenuAssets.panelHover =
            aGame->mAsset->LoadSequence("Resources/Interface/Main Menu Button/Panel Hover");
        mMenuAssets.panelPressed =
            aGame->mAsset->LoadSequence("Resources/Interface/Main Menu Button/Panel Pressed");

        // Initialization helper for each button
        auto initButton = [&](MainMenuButton& button, const char* caption, auto clickHandler)
        {
            button.SetAssets(mMenuAssets);
            button.SetCaption(caption);
            button.SetStyle(MainMenuButton::Style{.textColor = kMenuTextColor,
                                                  .fontPointSize = kMenuFontPointSize});
            button.SetOnClick(std::bind(clickHandler, this));
        };

        initButton(mPlayButton, "_Play Game", &MainMenuScene::OnPlayClick);
        initButton(mStatsButton, "_Statistics", &MainMenuScene::OnStatsClick);
        initButton(mFriendButton, "_Tell A Friend", &MainMenuScene::OnFriendClick);
        initButton(mOptionsButton, "_Options", &MainMenuScene::OnOptionsClick);
        initButton(mExitButton, "E_xit Game", &MainMenuScene::OnExitClick);

        mMenuButtonPointers[0] = &mPlayButton;
        mMenuButtonPointers[1] = &mStatsButton;
        mMenuButtonPointers[2] = &mFriendButton;
        mMenuButtonPointers[3] = &mOptionsButton;
        mMenuButtonPointers[4] = &mExitButton;

        mFocusContainer = std::make_unique<nuvelocity::FocusContainer>(mMenuButtonPointers.data(),
                                                                       mMenuButtonPointers.size());

        SDL_FPoint windowSize = GetWindowSizePixels(aGame);
        const SDL_FPoint buttonSize = mPlayButton.GetSize();
        if (buttonSize.x <= 0.0F || buttonSize.y <= 0.0F)
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                        "Main menu arm logical size is unavailable; menu buttons disabled.");
            return;
        }

        const float startY = 30;
        const float buttonX = windowSize.x - buttonSize.x - kButtonRightMargin;

        const uint64_t nowTick = SDL_GetTicks();
        for (std::size_t index = 0; index < mMenuButtonPointers.size(); ++index)
        {
            MainMenuButton* button = static_cast<MainMenuButton*>(mMenuButtonPointers[index]);
            const SDL_FRect buttonRect{
                .x = buttonX,
                .y = startY + (static_cast<float>(index) * (buttonSize.y + kButtonVerticalSpacing)),
                .w = 0.0F,
                .h = 0.0F};
            button->SetBounds(buttonRect);
            button->ResetAnimation(nowTick, kMenuButtonRevealDelayMs * index);
        }

        // Geography and spawn constants (shared between barriers and spawning logic)
        static constexpr SDL_FPoint kSpawnPolygon[] = {{49, 51},
                                                       {49, 414},
                                                       {130, 414},
                                                       {130, 260},
                                                       {197, 260},
                                                       {197, 414},
                                                       {275, 414},
                                                       {275, 275},
                                                       {213, 232},
                                                       {275, 190},
                                                       {275, 100},
                                                       {214, 51}};
        constexpr int kSpawnPolyCount = static_cast<int>(SDL_arraysize(kSpawnPolygon));
        constexpr SDL_FRect kInnerBox = {129, 107, 71, 93};
        const std::vector<SDL_FRect> excludeRects = {kInnerBox};

        // Initialize GameBoard barriers — outer playfield boundary + inner button-area box
        BrickInfo barrierInfo;

        {
            auto barrier = std::make_unique<BarrierBrick>(
                std::vector<SDL_FPoint>(kSpawnPolygon, kSpawnPolygon + kSpawnPolyCount));
            barrier->AttachBrickInfo(aGame, barrierInfo);
            barrier->SetPosition({0.0f, 0.0f});
            barrier->SetHoverColor({120, 124, 120, 255}); // #787c78
            barrier->SetAttractionEnabled(true);
            mBarriers.push_back(barrier.get());
            mGameBoard.AddBrick(std::move(barrier));
        }

        {
            // Inner box derived from kInnerBox
            auto barrier = std::make_unique<BarrierBrick>(
                std::vector<SDL_FPoint>{{kInnerBox.x, kInnerBox.y},
                                        {kInnerBox.x + kInnerBox.w, kInnerBox.y},
                                        {kInnerBox.x + kInnerBox.w, kInnerBox.y + kInnerBox.h},
                                        {kInnerBox.x, kInnerBox.y + kInnerBox.h}});
            barrier->AttachBrickInfo(aGame, barrierInfo);
            barrier->SetPosition({0.0f, 0.0f});
            barrier->SetShowHoverEffect(false);
            mBarriers.push_back(barrier.get());
            mGameBoard.AddBrick(std::move(barrier));
        }

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> disV(-150.0f, 150.0f);

        const SDL_FRect spawnBounds{50.0f, 52.0f, 224.0f, 361.0f}; // Covers (50,52) to (274,413)

        auto* menuBallSequence = aGame->mAsset->LoadSequence("Resources/Ball/Menu Ball");

        for (int i = 0; i < 8; ++i)
        {
            auto ball = std::make_unique<Ball>();
            ball->AttachSequence(aGame, menuBallSequence);
            const float ballRadius = ball->GetRadius();

            SDL_FPoint spawnPos =
                MathUtils::GetRandomPointInPolygon(
                    gen, kSpawnPolygon, kSpawnPolyCount, spawnBounds, ballRadius, excludeRects)
                    .value_or(SDL_FPoint{130.0f, 276.0f});

            ball->SetPosition(spawnPos);
            ball->SetVelocity({disV(gen), disV(gen)});
            mGameBoard.AddBall(std::move(ball));
        }

        mEntryFadeStartTick = nowTick;
    }

    void MainMenuScene::Update(Game* aGame)
    {
        if (mFocusContainer == nullptr || aGame->mInput == nullptr)
        {
            return;
        }

        mFocusContainer->Update(aGame);
        UpdateGameBoard(aGame);
    }

    void MainMenuScene::UpdateGameBoard(nuvelocity::Game* aGame)
    {
        // Apply gravity-like follow behavior if any barrier is hovered
        const SDL_FPoint mousePosition = aGame->mInput->GetMousePosition();
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
                    for (auto& ball : mGameBoard.GetBalls())
                    {
                        barrier->ApplyAttraction(aGame, ball.get(), mousePosition);
                    }
                }
            }
        }

        mGameBoard.Update(aGame);
    }

    void MainMenuScene::Draw(Game* aGame)
    {
        if (aGame == nullptr || aGame->mSpriteBatch == nullptr)
        {
            return;
        }

        aGame->mSpriteBatch->Clear(SDL_Color{0, 0, 0, SDL_ALPHA_OPAQUE});

        if (mBackgroundImage.IsValid())
        {
            aGame->mSpriteBatch->DrawCentered(mBackgroundImage.GetSurface());
        }

        mGameBoard.Draw(aGame);

        mFocusContainer->Draw(aGame);

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
                aGame->mSpriteBatch->FillRect(nullptr, SDL_Color{0, 0, 0, overlayAlpha});
            }
        }

        std::string versionFont = "Small Blue";
        std::string versionText = "1.01 Build 50";
        int versionTextWidth = 0;
        int versionTextHeight = 0;
        aGame->mFont->MeasureStringWithFont(
            versionFont, versionText, 8, versionTextWidth, versionTextHeight);

        aGame->mFont->DrawStringWithFontAt(
            versionFont,
            aGame->mSpriteBatch,
            versionText,
            static_cast<float>(aGame->mWindowWidth - versionTextWidth),
            static_cast<float>(aGame->mWindowHeight) - static_cast<float>(versionTextHeight) -
                12.0F,
            SDL_Color{255, 255, 255, 255},
            8);
    }

    std::string MainMenuScene::GetName() const
    {
        return "MainMenuScene";
    }

    void MainMenuScene::OnPlayClick() {}

    void MainMenuScene::OnStatsClick() {}

    void MainMenuScene::OnFriendClick() {}

    void MainMenuScene::OnOptionsClick() {}

    void MainMenuScene::OnExitClick()
    {
        SDL_Event quit_event;
        quit_event.type = SDL_EVENT_QUIT;
        SDL_PushEvent(&quit_event);
    }
} // namespace nuvelocity::frs42
