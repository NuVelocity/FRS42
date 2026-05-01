#include "LevelSelectWindow.h"
#include "ArenaScene.h"
#include "RoundSetManager.h"
#include <Game.h>
#include <system/AssetManager.h>
#include <system/ui/Button.h>
#include <system/ui/MdiManager.h>
#include <system/ui/skin/JWindowSkin.h>

namespace nuvelocity::frs42
{
    LevelSelectWindow::LevelSelectWindow(Game* game)
            : MdiWindow("Goto Level")
    {
        SetRect({0, 0, game->mWindowWidth, game->mWindowHeight});
        SetFullScreen(true);
        SetClosable(true);

        JWindowSkin* skin = game->mMdi->GetSkin("Ricochet");
        SetSkin(skin);

        const SDL_Rect clientRect = GetClientRect();

        mListView = std::make_shared<nuvelocity::JListBox>();
        mListView->SetSkin(skin);
        mListView->AddColumn("RoundSet", 150);
        mListView->AddColumn("Idx", 50);
        mListView->AddColumn("Round Name", 250);
        mListView->SetRect({.x = 10, .y = 20, .w = clientRect.w - 20, .h = clientRect.h - 70});
        AddChild(mListView);

        int btnY = clientRect.h - 40;
        int btnX = clientRect.w - 180;

        auto goBtn = std::make_shared<nuvelocity::Button>("Go");
        goBtn->SetSkin(skin);
        goBtn->SetRect({.x = btnX, .y = btnY, .w = 80, .h = 25});
        goBtn->SetOnClick([this](Game* g) { OnGoClick(g); });
        AddChild(goBtn);

        auto cancelBtn = std::make_shared<nuvelocity::Button>("Cancel");
        cancelBtn->SetSkin(skin);
        cancelBtn->SetRect({.x = btnX + 90, .y = btnY, .w = 80, .h = 25});
        cancelBtn->SetOnClick(
            [this](Game* g)
            {
                (void)g;
                Close();
            });
        AddChild(cancelBtn);

        PopulateList(game);
    }

    void LevelSelectWindow::PopulateList(Game* game)
    {
        (void)game;
        mListView->Clear();

        for (const auto& entry : RoundSetManager::Get().GetAllRounds())
        {
            mListView->AddRow({entry.setName, std::to_string(entry.roundIndex + 1), entry.name});
        }
    }

    void LevelSelectWindow::OnGoClick(Game* game)
    {
        int selected = mListView->GetSelectedIndex();
        const auto& allRounds = RoundSetManager::Get().GetAllRounds();
        if (selected >= 0 && selected < static_cast<int>(allRounds.size()))
        {
            const auto* entry = &allRounds[selected];
            game->mMdi->Clear();
            game->SetScene(new ArenaScene(entry));
        }
    }
} // namespace nuvelocity::frs42
