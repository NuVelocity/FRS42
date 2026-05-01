#ifndef FRS42_DIFFICULTY_H
#define FRS42_DIFFICULTY_H

#include <array>

namespace nuvelocity::frs42
{
    enum class Difficulty : int
    {
        Easy = 0,
        Normal = 1,
        Hard = 2,
        Insane = 3
    };

    const char* DifficultyToString(Difficulty difficulty);
    const char* DifficultyToString(int value);
    Difficulty DifficultyFromInt(int value);
    int DifficultyToInt(Difficulty difficulty);
    const std::array<Difficulty, 4>& GetAllDifficulties();
} // namespace nuvelocity::frs42

#endif // FRS42_DIFFICULTY_H
