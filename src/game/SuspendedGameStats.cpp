#include "SuspendedGameStats.h"

namespace nuvelocity
{
    SuspendedGameStats::SuspendedGameStats() = default;
    SuspendedGameStats::~SuspendedGameStats() = default;

    void SuspendedGameStats::InitClassInfo(ClassInfo& info)
    {
        info.mName = "CSuspendedGameStats";
        AddProperty(info, "Balls Left", &SuspendedGameStats::mBallsLeft);
    }
} // namespace nuvelocity
