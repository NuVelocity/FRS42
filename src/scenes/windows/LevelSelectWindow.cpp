#include "LevelSelectWindow.h"
#include "ArenaScene.h"
#include "RoundSet.h"
#include <Game.h>
#include <algorithm>
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
        mLevelEntries.clear();
        mListView->Clear();

        auto roundSets = game->mAsset->EnumerateRoundSets();
        std::ranges::sort(roundSets,
                          [](const auto& a, const auto& b) { return a.second < b.second; });

        for (const auto& [fullPath, rsName] : roundSets)
        {
            RoundSet* rs = static_cast<RoundSet*>(game->mAsset->LoadPropertyFile(fullPath));
            if (rs == nullptr)
                continue;

            const auto& rounds = rs->GetRoundList();
            for (size_t i = 0; i < rounds.size(); ++i)
            {
                LevelEntry entry;
                entry.roundSetPath = fullPath;
                entry.roundSetName = rsName;
                entry.roundName = rounds[i];
                entry.roundIndex = static_cast<int>(i);
                mLevelEntries.push_back(entry);

                mListView->AddRow({rsName, std::to_string(i + 1), rounds[i]});
            }
        }
    }

    void LevelSelectWindow::OnGoClick(Game* game)
    {
        int selected = mListView->GetSelectedIndex();
        if (selected >= 0 && selected < static_cast<int>(mLevelEntries.size()))
        {
            const auto& entry = mLevelEntries[selected];
            game->mMdi->Clear();
            game->SetScene(new ArenaScene(entry.roundSetName, entry.roundIndex));
        }
    }
} // namespace nuvelocity::frs42
