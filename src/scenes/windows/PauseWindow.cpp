#include "PauseWindow.h"
#include "ArenaScene.h"
#include "ConfirmWindow.h"
#include "MainMenuScene.h"
#include "OptionsWindow.h"
#include "StatisticsWindow.h"
#include <Game.h>
#include <system/ui/Button.h>
#include <system/ui/ButtonContainer.h>
#include <system/ui/MdiManager.h>
#include <system/ui/skin/JWindowSkin.h>

namespace nuvelocity::frs42
{
    PauseWindow::PauseWindow(Game* game)
            : MdiWindow("Pause")
    {
        if (game == nullptr || game->mMdi == nullptr)
        {
            return;
        }

        JWindowSkin* skin = game->mMdi->GetSkin("Ricochet");
        SetSkin(skin);
        SetMovable(false);

        auto container = std::make_shared<nuvelocity::ButtonContainer>();
        container->SetRect({.x = 0, .y = 0, .w = 173, .h = 0});
        container->SetGap(8);
        container->SetMargin(33, 0, 9, 0);
        container->SetAutoCenter(true);

        const int buttonHeight = 21;

        // Options
        auto optBtn = std::make_shared<nuvelocity::Button>();
        optBtn->SetCaption("&Options");
        optBtn->SetSkin(skin);
        optBtn->SetRect({.x = 0, .y = 0, .w = 173, .h = buttonHeight});
        optBtn->SetOnClick(
            [](Game* game)
            { game->mMdi->AddCenteredWindow(game, std::make_shared<OptionsWindow>(game)); });
        container->AddButton(optBtn);

        // Statistics
        auto statsBtn = std::make_shared<nuvelocity::Button>();
        statsBtn->SetCaption("&Statistics");
        statsBtn->SetSkin(skin);
        statsBtn->SetRect({.x = 0, .y = 0, .w = 221, .h = buttonHeight});
        statsBtn->SetOnClick([](Game* game)
                             { game->mMdi->AddWindow(std::make_shared<StatisticsWindow>(game)); });
        container->AddButton(statsBtn);

        // Abort Game
        auto abortBtn = std::make_shared<nuvelocity::Button>();
        abortBtn->SetCaption("&Abort Game");
        abortBtn->SetSkin(skin);
        abortBtn->SetRect({.x = 0, .y = 0, .w = 221, .h = buttonHeight});
        abortBtn->SetOnClick(
            [this](Game* game)
            {
                auto arena = dynamic_cast<ArenaScene*>(game->GetScene());
                if (arena && arena->GetScore() > 0)
                {
                    std::string title = "End Game Confirmation";
                    std::string message = "Do you really want to end the game in progress?";
                    auto confirm = std::make_shared<ConfirmWindow>(
                        game,
                        title,
                        message,
                        [this](Game* g)
                        {
                            auto arena = dynamic_cast<ArenaScene*>(g->GetScene());
                            if (arena)
                            {
                                arena->EndGame(g, false);
                            }
                            this->Close();
                        },
                        [](Game* g) { (void)g; },
                        "End Game",
                        "Continue Playing");
                    game->mMdi->AddCenteredWindow(game, confirm);
                }
                else
                {
                    arena->EndGame(game, false);
                    this->Close();
                }
            });
        container->AddButton(abortBtn);

        // Suspend Game
        auto suspendBtn = std::make_shared<nuvelocity::Button>();
        suspendBtn->SetCaption("&Suspend Game");
        suspendBtn->SetSkin(skin);
        suspendBtn->SetRect({.x = 0, .y = 0, .w = 221, .h = buttonHeight});
        suspendBtn->SetOnClick(
            [this](Game* game)
            {
                std::string title = "Suspend Game Confirmation";
                std::string message =
                    "This option will stop the game in progress and allow you to continue playing "
                    "it at a later time.  When you continue later, you will have to re-start from "
                    "the beginning of this round but your score and Ion Spheres will be "
                    "preserved.";

                auto confirm = std::make_shared<ConfirmWindow>(
                    game,
                    title,
                    message,
                    [this](Game* g)
                    {
                        auto arena = dynamic_cast<ArenaScene*>(g->GetScene());
                        if (arena)
                        {
                            arena->SuspendGame();
                        }
                        this->Close();
                        auto mainmenu = new MainMenuScene();
                        g->SetScene(mainmenu);

                        std::string alertTitle = "Game Suspended";
                        std::string alertMessage =
                            "Your game has been suspended.  Use the \"Play Game\" option to "
                            "resume it at any time.";
                        g->mMdi->AddCenteredWindow(
                            g, std::make_shared<ConfirmWindow>(g, alertTitle, alertMessage));
                    },
                    [](Game* g) { (void)g; },
                    "Suspend Game",
                    "Continue Playing");

                game->mMdi->AddCenteredWindow(game, confirm);
            });
        container->AddButton(suspendBtn);

        AddChild(container);

        // Continue Button
        auto contBtn = std::make_shared<nuvelocity::Button>();
        contBtn->SetCaption("&Continue");
        contBtn->SetSkin(skin);
        contBtn->SetRect({.x = 0, .y = 0, .w = 84, .h = buttonHeight});
        WidgetStyle ws;
        ws.margin = {.left = 0, .top = 4, .right = 0, .bottom = 0};

        Button::Style style;
        style.baseStyle = ws;

        contBtn->SetStyle(style);
        contBtn->SetOnClick(
            [this](Game* game)
            {
                (void)game;
                this->Close();
            });
        container->AddButton(contBtn);

        SetRect({.x = 0, .y = 0, .w = 231, .h = 235});
        FitToChildren(game);
    }
} // namespace nuvelocity::frs42
