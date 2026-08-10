/**
 * @file TeamRecord.h
 * @module Domain / Data Transfer Object
 * @brief Data structure representing team battle statistics history.
 */

#ifndef TURN_BASED_ADVENTURE_GAME_TEAM_RECORD_H
#define TURN_BASED_ADVENTURE_GAME_TEAM_RECORD_H

#include <string>
#include <vector>

namespace TurnBasedGame {

struct TeamRecord {
    int teamId{0};
    std::string teamName;
    std::vector<int> characterIds;
    int wins{0};
    int losses{0};
};

} // namespace TurnBasedGame

#endif // TURN_BASED_ADVENTURE_GAME_TEAM_RECORD_H
