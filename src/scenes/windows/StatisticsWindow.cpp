#include "StatisticsWindow.h"
#include "ConfirmWindow.h"
#include "Difficulty.h"
#include "NewPlayerWindow.h"
#include "StatsManager.h"
#include <SDL3/SDL.h>
#include <format>
#include <iomanip>
#include <locale>
#include <sstream>
#include <system/Game.h>
#include <system/InputManager.h>
#include <system/ui/Button.h>
#include <system/ui/Label.h>
#include <system/ui/MdiManager.h>
#include <system/ui/WidgetUtils.h>

namespace nuvelocity::frs42
{
    namespace
    {
        std::string FormatTime(double seconds)
        {
            int totalSeconds = static_cast<int>(seconds);
            int hours = totalSeconds / 3600;
            int minutes = (totalSeconds % 3600) / 60;
            int secs = totalSeconds % 60;

            return std::format("{:02d}:{:02d}:{:02d}", hours, minutes, secs);
        }

        std::string FormatScore(int score)
        {
            try
            {
                return std::format(std::locale(""), "{:L}", score);
            }
            catch (const std::exception&)
            {
                return std::to_string(score);
            }
        }

    } // namespace

    StatisticsWindow::StatisticsWindow(Game* game)
            : StatisticsWindow(game, 0, std::string(), -1)
    {
    }

    StatisticsWindow::StatisticsWindow(Game* game, int initialTabIndex)
            : StatisticsWindow(game, initialTabIndex, std::string(), -1)
    {
    }

    StatisticsWindow::StatisticsWindow(Game* game,
                                       int initialTabIndex,
                                       const std::string& listFooterText,
                                       int selectedIndex)
            : MdiWindow("Statistics")
            , mListFooterText(listFooterText)
            , mListSelectedIndex(selectedIndex)
            , mInitialTabIndex(initialTabIndex)
    {
        if (game == nullptr || game->mMdi == nullptr)
        {
            return;
        }

        JWindowSkin* skin = game->mMdi->GetSkin("Ricochet");
        SetSkin(skin);
        SetFullScreen(true);
        InitializeTabs(game);
        PopulateData();
        ApplyListLayout(game);
        mTabControl->SetSelectedIndex(mInitialTabIndex);
        OnTabChanged(mTabControl->GetSelectedIndex());
    }

    void StatisticsWindow::InitializeTabs(Game* game)
    {
        JWindowSkin* skin = (game->mMdi != nullptr) ? game->mMdi->GetSkin("Ricochet") : nullptr;

        mTabControl = std::make_shared<JTabControl>();
        mTabControl->SetRect({10, 35, 586, 354}); // was 356
        mTabControl->SetSkin(skin);
        mTabControl->SetTabHeaderHeight(24);
        mTabControl->AddTab("Players");
        mTabControl->AddTab("High Scores");
        mTabControl->AddTab("Easy");
        mTabControl->AddTab("Normal");
        mTabControl->AddTab("Hard");
        mTabControl->AddTab("Insane");
        mTabControl->SetOnTabChanged([this](int index) { this->OnTabChanged(index); });
        AddChild(mTabControl);

        for (int i = 0; i < 6; ++i)
        {
            mLists[i] = std::make_shared<JListBox>();
            mLists[i]->SetSkin(skin);
            mLists[i]->SetVisible(false);

            // Players
            if (i == 0)
            {
                mLists[i]->SetRect({5, 5, 572, 289});
                mLists[i]->AddColumn("Player", 165);
                mLists[i]->AddColumn("Games", 65);
                mLists[i]->AddColumn("Total Time", 95);
                mLists[i]->AddColumn("Highest Round", 135);
                mLists[i]->AddColumn("Best Score", 110);
                mLists[i]->SetOnSelectionChanged([this](int) { UpdatePlayerButtons(); });
            }
            // High Scores & Difficulties
            else
            {
                mLists[i]->SetRect({5, 5, 572, 319});
                mLists[i]->AddColumn("Player", 150);
                mLists[i]->AddColumn("Score", 75);
                mLists[i]->AddColumn("Difficulty", 78);
                mLists[i]->AddColumn("Time", 65);
                mLists[i]->AddColumn("Start", 47);
                mLists[i]->AddColumn("End", 47);
            }

            mTabControl->AddChild(mLists[i]);
        }

        mListFooter = std::make_shared<Label>(mListFooterText, "OCR");
        mListFooter->SetWrap(true);
        mListFooter->SetVisible(false);
        mTabControl->AddChild(mListFooter);

        mEditPlayerButton = std::make_shared<Button>();
        mEditPlayerButton->SetCaption("Edit Player");
        mEditPlayerButton->SetSkin(skin);
        mEditPlayerButton->SetVisible(false);
        mEditPlayerButton->SetRect({6, 301, 105, 21});
        mEditPlayerButton->SetOnClick([this](Game* g) { OnEditPlayer(g); });
        mTabControl->AddChild(mEditPlayerButton);

        mDeletePlayerButton = std::make_shared<Button>();
        mDeletePlayerButton->SetCaption("Delete Player");
        mDeletePlayerButton->SetSkin(skin);
        mDeletePlayerButton->SetVisible(false);
        mDeletePlayerButton->SetRect({119, 301, 133, 21});
        mDeletePlayerButton->SetOnClick([this](Game* g) { OnDeletePlayer(g); });
        mTabControl->AddChild(mDeletePlayerButton);

        mCloseButton = std::make_shared<Button>();
        mCloseButton->SetCaption("Close");
        mCloseButton->SetSkin(skin);
        mCloseButton->SetRect({274, 398, 57, 21});
        mCloseButton->SetOnClick([this](Game* g) { this->Close(); });
        AddChild(mCloseButton);
    }

