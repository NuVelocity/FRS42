#ifndef FRS42_PAUSE_WINDOW_H
#define FRS42_PAUSE_WINDOW_H

#include <system/ui/MdiWindow.h>

namespace nuvelocity::frs42
{
    class PauseWindow : public nuvelocity::MdiWindow
    {
    public:
        PauseWindow(nuvelocity::Game* game);
    };
} // namespace nuvelocity::frs42

#endif // FRS42_PAUSE_WINDOW_H
