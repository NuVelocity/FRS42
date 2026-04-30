#include "BrickLayout.h"

namespace nuvelocity::frs42
{
    BrickLayout::BrickLayout()
            : mDisplayName("Unnamed Round")
            , mBackgroundType("Environments/!None")
            , mBrickPalette({})
            , mRowsOffsets({})
            , mUseDefaultPowerUpWeights(true)
            , mPowerUpWeights({})
            , mLayout(nullptr)
            , mFloatingBricks({})
    {
    }

    BrickLayout::~BrickLayout() = default;
} // namespace nuvelocity::frs42
