#ifndef NVE_BACKGROUND_SPRITE_GENERATOR_H
#define NVE_BACKGROUND_SPRITE_GENERATOR_H

#include "BackgroundSpriteAI.h"
#include <string>

namespace nuvelocity::frs42
{
    class BackgroundSpriteGenerator : public Object<BackgroundSpriteGenerator>
    {
    public:
        static void InitClassInfo(ClassInfo& info)
        {
            info.mName = "CBackgroundSpriteGenerator";
            AddProperty(info, "Type", &BackgroundSpriteGenerator::mType);
            AddProperty(info, "Count", &BackgroundSpriteGenerator::mCount);
            AddProperty(info, "Layer", &BackgroundSpriteGenerator::mLayer);
        }

        BackgroundSpriteAI* mType = nullptr;
        int mCount = 0;
        std::string mLayer;
    };
} // namespace nuvelocity::frs42

#endif // NVE_BACKGROUND_SPRITE_GENERATOR_H
