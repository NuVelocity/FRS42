#ifndef NVE_CHECK_POINT_DEFINITION_H
#define NVE_CHECK_POINT_DEFINITION_H

#include "model/Model.h"
#include <string>

namespace nuvelocity::frs42
{
    class CheckPointDefinition : public Object<CheckPointDefinition>
    {
    public:
        CheckPointDefinition();
        ~CheckPointDefinition();

        static void InitClassInfo(ClassInfo& aInfo)
        {
            aInfo.mName = "CCheckPointDefinition";
            AddProperty(aInfo, "Round Number", &CheckPointDefinition::mRoundNumber);
            AddProperty(aInfo, "Image", &CheckPointDefinition::mImage);
        }

    private:
        int mRoundNumber;
        std::string mImage;
    };
} // namespace nuvelocity::frs42

#endif // NVE_CHECK_POINT_DEFINITION_H