    void StatisticsWindow::PopulateData()
    {
        auto& statsMgr = StatsManager::Get();

        std::array<int, 6> previousSelections{};
        for (int i = 0; i < 6; ++i)
        {
            previousSelections[i] = mLists[i]->GetSelectedIndex();
        }

        // 1. Players Tab
        auto playerSummaries = statsMgr.GetPlayerSummaries();
        mLists[0]->Clear();
        for (const auto& s : playerSummaries)
        {
            std::vector<std::string> row;
            row.push_back(s.name);
            row.push_back(std::to_string(s.gamesPlayed));
            row.push_back(FormatTime(s.totalTime));
            row.push_back(statsMgr.FormatRoundIndex(s.highestRoundIndex));
            row.push_back(FormatScore(s.bestScore));
            mLists[0]->AddRow(row);
        }

        // 2. High Scores Tab (All)
        auto allHighScores = statsMgr.GetHighScores();
        mLists[1]->Clear();
        for (const auto& r : allHighScores)
        {
            std::vector<std::string> row;
            row.push_back(r.playerName);
            row.push_back(FormatScore(r.score));
            row.push_back(DifficultyToString(r.difficulty));
            row.push_back(FormatTime(r.time));
            row.push_back(statsMgr.FormatRoundIndex(r.startRound));
            row.push_back(statsMgr.FormatRoundIndex(r.endRound));
            mLists[1]->AddRow(row);
        }

        // 3-6. Difficulty Tabs
        const auto& tabDifficulties = GetAllDifficulties();
        for (size_t i = 0; i < tabDifficulties.size(); ++i)
        {
            auto diffScores = statsMgr.GetHighScores(DifficultyToInt(tabDifficulties[i]));
            mLists[i + 2]->Clear();
            for (const auto& r : diffScores)
            {
                std::vector<std::string> row;
                row.push_back(r.playerName);
                row.push_back(FormatScore(r.score));
                row.push_back(DifficultyToString(r.difficulty));
                row.push_back(FormatTime(r.time));
                row.push_back(statsMgr.FormatRoundIndex(r.startRound));
                row.push_back(statsMgr.FormatRoundIndex(r.endRound));
                mLists[i + 2]->AddRow(row);
            }
        }

        for (int i = 0; i < 6; ++i)
        {
            int desiredIndex = previousSelections[i];
            if (i == mInitialTabIndex && mListSelectedIndex >= 0)
            {
                desiredIndex = mListSelectedIndex;
            }
            if (desiredIndex >= 0)
            {
                mLists[i]->SetSelectedIndex(desiredIndex);
            }
        }

        UpdatePlayerButtons();
    }

    void StatisticsWindow::OnTabChanged(int index)
    {
        for (int i = 0; i < 6; ++i)
        {
            mLists[i]->SetVisible(i == index);
        }

        if (mListFooter)
        {
            const bool hasFooter = !mListFooterText.empty();
            mListFooter->SetVisible(index == mInitialTabIndex && hasFooter);
        }

        const bool playersTab = index == 0;
        if (mEditPlayerButton)
        {
            mEditPlayerButton->SetVisible(playersTab);
        }
        if (mDeletePlayerButton)
        {
            mDeletePlayerButton->SetVisible(playersTab);
        }

        UpdatePlayerButtons();
    }

