#include "ConfirmWindow.h"
#include <Game.h>
#include <system/ui/Button.h>
#include <system/ui/Label.h>
#include <system/ui/MdiManager.h>
#include <system/ui/skin/JWindowSkin.h>

namespace nuvelocity::frs42
{
    ConfirmWindow::ConfirmWindow(Game* game,
                                 const std::string& title,
                                 const std::string& message,
                                 std::function<void(Game*)> onConfirm,
                                 std::function<void(Game*)> onCancel,
                                 const std::string& confirmText,
                                 const std::string& cancelText)
            : ConfirmWindow(
                  game, title, message, onConfirm, onCancel, confirmText, cancelText, true)
    {
    }

    ConfirmWindow::ConfirmWindow(Game* game, const std::string& title, const std::string& message)
            : ConfirmWindow(game, title, message, [](Game*) {}, {}, "OK", "", false)
    {
    }

    ConfirmWindow::ConfirmWindow(Game* game,
                                 const std::string& title,
                                 const std::string& message,
                                 std::function<void(Game*)> onConfirm,
                                 std::function<void(Game*)> onCancel,
                                 const std::string& confirmText,
                                 const std::string& cancelText,
                                 bool showCancelButton)
            : MdiWindow(title)
            , mOnConfirm(std::move(onConfirm))
            , mOnCancel(std::move(onCancel))
    {
        if (game == nullptr || game->mMdi == nullptr)
        {
            return;
        }

        JWindowSkin* skin = game->mMdi->GetSkin("Ricochet");
        SetSkin(skin);
        SetRect({0, 0, 560, 160});
        SetFullScreen(false);
        SetClosable(true);
        SetMovable(false);

        auto label = std::make_shared<Label>(message, "OCR");
        label->SetWrap(true);
        label->SetRect({20, 40, 500, -1});
        label->Update(game);
        AddChild(label);

        const SDL_Rect labelRect = label->GetRect();
        const int buttonY = labelRect.y + labelRect.h + 15;
        const int buttonW = 133;
        const int buttonH = 21;

        if (showCancelButton)
        {
            auto cancelBtn = std::make_shared<Button>();
            cancelBtn->SetCaption(cancelText);
            cancelBtn->SetSkin(skin);
            cancelBtn->SetRect({120, buttonY, buttonW, buttonH});
            cancelBtn->SetOnClick([this](Game* g) { OnCancel(g); });
            AddChild(cancelBtn);

            auto confirmBtn = std::make_shared<Button>();
            confirmBtn->SetCaption(confirmText);
            confirmBtn->SetSkin(skin);
            confirmBtn->SetRect({270, buttonY, buttonW, buttonH});
            confirmBtn->SetOnClick([this](Game* g) { OnConfirm(g); });
            AddChild(confirmBtn);
        }
        else
        {
            auto confirmBtn = std::make_shared<Button>();
            confirmBtn->SetCaption(confirmText);
            confirmBtn->SetSkin(skin);
            confirmBtn->SetRect({250, buttonY, 30, buttonH});
            confirmBtn->SetOnClick([this](Game* g) { OnConfirm(g); });
            AddChild(confirmBtn);
        }

        FitToChildren(game);

        // Ensure some minimum padding at the bottom
        SDL_Rect rect = GetRect();
        rect.h += 10;
        SetRect(rect);
    }

    void ConfirmWindow::OnConfirm(Game* game)
    {
        Close();
        if (mOnConfirm)
        {
            mOnConfirm(game);
        }
    }

    void ConfirmWindow::OnCancel(Game* game)
    {
        Close();
        if (mOnCancel)
        {
            mOnCancel(game);
        }
    }
} // namespace nuvelocity::frs42
