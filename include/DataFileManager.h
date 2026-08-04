/**
 * @file DataFileManager.h
 * @module Persistence
 * @brief Handles parsing and serialization of characters and teams to files.
 * @features read/write characters.txt and teams.txt in specified formats.
 * @input file paths and object references
 * @output boolean success and messages via console
 */

#ifndef TURN_BASED_ADVENTURE_GAME_DATA_FILE_MANAGER_H
#define TURN_BASED_ADVENTURE_GAME_DATA_FILE_MANAGER_H

#include <string>
#include <vector>
#include <memory>

#include "Character.h"
#include "Warrior.h"
#include "Mage.h"
#include "Team.h"

namespace TurnBasedGame {

class DataFileManager {
public:
    // Character Read / Write
    static bool loadCharacters(const std::string& filePath, std::vector<std::shared_ptr<Character>>& outCharacters);
    static bool saveCharacters(const std::string& filePath, const std::vector<std::shared_ptr<Character>>& characters);

    // Team Read / Write
    static bool loadTeams(const std::string& filePath, std::vector<Team>& outTeams);
    static bool saveTeams(const std::string& filePath, const std::vector<Team>& teams);

    // Parsing and Serialization helpers
    static std::shared_ptr<Character> parseCharacterLine(const std::string& line);
    static std::string serializeCharacter(const Character& character);

    static bool parseTeamLine(const std::string& line, Team& outTeam);
    static std::string serializeTeam(const Team& team);
};

} // namespace TurnBasedGame

#endif // TURN_BASED_ADVENTURE_GAME_DATA_FILE_MANAGER_H
