#ifndef NVE_FLOATING_BRICK_H
#define NVE_FLOATING_BRICK_H

#include "model/Model.h"
#include <string>

namespace nuvelocity::frs42
{
    class FloatingBrick : public Object<FloatingBrick>
    {
    public:
        FloatingBrick();
        ~FloatingBrick();

        static void InitClassInfo(ClassInfo& aInfo)
        {
            aInfo.mName = "CFloatingBrick";
            AddProperty(aInfo, "Position X", &FloatingBrick::mPositionX);
            AddProperty(aInfo, "Position Y", &FloatingBrick::mPositionY);
            AddProperty(aInfo, "Force Power-up", &FloatingBrick::mForcePowerUp);
            AddProperty(aInfo, "Speed", &FloatingBrick::mSpeed);
            AddProperty(aInfo, "Range 1", &FloatingBrick::mRange1);
            AddProperty(aInfo, "Range 2", &FloatingBrick::mRange2);
            AddProperty(aInfo, "Direction", &FloatingBrick::mDirection);
            AddProperty(aInfo, "Can Move Through Other Bricks",
                        &FloatingBrick::mCanMoveThroughOtherBricks);
            AddProperty(aInfo, "Palette Index", &FloatingBrick::mPaletteIndex);
            AddProperty(aInfo, "Brick To Change From", &FloatingBrick::mBrickToChangeFrom);
            AddProperty(aInfo, "Brick To Change To", &FloatingBrick::mBrickToChangeTo);
            AddProperty(aInfo, "Brick To Look Like", &FloatingBrick::mBrickToLookLike);
        }

    private:
        int mPositionX;
        int mPositionY;
        std::string mForcePowerUp;
        int mSpeed;
        int mRange1;
        int mRange2;
        int mDirection;
        bool mCanMoveThroughOtherBricks;
        int mPaletteIndex;
        std::string mBrickToChangeFrom;
        std::string mBrickToChangeTo;
        std::string mBrickToLookLike;
    };
} // namespace nuvelocity::frs42

#endif // NVE_FLOATING_BRICK_H
