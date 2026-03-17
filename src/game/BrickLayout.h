#ifndef NVE_BRICK_LAYOUT_H
#define NVE_BRICK_LAYOUT_H

#include "FloatingBrick.h"
#include "Frame.h"
#include "model/Model.h"
#include <string>

namespace nuvelocity::frs42
{
    class BrickLayout : public Object<BrickLayout>
    {
    public:
        BrickLayout();
        ~BrickLayout();

        static void InitClassInfo(ClassInfo& aInfo)
        {
            aInfo.mName = "CBrickLayout";
            AddProperty(aInfo, "Display Name", &BrickLayout::mDisplayName);
            AddProperty(aInfo, "Author", &BrickLayout::mAuthor);
            AddProperty(aInfo, "Background Type", &BrickLayout::mBackgroundType);
            AddProperty(aInfo, "Brick Palette", &BrickLayout::mBrickPalette, "Brick Type");
            AddProperty(aInfo, "Row Offsets", &BrickLayout::mRowOffsets, "X Offset");
            AddProperty(aInfo, "Use Default Power Up Weights",
                        &BrickLayout::mUseDefaultPowerUpWeights);
            AddProperty(aInfo, "Weighting Comment", &BrickLayout::mWeightingComment);
            AddProperty(aInfo, "Power Up Weights", &BrickLayout::mPowerUpWeights);
            AddProperty(aInfo, "Layout", &BrickLayout::mLayout);
            AddProperty(aInfo, "Floating Bricks", &BrickLayout::mFloatingBricks, "Brick");
        }

    private:
        std::string mDisplayName;
        std::string mAuthor;
        std::string mBackgroundType;
        std::vector<std::string> mBrickPalette;
        std::vector<int> mRowOffsets;
        bool mUseDefaultPowerUpWeights;
        std::string mWeightingComment;
        std::map<std::string, int> mPowerUpWeights;
        Frame* mLayout;
        std::vector<FloatingBrick*> mFloatingBricks;
    };
} // namespace nuvelocity::frs42

#endif // NVE_BRICK_LAYOUT_H
