#ifndef NVE_FLOATING_BRICK_H
#define NVE_FLOATING_BRICK_H

#include <Object.h>
#include <string>

namespace nuvelocity::frs42
{
    class FloatingBrick : public Object<FloatingBrick>
    {
    public:
        FloatingBrick();
        ~FloatingBrick();

        static void InitClassInfo(ClassInfo& info)
        {
            info.mName = "CFloatingBrick";
            AddProperty(info, "Position X", &FloatingBrick::mPositionX);
            AddProperty(info, "Position Y", &FloatingBrick::mPositionY);
            AddProperty(info, "Force Power-up", &FloatingBrick::mForcePowerUp);
            AddProperty(info, "Speed", &FloatingBrick::mSpeed);
            AddProperty(info, "Range 1", &FloatingBrick::mRange1);
            AddProperty(info, "Range 2", &FloatingBrick::mRange2);
            AddProperty(info, "Direction", &FloatingBrick::mDirection);
            AddProperty(
                info, "Can Move Through Other Bricks", &FloatingBrick::mCanMoveThroughOtherBricks);
            AddProperty(info, "Palette Index", &FloatingBrick::mPaletteIndex);
            AddProperty(info, "Brick To Change From", &FloatingBrick::mBrickToChangeFrom);
            AddProperty(info, "Brick To Change To", &FloatingBrick::mBrickToChangeTo);
            AddProperty(info, "Brick To Look Like", &FloatingBrick::mBrickToLookLike);
        }

        int GetX() const
        {
            return mPositionX;
        }
        int GetY() const
        {
            return mPositionY;
        }
        int GetPaletteIndex() const
        {
            return mPaletteIndex;
        }
        int GetSpeed() const
        {
            return mSpeed;
        }
        int GetDirection() const
        {
            return mDirection;
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
