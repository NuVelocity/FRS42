#include "Difficulty.h"

namespace nuvelocity::frs42
{
    const char* DifficultyToString(Difficulty difficulty)
    {
        switch (difficulty)
        {
        case Difficulty::Easy:
            return "Easy";
        case Difficulty::Normal:
            return "Normal";
        case Difficulty::Hard:
            return "Hard";
        case Difficulty::Insane:
            return "Insane";
        default:
            return "Unknown";
        }
    }

    const char* DifficultyToString(int value)
    {
        switch (value)
        {
        case static_cast<int>(Difficulty::Easy):
            return "Easy";
        case static_cast<int>(Difficulty::Normal):
            return "Normal";
        case static_cast<int>(Difficulty::Hard):
            return "Hard";
        case static_cast<int>(Difficulty::Insane):
            return "Insane";
        default:
            return "Unknown";
        }
    }

    Difficulty DifficultyFromInt(int value)
    {
        switch (value)
        {
        case static_cast<int>(Difficulty::Easy):
            return Difficulty::Easy;
        case static_cast<int>(Difficulty::Normal):
            return Difficulty::Normal;
        case static_cast<int>(Difficulty::Hard):
            return Difficulty::Hard;
        case static_cast<int>(Difficulty::Insane):
            return Difficulty::Insane;
        default:
            return Difficulty::Normal;
        }
    }

    int DifficultyToInt(Difficulty difficulty)
    {
        return static_cast<int>(difficulty);
    }

    const std::array<Difficulty, 4>& GetAllDifficulties()
    {
        static const std::array<Difficulty, 4> kAllDifficulties = {
            Difficulty::Easy, Difficulty::Normal, Difficulty::Hard, Difficulty::Insane};
        return kAllDifficulties;
    }
} // namespace nuvelocity::frs42
