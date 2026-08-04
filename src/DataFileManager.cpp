/**
 * @file DataFileManager.cpp
 * @module Persistence
 * @brief Skeleton implementation for DataFileManager. IO logic intentionally omitted.
 */

#include "DataFileManager.h"
#include "Utils.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace TurnBasedGame {

std::shared_ptr<Character> DataFileManager::parseCharacterLine(const std::string& rawLine) {
    std::string line = Utils::Utils::trim(rawLine);
    if (line.empty() || line[0] == '#') {
        return nullptr;
    }

    std::vector<std::string> tokens = Utils::Utils::split(line, '|');
    if (tokens.empty()) {
        return nullptr;
    }

    std::string type = tokens[0];
    if (type == "WARRIOR") {
        if (tokens.size() < 5) {
            return nullptr;
        }
        int id = Utils::Utils::parseInt(tokens[1], 0);
        std::string name = tokens[2];
        int maxHp = Utils::Utils::parseInt(tokens[3], 0);
        int attackPower = Utils::Utils::parseInt(tokens[4], 0);

        if (id <= 0 || name.empty() || maxHp <= 0 || attackPower <= 0) {
            return nullptr;
        }
        return std::make_shared<Warrior>(id, name, maxHp, attackPower);
    } else if (type == "MAGE") {
        if (tokens.size() < 8) {
            return nullptr;
        }
        int id = Utils::Utils::parseInt(tokens[1], 0);
        std::string name = tokens[2];
        int maxHp = Utils::Utils::parseInt(tokens[3], 0);
        int maxMana = Utils::Utils::parseInt(tokens[4], 0);
        int spellDamage = Utils::Utils::parseInt(tokens[5], 0);
        int manaCost = Utils::Utils::parseInt(tokens[6], 0);
        int fallbackDamage = Utils::Utils::parseInt(tokens[7], 0);

        if (id <= 0 || name.empty() || maxHp <= 0 || maxMana <= 0 || spellDamage <= 0 || manaCost <= 0 || fallbackDamage <= 0) {
            return nullptr;
        }
        return std::make_shared<Mage>(id, name, maxHp, maxMana, spellDamage, manaCost, fallbackDamage);
    }

    return nullptr;
}

std::string DataFileManager::serializeCharacter(const Character& character) {
    if (character.getType() == "WARRIOR") {
        const auto* warrior = dynamic_cast<const Warrior*>(&character);
        if (warrior) {
            std::ostringstream oss;
            oss << "WARRIOR|" << warrior->getId() << "|"
                << warrior->getName() << "|"
                << warrior->getMaxHp() << "|"
                << warrior->getAttackPower();
            return oss.str();
        }
    } else if (character.getType() == "MAGE") {
        const auto* mage = dynamic_cast<const Mage*>(&character);
        if (mage) {
            std::ostringstream oss;
            oss << "MAGE|" << mage->getId() << "|"
                << mage->getName() << "|"
                << mage->getMaxHp() << "|"
                << mage->getMaxMana() << "|"
                << mage->getSpellDamage() << "|"
                << mage->getManaCost() << "|"
                << mage->getFallbackDamage();
            return oss.str();
        }
    }
    return "";
}

bool DataFileManager::parseTeamLine(const std::string& rawLine, Team& outTeam) {
    std::string line = Utils::Utils::trim(rawLine);
    if (line.empty() || line[0] == '#') {
        return false;
    }

    std::vector<std::string> tokens = Utils::Utils::split(line, '|');
    if (tokens.size() < 2) {
        return false;
    }

    int teamId = Utils::Utils::parseInt(tokens[0], 0);
    std::string teamName = tokens[1];
    if (teamId <= 0 || teamName.empty()) {
        return false;
    }

    std::vector<int> charIds;
    if (tokens.size() >= 3 && !tokens[2].empty()) {
        std::vector<std::string> idTokens = Utils::Utils::split(tokens[2], ',');
        for (const auto& idStr : idTokens) {
            if (!idStr.empty() && Utils::Utils::isInteger(idStr)) {
                int charId = Utils::Utils::parseInt(idStr, 0);
                if (charId > 0) {
                    charIds.push_back(charId);
                }
            }
        }
    }

    outTeam = Team(teamId, teamName, charIds);
    return true;
}

std::string DataFileManager::serializeTeam(const Team& team) {
    std::ostringstream oss;
    oss << team.getId() << "|" << team.getName() << "|";
    const auto& ids = team.getCharacterIds();
    for (size_t i = 0; i < ids.size(); ++i) {
        oss << ids[i];
        if (i + 1 < ids.size()) {
            oss << ",";
        }
    }
    return oss.str();
}

bool DataFileManager::loadCharacters(const std::string& filePath, std::vector<std::shared_ptr<Character>>& outCharacters) {
    outCharacters.clear();
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "DataFileManager: Error opening file for reading characters: " << filePath << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        auto character = parseCharacterLine(line);
        if (character) {
            outCharacters.push_back(character);
        }
    }
    file.close();
    return true;
}

bool DataFileManager::saveCharacters(const std::string& filePath, const std::vector<std::shared_ptr<Character>>& characters) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "DataFileManager: Error opening file for writing characters: " << filePath << std::endl;
        return false;
    }

    for (const auto& character : characters) {
        if (character) {
            std::string serialized = serializeCharacter(*character);
            if (!serialized.empty()) {
                file << serialized << "\n";
            }
        }
    }
    file.close();
    return true;
}

bool DataFileManager::loadTeams(const std::string& filePath, std::vector<Team>& outTeams) {
    outTeams.clear();
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "DataFileManager: Error opening file for reading teams: " << filePath << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        Team team;
        if (parseTeamLine(line, team)) {
            outTeams.push_back(team);
        }
    }
    file.close();
    return true;
}

bool DataFileManager::saveTeams(const std::string& filePath, const std::vector<Team>& teams) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "DataFileManager: Error opening file for writing teams: " << filePath << std::endl;
        return false;
    }

    for (const auto& team : teams) {
        std::string serialized = serializeTeam(team);
        if (!serialized.empty()) {
            file << serialized << "\n";
        }
    }
    file.close();
    return true;
}

} // namespace TurnBasedGame
