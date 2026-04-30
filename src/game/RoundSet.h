#ifndef NVE_ROUND_SET_H
#define NVE_ROUND_SET_H

#include "CheckPointDefinition.h"
#include <Object.h>
#include <string>

namespace nuvelocity::frs42
{
    class RoundSet : public Object<RoundSet>
    {
    public:
        RoundSet();
        ~RoundSet();

        static void InitClassInfo(ClassInfo& info)
        {
            info.mName = "CRoundSet";
            AddProperty(info, "Round List", &RoundSet::mRoundList, "Round");
            AddProperty(info, "Checkpoints", &RoundSet::mCheckPoints);
        }

        const std::vector<std::string>& GetRoundList() const
        {
            return mRoundList;
        }
        const std::vector<CheckPointDefinition*>& GetCheckPoints() const
        {
            return mCheckPoints;
        }

    private:
        std::vector<std::string> mRoundList;
        std::vector<CheckPointDefinition*> mCheckPoints;
    };
} // namespace nuvelocity::frs42

#endif // NVE_ROUND_SET_H
