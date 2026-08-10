/**
 * @file TeamManager.cpp
 * @module Team
 * @brief Manages collection of Team objects and persistence.
 * @features create/delete/rename teams, add/remove characters by id, validation and listing.
 * @input Team definitions and character existence from roster.
 * @output Team collection for battle orchestration.
 */

#include "TeamManager.h"
#include "CharacterRoster.h"
#include "DataFileManager.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>

namespace TurnBasedGame {

bool TeamManager::createTeam(
    int teamId,
    const std::string& teamName) {

    if (!isValidTeamId(teamId)
        || !isValidTeamName(teamName)
        || teamExists(teamId)
        || teamNameExists(teamName)) {
        return false;
    }

    m_teams.push_back(std::unique_ptr<Team>(new Team(teamId, teamName)));
    return true;
}

bool TeamManager::deleteTeam(int teamId) {
    TeamIterator team = findTeamById(teamId);
    if (team == m_teams.end()) {
        return false;
    }

    m_teams.erase(team);
    return true;
}

bool TeamManager::renameTeam(
    int teamId,
    const std::string& newName) {

    if (!isValidTeamName(newName)) {
        return false;
    }

    TeamIterator team = findTeamById(teamId);
    if (team == m_teams.end()) {
        return false;
    }

    if ((*team)->getName() == newName) {
        return true;
    }

    if (isNameUsedByAnotherTeam(newName, teamId)) {
        return false;
    }

    (*team)->setName(newName);
    return true;
}

Team* TeamManager::getTeamById(int teamId) {
    TeamIterator team = findTeamById(teamId);
    return team == m_teams.end() ? nullptr : team->get();
}

const Team* TeamManager::getTeamById(int teamId) const {
    ConstTeamIterator team = findTeamById(teamId);
    return team == m_teams.cend() ? nullptr : team->get();
}

bool TeamManager::teamExists(int teamId) const {
    return findTeamById(teamId) != m_teams.cend();
}

bool TeamManager::teamNameExists(
    const std::string& teamName) const {
    return isNameUsedByAnotherTeam(teamName, -1);
}

bool TeamManager::addCharacterToTeam(
    int teamId,
    int characterId,
    const CharacterRoster& roster) {

    TeamIterator team = findTeamById(teamId);
    if (team == m_teams.end()) {
        return false;
    }

    if (characterId <= 0
        || roster.findById(characterId) == nullptr
        || (*team)->hasCharacterId(characterId)
        || (*team)->getCharacterIds().size() >= MAX_CHARACTERS_PER_TEAM) {
        return false;
    }

    (*team)->addCharacterId(characterId);
    return true;
}

bool TeamManager::removeCharacterFromTeam(
    int teamId,
    int characterId) {

    TeamIterator team = findTeamById(teamId);
    if (team == m_teams.end()) {
        return false;
    }

    return (*team)->removeCharacterId(characterId);
}

bool TeamManager::removeCharacterFromAllTeams(int characterId) {
    if (characterId <= 0) {
        return false;
    }

    for (auto& team : m_teams) {
        if (team != nullptr) {
            team->removeCharacterId(characterId);
        }
    }

    return true;
}

std::vector<Team> TeamManager::getAllTeams() const {
    std::vector<Team> result;
    result.reserve(m_teams.size());
    for (const auto& team : m_teams) {
        if (team != nullptr) {
            result.push_back(*team);
        }
    }
    return result;
}

void TeamManager::clear() {
    m_teams.clear();
}

bool TeamManager::load(
    const std::string& filePath,
    const CharacterRoster& roster) {

    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "TeamManager: Failed to open team file: " << filePath << std::endl;
        return false;
    }

    std::vector<std::unique_ptr<Team>> loadedTeams;
    std::string line;
    std::size_t lineNumber = 0;

    while (std::getline(file, line)) {
        ++lineNumber;
        if (line.empty()) {
            continue;
        }

        Team team;
        if (!DataFileManager::parseTeamLine(line, team)) {
            std::cerr << "TeamManager: Skipping invalid team line "
                      << lineNumber << "." << std::endl;
            continue;
        }

        if (!isValidTeamId(team.getId())
            || !isValidTeamName(team.getName())
            || team.getCharacterIds().size() > MAX_CHARACTERS_PER_TEAM
            || std::any_of(
                   loadedTeams.begin(),
                   loadedTeams.end(),
                   [&team](const std::unique_ptr<Team>& existing) {
                       return existing != nullptr && existing->getId() == team.getId();
                   })
            || std::any_of(
                   loadedTeams.begin(),
                   loadedTeams.end(),
                   [&team](const std::unique_ptr<Team>& existing) {
                       return existing != nullptr && existing->getName() == team.getName();
                   })) {
            std::cerr << "TeamManager: Skipping invalid or duplicate team line "
                      << lineNumber << "." << std::endl;
            continue;
        }

        const auto& characterIds = team.getCharacterIds();
        bool valid = true;
        std::vector<int> seenIds;
        seenIds.reserve(characterIds.size());

        for (int characterId : characterIds) {
            if (characterId <= 0
                || roster.findById(characterId) == nullptr
                || std::find(seenIds.begin(), seenIds.end(), characterId) != seenIds.end()) {
                valid = false;
                break;
            }
            seenIds.push_back(characterId);
        }

        if (!valid) {
            std::cerr << "TeamManager: Skipping team with invalid character IDs on line "
                      << lineNumber << "." << std::endl;
            continue;
        }

        loadedTeams.push_back(std::unique_ptr<Team>(new Team(team)));
    }

    file.close();
    m_teams = std::move(loadedTeams);
    return true;
}

bool TeamManager::save(const std::string& filePath) const {
    return DataFileManager::saveTeams(filePath, m_teams);
}

TeamManager::TeamIterator TeamManager::findTeamById(int teamId) {
    return std::find_if(
        m_teams.begin(),
        m_teams.end(),
        [teamId](const std::unique_ptr<Team>& team) {
            return team != nullptr && team->getId() == teamId;
        });
}

TeamManager::ConstTeamIterator TeamManager::findTeamById(
    int teamId) const {
    return std::find_if(
        m_teams.cbegin(),
        m_teams.cend(),
        [teamId](const std::unique_ptr<Team>& team) {
            return team != nullptr && team->getId() == teamId;
        });
}

bool TeamManager::isNameUsedByAnotherTeam(
    const std::string& teamName,
    int exceptTeamId) const {

    // So sánh không phân biệt hoa/thường để nhất quán với UI layer
    std::string lowerName = teamName;
    std::transform(
        lowerName.begin(), lowerName.end(), lowerName.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    return std::any_of(
        m_teams.cbegin(),
        m_teams.cend(),
        [lowerName, exceptTeamId](const std::unique_ptr<Team>& team) {
            if (team == nullptr) return false;
            std::string existingLower = team->getName();
            std::transform(
                existingLower.begin(),
                existingLower.end(),
                existingLower.begin(),
                [](unsigned char c) {
                    return static_cast<char>(std::tolower(c));
                });
            return existingLower == lowerName
                && team->getId() != exceptTeamId;
        });
}

bool TeamManager::isValidTeamId(int teamId) const {
    return teamId > 0;
}

bool TeamManager::isValidTeamName(
    const std::string& teamName) const {
    if (teamName.empty()) {
        return false;
    }

    if (teamName.find('|') != std::string::npos
        || teamName.find(',') != std::string::npos) {
        return false;
    }

    return std::any_of(
        teamName.begin(),
        teamName.end(),
        [](char character) {
            return !std::isspace(static_cast<unsigned char>(character));
        });
}

} // namespace TurnBasedGame
