#include "DisplayShip.h"
#include <Game.h>

namespace nuvelocity::frs42
{
    DisplayShip::DisplayShip()
            : Widget()
    {
    }

    void DisplayShip::Load(Game* game)
    {
        mShip.Load(game);
    }

    void DisplayShip::Draw(Game* game)
    {
        if (!mVisible)
        {
            return;
        }

        SDL_Rect r = GetScreenRect();
        mShip.SetPosition({static_cast<float>(r.x + r.w / 2), static_cast<float>(r.y + r.h / 2)});
        mShip.Draw(game);
    }
} // namespace nuvelocity::frs42
