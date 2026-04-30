#ifndef NVE_PLANKTON_AI_H
#define NVE_PLANKTON_AI_H

#include "BackgroundSpriteAI.h"
#include <string>

namespace nuvelocity::frs42
{
    class PlanktonAI : public Object<PlanktonAI, BackgroundSpriteAI>
    {
    public:
        static void InitClassInfo(ClassInfo& info)
        {
            info.mName = "CPlanktonAI";
            AddProperty(info, "Sequence", &PlanktonAI::mSequence);
            AddProperty(info, "Min Speed", &PlanktonAI::mMinSpeed);
            AddProperty(info, "Max Speed", &PlanktonAI::mMaxSpeed);
        }

        std::string mSequence;
        float mMinSpeed = 0.0F;
        float mMaxSpeed = 0.0F;
    };
} // namespace nuvelocity::frs42

#endif // NVE_PLANKTON_AI_H
