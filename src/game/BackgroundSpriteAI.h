#ifndef NVE_BACKGROUND_SPRITE_AI_H
#define NVE_BACKGROUND_SPRITE_AI_H

#include <Object.h>

namespace nuvelocity::frs42
{
    class BackgroundSpriteAI : public Object<BackgroundSpriteAI>
    {
    public:
        virtual ~BackgroundSpriteAI() = default;

        static void InitClassInfo(ClassInfo& info)
        {
            info.mName = "CBackgroundSpriteAI";
        }
    };
} // namespace nuvelocity::frs42

#endif // NVE_BACKGROUND_SPRITE_AI_H
