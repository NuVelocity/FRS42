#ifndef FRS42_SELECT_PLAYER_WINDOW_H
#define FRS42_SELECT_PLAYER_WINDOW_H

#include <memory>
#include <system/ui/MdiWindow.h>

namespace nuvelocity
{
    class Game;
}

namespace nuvelocity::frs42
{
    class SelectPlayerWindow : public MdiWindow
    {
    public:
        SelectPlayerWindow(Game* game);
        virtual ~SelectPlayerWindow() = default;

    private:
        void OnSelectPlayer(Game* game, int index);
        void OnNewPlayer(Game* game);
        bool CanAddNewPlayer();
    };
} // namespace nuvelocity::frs42

#endif // FRS42_SELECT_PLAYER_WINDOW_H
