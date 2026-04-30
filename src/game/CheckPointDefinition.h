#ifndef NVE_CHECK_POINT_DEFINITION_H
#define NVE_CHECK_POINT_DEFINITION_H

#include <Object.h>
#include <string>

namespace nuvelocity::frs42
{
    class CheckPointDefinition : public Object<CheckPointDefinition>
    {
    public:
        CheckPointDefinition();
        ~CheckPointDefinition();

        static void InitClassInfo(ClassInfo& info)
        {
            info.mName = "CCheckPointDefinition";
            AddProperty(info, "Round Number", &CheckPointDefinition::mRoundNumber);
            AddProperty(info, "Image", &CheckPointDefinition::mImage);
        }

        int GetRoundNumber() const
        {
            return mRoundNumber;
        }
        const std::string& GetImage() const
        {
            return mImage;
        }

    private:
        int mRoundNumber;
        std::string mImage;
    };
} // namespace nuvelocity::frs42

#endif // NVE_CHECK_POINT_DEFINITION_H
