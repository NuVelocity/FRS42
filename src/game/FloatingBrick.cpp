#include "FloatingBrick.h"

namespace nuvelocity::frs42
{
    FloatingBrick::FloatingBrick()
            : mPositionX(0)
            , mPositionY(0)
            , mForcePowerUp("No Power-Up")
            , mSpeed(0)
            , mRange1(0)
            , mRange2(0)
            , mDirection(0)
            // XXX: Is this the default value?
            , mCanMoveThroughOtherBricks(true)
            , mPaletteIndex(0)
            , mBrickToChangeFrom("Bricks/!None")
            , mBrickToChangeTo("Bricks/!None")
            , mBrickToLookLike("Bricks/!None")
    {
    }

    FloatingBrick::~FloatingBrick() = default;
} // namespace nuvelocity::frs42
