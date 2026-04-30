#ifndef NVE_RANDOM_DEBRIS_AI_H
#define NVE_RANDOM_DEBRIS_AI_H

#include "BackgroundSpriteAI.h"
#include <string>

namespace nuvelocity::frs42
{
    class RandomDebrisAI : public Object<RandomDebrisAI, BackgroundSpriteAI>
    {
    public:
        static void InitClassInfo(ClassInfo& info)
        {
            info.mName = "CRandomDebrisAI";
            AddProperty(info, "Sequence", &RandomDebrisAI::mSequence);
            AddProperty(info, "Min Speed", &RandomDebrisAI::mMinSpeed);
            AddProperty(info, "Max Speed", &RandomDebrisAI::mMaxSpeed);
            AddProperty(info, "Min Angle", &RandomDebrisAI::mMinAngle);
            AddProperty(info, "Max Angle", &RandomDebrisAI::mMaxAngle);
        }

        std::string mSequence;
        float mMinSpeed = 0.0F;
        float mMaxSpeed = 0.0F;
        float mMinAngle = 0.0F;
        float mMaxAngle = 0.0F;
    };
} // namespace nuvelocity::frs42

#endif // NVE_RANDOM_DEBRIS_AI_H
