#ifndef FRS42_LIST_ENTRIES_H
#define FRS42_LIST_ENTRIES_H

#include <string>

namespace nuvelocity::frs42
{
    struct PlayerEntry
    {
        std::string name;
        int gamesPlayed;
        double totalTime;
        int highestRoundIndex;
        int bestScore;
    };

    struct HighScoreEntry
    {
        std::string playerName;
        int score;
        int difficulty;
        double time;
        int startRound;
        int endRound;
    };

    struct RoundEntry
    {
        std::string setPath;
        std::string setName;
        std::string name;
        int roundIndex;  // Index within the round set
        int setIndex;    // Index of the round set
        int globalIndex; // Global index across all rounds
    };
} // namespace nuvelocity::frs42

#endif // FRS42_LIST_ENTRIES_H
