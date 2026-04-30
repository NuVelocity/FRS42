#ifndef NVE_BRICK_LAYOUT_H
#define NVE_BRICK_LAYOUT_H

#include "FloatingBrick.h"
#include "Frame.h"
#include <Object.h>
#include <string>

namespace nuvelocity::frs42
{
    class BrickLayout : public Object<BrickLayout>
    {
    public:
        BrickLayout();
        ~BrickLayout();

        static void InitClassInfo(ClassInfo& info)
        {
            info.mName = "CBrickLayout";
            AddProperty(info, "Display Name", &BrickLayout::mDisplayName);
            AddProperty(info, "Author", &BrickLayout::mAuthor);
            AddProperty(info, "Background Type", &BrickLayout::mBackgroundType);
            AddProperty(info, "Brick Palette", &BrickLayout::mBrickPalette, "Brick Type");
            AddProperty(info, "Rows Offsets", &BrickLayout::mRowsOffsets, "X Offset");
            AddProperty(
                info, "Use Default Power Up Weights", &BrickLayout::mUseDefaultPowerUpWeights);
            AddProperty(info, "Weighting Comment", &BrickLayout::mWeightingComment);
            AddProperty(info, "Power Up Weights", &BrickLayout::mPowerUpWeights);
            AddProperty(info, "Layout", &BrickLayout::mLayout);
            AddProperty(info, "Floating Bricks", &BrickLayout::mFloatingBricks, "Brick");
        }

        const std::string& GetDisplayName() const
        {
            return mDisplayName;
        }
        const std::string& GetBackgroundType() const
        {
            return mBackgroundType;
        }
        const std::vector<std::string>& GetBrickPalette() const
        {
            return mBrickPalette;
        }
        Frame* GetLayout() const
        {
            return mLayout;
        }
        const std::vector<FloatingBrick*>& GetFloatingBricks() const
        {
            return mFloatingBricks;
        }
        const std::map<std::string, int>& GetPowerUpWeights() const
        {
            return mPowerUpWeights;
        }

    private:
        std::string mDisplayName;
        std::string mAuthor;
        std::string mBackgroundType;
        std::vector<std::string> mBrickPalette;
        std::vector<int> mRowsOffsets;
        bool mUseDefaultPowerUpWeights;
        std::string mWeightingComment;
        std::map<std::string, int> mPowerUpWeights;
        Frame* mLayout;
        std::vector<FloatingBrick*> mFloatingBricks;

        friend class DebugPrinter;
    };
} // namespace nuvelocity::frs42

#endif // NVE_BRICK_LAYOUT_H
