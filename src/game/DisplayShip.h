#ifndef FRS42_DISPLAY_SHIP_H
#define FRS42_DISPLAY_SHIP_H

#include "Ship.h"
#include <system/ui/Widget.h>

namespace nuvelocity::frs42
{
    class DisplayShip : public Widget
    {
    public:
        DisplayShip();
        virtual ~DisplayShip() = default;

        void Load(Game* game);
        void Draw(Game* game) override;

        Ship* GetShip()
        {
            return &mShip;
        }

    private:
        Ship mShip;
    };
} // namespace nuvelocity::frs42

#endif // FRS42_DISPLAY_SHIP_H