    void StatisticsWindow::ApplyListLayout(Game* game)
    {
        if (mTabControl == nullptr)
        {
            return;
        }

        const SDL_Rect tabRect = mTabControl->GetRect();
        const int contentHeight = SDL_max(0, tabRect.h - mTabControl->GetTabHeaderHeight());
        const int listX = 5;
        const int listY = 5;
        const int listW = SDL_max(0, tabRect.w - 14);
        for (int i = 0; i < 6; ++i)
        {
            int tabHBase = (i == 0) ? 289 : SDL_max(0, contentHeight - 10);
            int footerHeight = 0;

            if (i == mInitialTabIndex && mListFooter && !mListFooterText.empty())
            {
                mListFooter->SetRect({listX, 0, listW, -1});
                mListFooter->Update(game);
                footerHeight = SDL_max(0, mListFooter->GetRect().h) + 22;
            }

            const int listH = SDL_max(40, tabHBase - footerHeight);
            mLists[i]->SetRect({listX, listY, listW, listH});

            if (i == mInitialTabIndex && mListFooter && !mListFooterText.empty())
            {
                const int footerY = listY + tabHBase - footerHeight;
                mListFooter->SetRect({listX, footerY + 11, listW, footerHeight - 22});
            }
        }
    }

    void StatisticsWindow::UpdatePlayerButtons()
    {
        if (mLists[0] == nullptr || mEditPlayerButton == nullptr || mDeletePlayerButton == nullptr)
        {
            return;
        }

        auto* stats = StatsManager::Get().GetStats();
        const int index = mLists[0]->GetSelectedIndex();
        const bool valid =
            stats != nullptr && index >= 0 && index < static_cast<int>(stats->mPlayers.size());
        bool allowEdit = valid;
        bool allowDelete = valid;

        if (valid)
        {
            const auto* player = stats->mPlayers[index];
            if (player != nullptr && player->mName == StatsManager::kGuestName)
            {
                allowEdit = false;
                allowDelete = false;
            }
        }

        mEditPlayerButton->SetEnabled(allowEdit);
        mDeletePlayerButton->SetEnabled(allowDelete);
    }

    void StatisticsWindow::OnEditPlayer(Game* game)
    {
        if (game == nullptr || game->mMdi == nullptr || mLists[0] == nullptr)
        {
            return;
        }

        const int index = mLists[0]->GetSelectedIndex();
        auto* stats = StatsManager::Get().GetStats();
        if (stats == nullptr || index < 0 || index >= static_cast<int>(stats->mPlayers.size()))
        {
            return;
        }

        auto* player = stats->mPlayers[index];
        if (player == nullptr || player->mName == StatsManager::kGuestName)
        {
            return;
        }

        auto window = std::make_shared<NewPlayerWindow>(game,
                                                        index,
                                                        [this, index](Game* g)
                                                        {
                                                            PopulateData();
                                                            mLists[0]->SetSelectedIndex(index);
                                                            UpdatePlayerButtons();
                                                            (void)g;
                                                        });
        game->mMdi->AddCenteredWindow(game, window);
    }

    void StatisticsWindow::OnDeletePlayer(Game* game)
    {
        if (game == nullptr || game->mMdi == nullptr || mLists[0] == nullptr)
        {
            return;
        }

        const int index = mLists[0]->GetSelectedIndex();
        auto* stats = StatsManager::Get().GetStats();
        if (stats == nullptr || index < 0 || index >= static_cast<int>(stats->mPlayers.size()))
        {
            return;
        }

        auto* player = stats->mPlayers[index];
        if (player == nullptr || player->mName == StatsManager::kGuestName)
        {
            return;
        }

        const std::string message = "Are you sure you want to permanently delete \"" +
                                    player->mName + "\" and all associated scores?";
        auto confirm = std::make_shared<ConfirmWindow>(
            game,
            "Delete Confirmation",
            message,
            [this, index](Game* g) { OnDeleteConfirmed(g, index); },
            nullptr,
            "Delete Player");
        game->mMdi->AddCenteredWindow(game, confirm);
    }

    void StatisticsWindow::OnDeleteConfirmed(Game* game, int playerIndex)
    {
        (void)game;
        if (!StatsManager::Get().RemovePlayer(playerIndex))
        {
            return;
        }

        PopulateData();
        const int rowCount = static_cast<int>(mLists[0]->GetRows().size());
        if (rowCount > 0)
        {
            const int nextIndex = SDL_min(playerIndex, rowCount - 1);
            mLists[0]->SetSelectedIndex(nextIndex);
        }
        UpdatePlayerButtons();
    }

    void StatisticsWindow::Update(Game* game)
    {
        MdiWindow::Update(game);

        // Escape to close
        if (game->mInput->IsKeyPressed(SDL_SCANCODE_ESCAPE))
        {
            Close();
        }
    }

    void StatisticsWindow::Draw(Game* game)
    {
        MdiWindow::Draw(game);
    }
} // namespace nuvelocity::frs42
