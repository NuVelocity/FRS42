#ifndef FRS42_CONFIRM_WINDOW_H
#define FRS42_CONFIRM_WINDOW_H

#include <functional>
#include <string>
#include <system/ui/MdiWindow.h>

namespace nuvelocity
{
    class Game;
}

namespace nuvelocity::frs42
{
    class ConfirmWindow : public MdiWindow
    {
    public:
        // Full confirmation dialog (Yes/No style)
        ConfirmWindow(Game* game,
                      const std::string& title,
                      const std::string& message,
                      std::function<void(Game*)> onConfirm,
                      std::function<void(Game*)> onCancel = {},
                      const std::string& confirmText = "Confirm",
                      const std::string& cancelText = "Cancel");

        // Alert dialog (OK only style)
        ConfirmWindow(Game* game, const std::string& title, const std::string& message);

        virtual ~ConfirmWindow() = default;

    private:
        ConfirmWindow(Game* game,
                      const std::string& title,
                      const std::string& message,
                      std::function<void(Game*)> onConfirm,
                      std::function<void(Game*)> onCancel,
                      const std::string& confirmText,
                      const std::string& cancelText,
                      bool showCancelButton);

        void OnConfirm(Game* game);
        void OnCancel(Game* game);

        std::function<void(Game*)> mOnConfirm;
        std::function<void(Game*)> mOnCancel;
    };
} // namespace nuvelocity::frs42

#endif // FRS42_CONFIRM_WINDOW_H
