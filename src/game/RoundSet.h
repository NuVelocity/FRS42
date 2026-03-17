#ifndef NVE_ROUND_SET_H
#define NVE_ROUND_SET_H

#include "CheckPointDefinition.h"
#include "model/Model.h"
#include <string>

namespace nuvelocity::frs42
{
    class RoundSet : public Object<RoundSet>
    {
    public:
        RoundSet();
        ~RoundSet();

        static void InitClassInfo(ClassInfo& aInfo)
        {
            aInfo.mName = "CRoundSet";
            AddProperty(aInfo, "Round List", &RoundSet::mRoundList, "Round");
            AddProperty(aInfo, "Checkpoints", &RoundSet::mCheckPoints);
        }

    private:
        std::vector<std::string> mRoundList;
        std::vector<CheckPointDefinition*> mCheckPoints;
    };
} // namespace nuvelocity::frs42

#endif // NVE_ROUND_SET_H
