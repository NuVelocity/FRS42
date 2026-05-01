#include "NewGameOptionsWindow.h"
#include "ArenaScene.h"
#include "CheckpointButton.h"
#include "ConfirmWindow.h"
#include "Difficulty.h"
#include "LevelSelectWindow.h"
#include "RoundSet.h"
#include "RoundSetManager.h"
#include "StatsManager.h"
#include <Game.h>
#include <algorithm>
#include <array>
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

        auto diffLabel = std::make_shared<nuvelocity::Label>("Difficulty:");
        diffLabel->SetRect({.x = margin, .y = 40, .w = 100, .h = 20});
        AddChild(diffLabel);

        PlayerStats* player = StatsManager::Get().GetCurrentPlayer();
        mDifficultyGroup.SetOnSelectionChanged(
            [player](int id)
            {
                if (player)
                {
                    player->mLastSelectedDifficulty = id;
                }
            });

        const std::array<int, 4> diffButtonX = {120, 210, 300, 390};
        const int diffButtonY = 40;
        const int diffButtonW = 80;
        const int diffButtonH = 20;
        const auto& difficulties = GetAllDifficulties();
        for (size_t i = 0; i < difficulties.size(); ++i)
        {
            const Difficulty diff = difficulties[i];
            auto diffBtn = std::make_shared<nuvelocity::Button>();
            diffBtn->SetCaption(DifficultyToString(diff));
            diffBtn->SetSkin(skin);
            diffBtn->SetRect(
                {.x = diffButtonX[i], .y = diffButtonY, .w = diffButtonW, .h = diffButtonH});

            mDifficultyGroup.AddButton(diffBtn, DifficultyToInt(diff));
            AddChild(diffBtn);
        }

        if (player)
        {
            mDifficultyGroup.SetSelectedIndex(player->mLastSelectedDifficulty);
        }
        else
        {
            mDifficultyGroup.SetSelectedIndex(DifficultyToInt(Difficulty::Normal));
        }

        auto gotoBtn = std::make_shared<nuvelocity::Button>();
        gotoBtn->SetCaption("Goto Level");
        gotoBtn->SetSkin(skin);
        gotoBtn->SetRect({.x = 490, .y = 40, .w = 100, .h = 20});
        gotoBtn->SetOnClick([this](Game* g)
                            { g->mMdi->AddWindow(std::make_shared<LevelSelectWindow>(g)); });
        AddChild(gotoBtn);

        auto hintLabel = std::make_shared<nuvelocity::Label>(
            "Click on the image of the round you would like to start from");
        hintLabel->SetRect({.x = margin, .y = 75, .w = 600, .h = 20});
        AddChild(hintLabel);

        auto& rsMgr = RoundSetManager::Get();
        int ry = 110;

        // We need to iterate over sets to build the grid
        auto roundSets = game->mAsset->EnumerateRoundSets();
        std::ranges::sort(roundSets,
                          [](const auto& a, const auto& b) { return a.second < b.second; });

        int setIdx = 0;
        for (const auto& [fullPath, rsName] : roundSets)
        {
            RoundSet* rs = rsMgr.GetRoundSet(setIdx);
            if (rs == nullptr)
            {
                continue;
            }

            int rx = margin;
            int cpIdx = 0;
            for (auto* cp : rs->GetCheckPoints())
            {
                std::string imgPath = "Resources/" + cp->GetImage();
                Sequence* img = game->mAsset->LoadSequence(imgPath);

                int roundNum = cp->GetRoundNumber();
                std::string labelStr =
                    std::to_string(setIdx + 1) + "-" + std::to_string(roundNum + 1);

                auto btn = std::make_shared<frs42::CheckpointButton>(img, roundNum, labelStr);
                btn->SetRect({.x = rx, .y = ry, .w = 107, .h = 74});
                btn->SetSkin(skin);

                bool locked = false;
                if (player != nullptr &&
                    setIdx < static_cast<int>(player->mCheckPointsReached.size()))
                {
                    locked = cpIdx > player->mCheckPointsReached[setIdx] && cpIdx != 0;
                }
                else
                {
                    // If we don't have player data, lock all but the first checkpoint.
                    locked = cpIdx != 0 || setIdx != 0;
                }

                btn->SetEnabled(true);
                btn->SetLocked(locked);

                // Find the entry for this checkpoint
                const RoundEntry* foundEntry = nullptr;
                for (const auto& entry : rsMgr.GetAllRounds())
                {
                    if (entry.setIndex == setIdx && entry.roundIndex == roundNum)
                    {
                        foundEntry = &entry;
                        break;
                    }
                }

                if (foundEntry != nullptr)
                {
                    btn->SetOnClick(
                        [this, btn, foundEntry](Game* g)
                        {
                            if (btn->IsLocked())
                            {
                                std::string title = "Round not enabled yet";
                                std::string message =
                                    "You cannot start from that round because you have not "
                                    "progressed that far in the game.  Each time you beat a set of "
                                    "ten rounds, you earn the right to start future games at the "
                                    "beginning of the next set of rounds.";
                                g->mMdi->AddCenteredWindow(
                                    g, std::make_shared<ConfirmWindow>(g, title, message));
                                return;
                            }
                            this->Close();
                            Difficulty diff =
                                DifficultyFromInt(this->mDifficultyGroup.GetSelectedIndex());
                            g->SetScene(new ArenaScene(foundEntry, true, diff));
                        });
                }

                AddChild(btn);
                rx += 115;
                cpIdx++;
            }
            ry += 82;
            setIdx++;
        }
    }
} // namespace nuvelocity::frs42
