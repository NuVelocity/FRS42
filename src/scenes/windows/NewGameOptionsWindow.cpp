#include "NewGameOptionsWindow.h"
#include "ArenaScene.h"
#include "CheckpointButton.h"
#include "LevelSelectWindow.h"
#include "RoundSet.h"
#include <Game.h>
#include <algorithm>
#include <system/AssetManager.h>
#include <system/ui/Button.h>
#include <system/ui/Label.h>
#include <system/ui/MdiManager.h>
#include <system/ui/skin/JWindowSkin.h>

namespace nuvelocity::frs42
{
    NewGameOptionsWindow::NewGameOptionsWindow(Game* game)
            : MdiWindow("New Game Options")
    {
        if (game == nullptr || game->mMdi == nullptr)
        {
            return;
        }

        JWindowSkin* skin = game->mMdi->GetSkin("Ricochet");
        SetSkin(skin);
        SetFullScreen(true);
        SetClosable(true);

        const int margin = 20;

        // Difficulty row
        auto diffLabel = std::make_shared<nuvelocity::Label>("Difficulty:");
        diffLabel->SetRect({.x = margin, .y = 40, .w = 100, .h = 20});
        AddChild(diffLabel);

        auto diffEasy = std::make_shared<nuvelocity::Button>();
        diffEasy->SetCaption("Easy");
        diffEasy->SetSkin(skin);
        diffEasy->SetRect({.x = 120, .y = 40, .w = 80, .h = 20});
        AddChild(diffEasy);

        auto diffNormal = std::make_shared<nuvelocity::Button>();
        diffNormal->SetCaption("Normal");
        diffNormal->SetSkin(skin);
        diffNormal->SetRect({.x = 210, .y = 40, .w = 80, .h = 20});
        AddChild(diffNormal);

        auto diffHard = std::make_shared<nuvelocity::Button>();
        diffHard->SetCaption("Hard");
        diffHard->SetSkin(skin);
        diffHard->SetRect({.x = 300, .y = 40, .w = 80, .h = 20});
        AddChild(diffHard);

        auto diffInsane = std::make_shared<nuvelocity::Button>();
        diffInsane->SetCaption("Insane");
        diffInsane->SetSkin(skin);
        diffInsane->SetRect({.x = 390, .y = 40, .w = 80, .h = 20});
        AddChild(diffInsane);

        // Goto Level Button
        auto gotoBtn = std::make_shared<nuvelocity::Button>();
        gotoBtn->SetCaption("Goto Level");
        gotoBtn->SetSkin(skin);
        gotoBtn->SetRect({.x = 520, .y = 40, .w = 100, .h = 20});
        gotoBtn->SetOnClick([this](Game* game)
                            { game->mMdi->AddWindow(std::make_shared<LevelSelectWindow>(game)); });
        AddChild(gotoBtn);

        // Hint label
        auto hintLabel = std::make_shared<nuvelocity::Label>(
            "Click on the image of the round you would like to start from");
        hintLabel->SetRect({.x = margin, .y = 75, .w = 600, .h = 20});
        AddChild(hintLabel);

        // Load RoundSets
        auto roundSets = game->mAsset->EnumerateRoundSets();
        std::ranges::sort(roundSets,
                          [](const auto& a, const auto& b) { return a.second < b.second; });

        int ry = 110;
        int rsIdx = 1;
        for (const auto& [fullPath, rsName] : roundSets)
        {
            RoundSet* rs = static_cast<RoundSet*>(game->mAsset->LoadPropertyFile(fullPath));
            if (rs == nullptr)
            {
                continue;
            }

            int rx = margin;
            for (auto* cp : rs->GetCheckPoints())
            {
                std::string imgPath = "Resources/" + cp->GetImage();
                Sequence* img = game->mAsset->LoadSequence(imgPath);

                int roundNum = cp->GetRoundNumber();
                std::string label = std::to_string(rsIdx) + "-" + std::to_string(roundNum + 1);

                auto btn = std::make_shared<frs42::CheckpointButton>(img, roundNum, label);
                btn->SetRect({.x = rx, .y = ry, .w = 107, .h = 74});
                btn->SetSkin(skin);

                std::string rsNameCopy = rsName;
                btn->SetOnClick(
                    [this, rsNameCopy, roundNum](Game* game)
                    {
                        this->Close();
                        game->SetScene(new ArenaScene(rsNameCopy, roundNum));
                    });

                AddChild(btn);
                rx += 115;
            }
            ry += 82;
            rsIdx++;
        }
    }
} // namespace nuvelocity::frs42
