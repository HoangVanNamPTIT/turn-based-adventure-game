/**
 * @file TeamManager.h
 * @module Team
 * @brief Manages collection of Team objects and persistence.
 * @features create/delete/rename teams, add/remove characters by id, validation and listing.
 * @input Team definitions and character existence from roster.
 * @output Team collection for battle orchestration.
 */

#ifndef TURN_BASED_ADVENTURE_GAME_TEAM_MANAGER_H
#define TURN_BASED_ADVENTURE_GAME_TEAM_MANAGER_H

#include <cstddef>
#include <string>
#include <vector>

#include "Team.h"

namespace TurnBasedGame {

class CharacterRoster;

class TeamManager {
public:

    static const std::size_t MAX_CHARACTERS_PER_TEAM = 5;

    TeamManager() = default;
    ~TeamManager() = default;

    bool createTeam(int teamId, const std::string& teamName);
    bool deleteTeam(int teamId);
    bool renameTeam(int teamId, const std::string& newName);

    Team* getTeamById(int teamId);
    const Team* getTeamById(int teamId) const;

    bool teamExists(int teamId) const;
    bool teamNameExists(const std::string& teamName) const;

    bool addCharacterToTeam(int teamId,
                            int characterId,
                            const CharacterRoster& roster);
    bool removeCharacterFromTeam(int teamId, int characterId);
    bool removeCharacterFromAllTeams(int characterId);

    const std::vector<Team>& getAllTeams() const;

    void displayTeam(int teamId) const;
    void displayAllTeams() const;

    void clear();

    bool load(const std::string& filePath,
              const CharacterRoster& roster);
    bool save(const std::string& filePath) const;

private:

    using TeamIterator = std::vector<Team>::iterator;
    using ConstTeamIterator = std::vector<Team>::const_iterator;

    TeamIterator findTeamById(int teamId);
    ConstTeamIterator findTeamById(int teamId) const;
    bool isNameUsedByAnotherTeam(const std::string& teamName,
                                 int exceptTeamId = -1) const;
    bool isValidTeamId(int teamId) const;
    bool isValidTeamName(const std::string& teamName) const;

    std::vector<Team> m_teams;
};

} // namespace TurnBasedGame

#endif // TURN_BASED_ADVENTURE_GAME_TEAM_MANAGER_H
