#ifndef NVE_FISH_AI_H
#define NVE_FISH_AI_H

#include "BackgroundSpriteAI.h"
#include <string>

namespace nuvelocity::frs42
{
    class FishAI : public Object<FishAI, BackgroundSpriteAI>
    {
    public:
        static void InitClassInfo(ClassInfo& info)
        {
            info.mName = "CFishAI";
            AddProperty(info, "Left Sequence", &FishAI::mLeftSequence);
            AddProperty(info, "Right Sequence", &FishAI::mRightSequence);
            AddProperty(info, "Minimum Speed", &FishAI::mMinSpeed);
            AddProperty(info, "Maximum Speed", &FishAI::mMaxSpeed);
        }

        std::string mLeftSequence;
        std::string mRightSequence;
        float mMinSpeed = 0.0F;
        float mMaxSpeed = 0.0F;
    };
} // namespace nuvelocity::frs42

#endif // NVE_FISH_AI_H
