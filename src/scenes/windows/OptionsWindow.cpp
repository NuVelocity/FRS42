#include "OptionsWindow.h"
#include "ConfirmWindow.h"
#include <Game.h>
#include <algorithm>
#include <system/ui/Button.h>
#include <system/ui/ButtonContainer.h>
#include <system/ui/MdiManager.h>
#include <system/ui/skin/JWindowSkin.h>


namespace nuvelocity::frs42
{
    OptionsWindow::OptionsWindow(Game* game)
            : MdiWindow("Options")
    {
        if (game == nullptr || game->mMdi == nullptr)
        {
            return;
        }

        JWindowSkin* skin = game->mMdi->GetSkin("Ricochet");
        SetSkin(skin);
        SetMovable(false);

        auto container = std::make_shared<nuvelocity::ButtonContainer>();
        container->SetRect({.x = 0, .y = 0, .w = 221, .h = 0});
        container->SetGap(8);
        container->SetMargin(31, 0, 20, 0);
        container->SetAutoCenter(true);

        const int buttonHeight = 21;

        std::vector<std::string> labels = {"Configure &Sound",
                                           "Configure &Video",
                                           "&Help",
                                           "Show &Demo",
                                           "Check Web For &Updates",
                                           "&Credits",
                                           "&About Ricochet"};

        for (const auto& label : labels)
        {
            auto btn = std::make_shared<nuvelocity::Button>();
            btn->SetCaption(label);
            btn->SetSkin(skin);
            btn->SetRect({.x = 0, .y = 0, .w = 221, .h = buttonHeight});

            btn->SetOnClick(
                [label](Game* g)
                {
                    std::string title = label;
                    title.erase(std::remove(title.begin(), title.end(), '&'), title.end());
                    auto confirm = std::make_shared<ConfirmWindow>(g, title, "Not yet implemented");
                    g->mMdi->AddCenteredWindow(g, confirm);
                });

            container->AddButton(btn);
        }

        // Done Button
        auto doneBtn = std::make_shared<nuvelocity::Button>();
        doneBtn->SetCaption("&Done");
        doneBtn->SetSkin(skin);
        int doneWidth = 120;
        doneBtn->SetRect({.x = 0, .y = 0, .w = doneWidth, .h = buttonHeight});

        doneBtn->SetOnClick(
            [this](Game* game)
            {
                (void)game;
                this->Close();
            });

        container->AddButton(doneBtn);
        AddChild(container);

        SetRect({.x = 0, .y = 0, .w = 280, .h = 100}); // Initial height, will grow
        FitToChildren(game);
    }
} // namespace nuvelocity::frs42
