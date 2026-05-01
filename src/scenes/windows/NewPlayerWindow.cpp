#include "NewPlayerWindow.h"
#include "DisplayShip.h"
#include "NewGameOptionsWindow.h"
#include "SelectPlayerWindow.h"
#include "Ship.h"
#include "StatsManager.h"
#include <Game.h>
#include <Sequence.h>
#include <system/AssetManager.h>
#include <system/SpriteBatch.h>
#include <system/ui/Button.h>
#include <system/ui/Label.h>
#include <system/ui/MdiManager.h>
#include <system/ui/skin/JWindowSkin.h>

namespace nuvelocity::frs42
{
    NewPlayerWindow::NewPlayerWindow(Game* game, int playerIndex, std::function<void(Game*)> onDone)
            : MdiWindow(playerIndex >= 0 ? "Edit Player" : "New Player")
            , mEditPlayerIndex(playerIndex)
            , mOnDone(std::move(onDone))
    {
        JWindowSkin* skin = game->mMdi->GetSkin("Ricochet");
        SetSkin(skin);
        SetRect({0, 0, 327, 220});
        SetFullScreen(false);
        SetClosable(true);
        SetMovable(false);

        auto nameLabel = std::make_shared<Label>("Name:", "OCR");
        nameLabel->SetRect({15, 37, 360, 20});
        AddChild(nameLabel);

        mNameInput = std::make_shared<TextBox>();
        mNameInput->SetSkin(skin);
        mNameInput->SetRect({78, 33, 201, 23});
        mNameInput->SetText("New Player");
        mNameInput->SetFocused(true);
        mNameInput->SelectAll();
        AddChild(mNameInput);

        mSelectedShipIndex = 1; // Default RS Ship 2

        mPreviewShip = std::make_shared<DisplayShip>();
        // FIXME: display ship widget point handling might need to be fixed.
        mPreviewShip->SetRect({145, 100, 0, 0});
        mPreviewShip->GetShip()->SetShipStyle(mSelectedShipIndex);
        mPreviewShip->Load(game);
        AddChild(mPreviewShip);

        // Ship Selection UI
        auto leftBtn = std::make_shared<Button>();
        leftBtn->SetCaption("<");
        leftBtn->SetSkin(skin);
        leftBtn->SetRect({76, 109, 16, 21});
        leftBtn->SetOnClick([this](Game* g) { ChangeShip(g, -1); });
        AddChild(leftBtn);

        auto rightBtn = std::make_shared<Button>();
        rightBtn->SetCaption(">");
        rightBtn->SetSkin(skin);
        rightBtn->SetRect({198, 109, 16, 21});
        rightBtn->SetOnClick([this](Game* g) { ChangeShip(g, 1); });
        AddChild(rightBtn);

        auto saveBtn = std::make_shared<Button>();
        saveBtn->SetCaption("Save");
        saveBtn->SetSkin(skin);
        saveBtn->SetRect({74, 142, 67, 21});
        saveBtn->SetOnClick([this](Game* g) { OnSave(g); });
        AddChild(saveBtn);

        auto cancelBtn = std::make_shared<Button>();
        cancelBtn->SetCaption("Cancel");
        cancelBtn->SetSkin(skin);
        cancelBtn->SetRect({149, 143, 67, 21});
        cancelBtn->SetOnClick([this](Game* g) { OnCancel(g); });
        AddChild(cancelBtn);

        if (mEditPlayerIndex >= 0)
        {
            LoadPlayer(game, mEditPlayerIndex);
        }
    }

    NewPlayerWindow::~NewPlayerWindow() {}

    void NewPlayerWindow::ChangeShip(Game* game, int delta)
    {
        const auto& paths = Ship::GetShipSequencePaths();
        mSelectedShipIndex = (mSelectedShipIndex + delta + (int)paths.size()) % (int)paths.size();
        if (mPreviewShip)
        {
            mPreviewShip->GetShip()->SetShipStyle(mSelectedShipIndex);
            mPreviewShip->Load(game);
        }
    }

    void NewPlayerWindow::OnSave(Game* game)
    {
        std::string name = mNameInput->GetText();
        if (name.empty())
            name = "New Player";

        auto* stats = StatsManager::Get().GetStats();
        if (mEditPlayerIndex >= 0 && stats != nullptr &&
            mEditPlayerIndex < static_cast<int>(stats->mPlayers.size()))
        {
            auto* player = stats->mPlayers[mEditPlayerIndex];
            player->mName = name;
            player->mShipStyle = mSelectedShipIndex;
            StatsManager::Get().Save();

            Close();
            if (mOnDone)
            {
                mOnDone(game);
            }
            return;
        }

        auto* player = new PlayerStats();
        player->mName = name;
        player->mShipStyle = mSelectedShipIndex;

        const int insertIndex = static_cast<int>(stats->mPlayers.size()) - 1;
        stats->mPlayers.insert(stats->mPlayers.begin() + insertIndex, player);
        StatsManager::Get().SetCurrentPlayer(insertIndex);
        StatsManager::Get().Save();

        Close();
        if (mOnDone)
        {
            mOnDone(game);
            return;
        }
        game->mMdi->AddCenteredWindow(game, std::make_shared<SelectPlayerWindow>(game));
    }

    void NewPlayerWindow::OnCancel(Game* game)
    {
        Close();
        if (mOnDone)
        {
            mOnDone(game);
            return;
        }
        game->mMdi->AddCenteredWindow(game, std::make_shared<SelectPlayerWindow>(game));
    }

    void NewPlayerWindow::LoadPlayer(Game* game, int playerIndex)
    {
        auto* stats = StatsManager::Get().GetStats();
        if (stats == nullptr || playerIndex < 0 ||
            playerIndex >= static_cast<int>(stats->mPlayers.size()))
        {
            return;
        }

        auto* player = stats->mPlayers[playerIndex];
        if (player == nullptr)
        {
            return;
        }

        mNameInput->SetText(player->mName);
        mNameInput->SelectAll();
        mSelectedShipIndex = player->mShipStyle;
        if (mPreviewShip)
        {
            mPreviewShip->GetShip()->SetShipStyle(mSelectedShipIndex);
            mPreviewShip->Load(game);
        }
    }

    void NewPlayerWindow::Update(Game* game)
    {
        MdiWindow::Update(game);

        float dt = game->GetDeltaTime();
        mAnimationTimer += dt;

        if (mAnimationTimer >= 0.05F)
        {
            mAnimationTimer = 0.0F;
            if (mTiltIncreasing)
            {
                mCurrentTiltFrame++;
                if (mCurrentTiltFrame >= 20)
                {
                    mCurrentTiltFrame = 20;
                    mTiltIncreasing = false;
                }
            }
            else
            {
                mCurrentTiltFrame--;
                if (mCurrentTiltFrame <= 0)
                {
                    mCurrentTiltFrame = 0;
                    mTiltIncreasing = true;
                }
            }
        }

        if (mPreviewShip)
        {
            float velocity = (mCurrentTiltFrame - 10) / 2.5F;
            mPreviewShip->GetShip()->SetVelocityX(-velocity);
        }
    }

    void NewPlayerWindow::Draw(Game* game)
    {
        MdiWindow::Draw(game);
    }
} // namespace nuvelocity::frs42
