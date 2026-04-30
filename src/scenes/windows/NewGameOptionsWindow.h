#ifndef FRS42_NEW_GAME_OPTIONS_WINDOW_H
#define FRS42_NEW_GAME_OPTIONS_WINDOW_H

#include <system/ui/MdiWindow.h>

namespace nuvelocity::frs42
{
    class NewGameOptionsWindow : public nuvelocity::MdiWindow
    {
    public:
        NewGameOptionsWindow(nuvelocity::Game* game);
    };
} // namespace nuvelocity::frs42

#endif // FRS42_NEW_GAME_OPTIONS_WINDOW_H
