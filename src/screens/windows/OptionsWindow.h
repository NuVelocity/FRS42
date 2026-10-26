#ifndef FRS42_OPTIONS_WINDOW_H
#define FRS42_OPTIONS_WINDOW_H

#include <system/ui/MdiWindow.h>

namespace nuvelocity::frs42
{
    class OptionsWindow : public nuvelocity::MdiWindow
    {
    public:
        OptionsWindow(nuvelocity::Game* game);
    };
} // namespace nuvelocity::frs42

#endif // FRS42_OPTIONS_WINDOW_H
