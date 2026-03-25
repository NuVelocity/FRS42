#include "BrickLayout.h"

namespace nuvelocity::frs42
{
    BrickLayout::BrickLayout()
            : mDisplayName("Unnamed Round")
            , mAuthor("")
            , mBackgroundType("Environments/!None")
            , mBrickPalette({})
            , mRowsOffsets({})
            , mUseDefaultPowerUpWeights(true)
            , mWeightingComment("")
            , mPowerUpWeights({})
            , mLayout(nullptr)
            , mFloatingBricks({})
    {
    }

    BrickLayout::~BrickLayout() = default;
} // namespace nuvelocity::frs42
