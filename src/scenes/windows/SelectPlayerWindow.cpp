#include "SelectPlayerWindow.h"
#include "ArenaScene.h"
#include "ConfirmWindow.h"
#include "NewGameOptionsWindow.h"
#include "NewPlayerWindow.h"
#include "RoundSetManager.h"
#include "StatsManager.h"
#include "SuspendedGameStats.h"
#include <Game.h>
#include <system/ui/Button.h>
#include <system/ui/ButtonContainer.h>
#include <system/ui/MdiManager.h>
#include <system/ui/skin/JWindowSkin.h>

namespace nuvelocity::frs42
{
    SelectPlayerWindow::SelectPlayerWindow(Game* game)
            : MdiWindow("Select Player")
    {
        JWindowSkin* skin = game->mMdi->GetSkin("Ricochet");
        SetSkin(skin);
        SetMovable(false);

        auto container = std::make_shared<nuvelocity::ButtonContainer>();
        container->SetRect({.x = 0, .y = 0, .w = 221, .h = 0});
        container->SetGap(8);
        container->SetMargin(31, 0, 10, 0);
        container->SetAutoCenter(true);

        const int buttonHeight = 21;
        auto* stats = StatsManager::Get().GetStats();

        int index = 0;
        for (auto* player : stats->mPlayers)
        {
            auto btn = std::make_shared<Button>();
            btn->SetCaption(player->mName);
            btn->SetSkin(skin);
            btn->SetRect({0, 0, 221, buttonHeight});
            btn->SetOnClick([this, index](Game* g) { OnSelectPlayer(g, index); });
            container->AddButton(btn);
            index++;
        }

        // New Player Button
        auto newPlayerBtn = std::make_shared<Button>();
        newPlayerBtn->SetCaption("New Player");
        newPlayerBtn->SetSkin(skin);
        newPlayerBtn->SetRect({.x = 0, .y = 0, .w = 84, .h = buttonHeight});
        WidgetStyle ws;
        ws.margin = {.left = 0, .top = 4, .right = 0, .bottom = 0};

        Button::Style style;
        style.baseStyle = ws;
        newPlayerBtn->SetStyle(style);
        newPlayerBtn->SetRect({0, 0, 114, buttonHeight});
        newPlayerBtn->SetOnClick([this](Game* g) { OnNewPlayer(g); });
        newPlayerBtn->SetEnabled(CanAddNewPlayer());
        container->AddButton(newPlayerBtn);

        AddChild(container);

        SetRect({0, 0, 280, 100});
        FitToChildren(game);
    }

    void SelectPlayerWindow::OnSelectPlayer(Game* game, int index)
    {
        StatsManager::Get().SetCurrentPlayer(index);
        PlayerStats* player = StatsManager::Get().GetCurrentPlayer();

        Close();

        if (player->mSuspendedGame != nullptr)
        {
            const std::string msg = "The selected player has a previously suspended game. Press "
                                    "play to resume the previous game.";
            auto confirm = std::make_shared<ConfirmWindow>(
                game,
                "Resume Game",
                msg,
                [](Game* g)
                {
                    PlayerStats* player = StatsManager::Get().GetCurrentPlayer();
                    if (player == nullptr || player->mSuspendedGame == nullptr)
                        return;

                    int globalIndex = player->mSuspendedGame->mEndingRoundNumber;
                    const auto* entry = RoundSetManager::Get().GetRoundEntry(globalIndex);
                    if (entry == nullptr)
                        return;

                    auto arena = new ArenaScene(entry);
                    arena->SetInitialStats(*player->mSuspendedGame,
                                           player->mSuspendedGame->mBallsLeft);

                    // Clear suspended game after resume
                    delete player->mSuspendedGame;
                    player->mSuspendedGame = nullptr;
                    StatsManager::Get().Save();

                    g->SetScene(arena);
                },
                [](Game* g)
                { g->mMdi->AddCenteredWindow(g, std::make_shared<SelectPlayerWindow>(g)); },
                "Resume Play",
                "Cancel");
            game->mMdi->AddCenteredWindow(game, confirm);
        }
        else
        {
            game->mMdi->AddCenteredWindow(game, std::make_shared<NewGameOptionsWindow>(game));
        }
    }

    void SelectPlayerWindow::OnNewPlayer(Game* game)
    {
        if (CanAddNewPlayer())
        {
            Close();
            game->mMdi->AddCenteredWindow(game, std::make_shared<NewPlayerWindow>(game));
        }
    }

    bool SelectPlayerWindow::CanAddNewPlayer()
    {
        auto* stats = StatsManager::Get().GetStats();
        return stats->mPlayers.size() < StatsManager::kMaxPlayers;
    }
} // namespace nuvelocity::frs42
