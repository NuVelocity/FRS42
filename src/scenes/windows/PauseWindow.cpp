#include "PauseWindow.h"
#include "MainMenuScene.h"
#include "OptionsWindow.h"
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
        container->AddButton(statsBtn);

        // Abort Game
        auto abortBtn = std::make_shared<nuvelocity::Button>();
        abortBtn->SetCaption("&Abort Game");
        abortBtn->SetSkin(skin);
        abortBtn->SetRect({.x = 0, .y = 0, .w = 221, .h = buttonHeight});
        abortBtn->SetOnClick(
            [this](Game* game)
            {
                this->Close();
                game->SetScene(new MainMenuScene());
            });
        container->AddButton(abortBtn);

        // Suspend Game
        auto suspendBtn = std::make_shared<nuvelocity::Button>();
        suspendBtn->SetCaption("&Suspend Game");
        suspendBtn->SetSkin(skin);
        suspendBtn->SetRect({.x = 0, .y = 0, .w = 221, .h = buttonHeight});
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
