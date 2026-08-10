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
#include <memory>
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

    TeamManager(const TeamManager&) = delete;
    TeamManager& operator=(const TeamManager&) = delete;

    TeamManager(TeamManager&&) noexcept = default;
    TeamManager& operator=(TeamManager&&) noexcept = default;

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

    std::vector<Team> getAllTeams() const;

    bool isNameUsedByAnotherTeam(const std::string& teamName,
                                 int exceptTeamId = -1) const;


    void clear();

    bool load(const std::string& filePath,
              const CharacterRoster& roster);
    bool save(const std::string& filePath) const;

private:

    using TeamIterator = std::vector<std::unique_ptr<Team>>::iterator;
    using ConstTeamIterator = std::vector<std::unique_ptr<Team>>::const_iterator;

    TeamIterator findTeamById(int teamId);
    ConstTeamIterator findTeamById(int teamId) const;
    bool isValidTeamId(int teamId) const;
    bool isValidTeamName(const std::string& teamName) const;

    std::vector<std::unique_ptr<Team>> m_teams;
};

} // namespace TurnBasedGame

#endif // TURN_BASED_ADVENTURE_GAME_TEAM_MANAGER_H
