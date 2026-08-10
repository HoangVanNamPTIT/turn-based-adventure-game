/**
 * @file DataFileManager.cpp
 * @module Persistence
 * @brief Skeleton implementation for DataFileManager. IO logic intentionally omitted.
 */

#include "DataFileManager.h"
#include "CharacterRoster.h"
#include "Utils.h"

#include <algorithm>
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
        if (tokens.size() != 5) {
            return nullptr;
        }
        if (!Utils::Utils::isInteger(tokens[1]) || 
            !Utils::Utils::isInteger(tokens[3]) || 
            !Utils::Utils::isInteger(tokens[4])) {
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
    } else if (type == "ARCHER") {
        if (tokens.size() != 5) {
            return nullptr;
        }
        if (!Utils::Utils::isInteger(tokens[1]) || 
            !Utils::Utils::isInteger(tokens[3]) || 
            !Utils::Utils::isInteger(tokens[4])) {
            return nullptr;
        }
        int id = Utils::Utils::parseInt(tokens[1], 0);
        std::string name = tokens[2];
        int maxHp = Utils::Utils::parseInt(tokens[3], 0);
        int attackPower = Utils::Utils::parseInt(tokens[4], 0);

        if (id <= 0 || name.empty() || maxHp <= 0 || attackPower <= 0) {
            return nullptr;
        }
        return std::make_shared<Archer>(id, name, maxHp, attackPower);
    } else if (type == "MAGE") {
        if (tokens.size() != 8) {
            return nullptr;
        }
        if (!Utils::Utils::isInteger(tokens[1]) || 
            !Utils::Utils::isInteger(tokens[3]) || 
            !Utils::Utils::isInteger(tokens[4]) ||
            !Utils::Utils::isInteger(tokens[5]) ||
            !Utils::Utils::isInteger(tokens[6]) ||
            !Utils::Utils::isInteger(tokens[7])) {
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
    } else if (type == "HEALER") {
        if (tokens.size() != 8) {
            return nullptr;
        }
        if (!Utils::Utils::isInteger(tokens[1]) || 
            !Utils::Utils::isInteger(tokens[3]) || 
            !Utils::Utils::isInteger(tokens[4]) ||
            !Utils::Utils::isInteger(tokens[5]) ||
            !Utils::Utils::isInteger(tokens[6]) ||
            !Utils::Utils::isInteger(tokens[7])) {
            return nullptr;
        }
        int id = Utils::Utils::parseInt(tokens[1], 0);
        std::string name = tokens[2];
        int maxHp = Utils::Utils::parseInt(tokens[3], 0);
        int maxMana = Utils::Utils::parseInt(tokens[4], 0);
        int healAmount = Utils::Utils::parseInt(tokens[5], 0);
        int manaCost = Utils::Utils::parseInt(tokens[6], 0);
        int fallbackDamage = Utils::Utils::parseInt(tokens[7], 0);

        if (id <= 0 || name.empty() || maxHp <= 0 || maxMana <= 0 || healAmount <= 0 || healAmount > maxHp || manaCost <= 0 || fallbackDamage <= 0) {
            return nullptr;
        }
        return std::make_shared<Healer>(id, name, maxHp, maxMana, healAmount, manaCost, fallbackDamage);
    }

    return nullptr;
}

std::string DataFileManager::serializeCharacter(const Character& character) {
    if (character.getId() <= 0 || character.getName().empty() || character.getMaxHp() <= 0) {
        return "";
    }

    if (character.getType() == "WARRIOR") {
        const auto* warrior = dynamic_cast<const Warrior*>(&character);
        if (warrior && warrior->getAttackPower() > 0) {
            std::ostringstream oss;
            oss << "WARRIOR|" << warrior->getId() << "|"
                << warrior->getName() << "|"
                << warrior->getMaxHp() << "|"
                << warrior->getAttackPower();
            return oss.str();
        }
    } else if (character.getType() == "ARCHER") {
        const auto* archer = dynamic_cast<const Archer*>(&character);
        if (archer && archer->getAttackPower() > 0) {
            std::ostringstream oss;
            oss << "ARCHER|" << archer->getId() << "|"
                << archer->getName() << "|"
                << archer->getMaxHp() << "|"
                << archer->getAttackPower();
            return oss.str();
        }
    } else if (character.getType() == "MAGE") {
        const auto* mage = dynamic_cast<const Mage*>(&character);
        if (mage && mage->getMaxMana() > 0 && mage->getSpellDamage() > 0 && mage->getManaCost() > 0 && mage->getFallbackDamage() > 0) {
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
    } else if (character.getType() == "HEALER") {
        const auto* healer = dynamic_cast<const Healer*>(&character);
        if (healer && healer->getMaxMana() > 0 && healer->getHealAmount() > 0 && healer->getHealAmount() <= healer->getMaxHp() && healer->getManaCost() > 0 && healer->getFallbackDamage() > 0) {
            std::ostringstream oss;
            oss << "HEALER|" << healer->getId() << "|"
                << healer->getName() << "|"
                << healer->getMaxHp() << "|"
                << healer->getMaxMana() << "|"
                << healer->getHealAmount() << "|"
                << healer->getManaCost() << "|"
                << healer->getFallbackDamage();
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
    if (tokens.size() != 3) {
        return false;
    }

    if (!Utils::Utils::isInteger(tokens[0])) {
        return false;
    }

    int teamId = Utils::Utils::parseInt(tokens[0], 0);
    std::string teamName = tokens[1];
    if (teamId <= 0 || teamName.empty()) {
        return false;
    }

    std::vector<int> charIds;
    if (!tokens[2].empty()) {
        std::vector<std::string> idTokens = Utils::Utils::split(tokens[2], ',');
        for (const auto& rawIdStr : idTokens) {
            std::string idStr = Utils::Utils::trim(rawIdStr);
            if (idStr.empty() || !Utils::Utils::isInteger(idStr)) {
                return false;
            }
            int charId = Utils::Utils::parseInt(idStr, 0);
            if (charId <= 0) {
                return false;
            }
            if (std::find(charIds.begin(), charIds.end(), charId) != charIds.end()) {
                return false; // Duplicate character ID in team line
            }
            charIds.push_back(charId);
        }
    }

    outTeam = Team(teamId, teamName, charIds);
    return true;
}

std::string DataFileManager::serializeTeam(const Team& team) {
    if (team.getId() <= 0 || team.getName().empty()) {
        return "";
    }

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
    std::size_t lineNumber = 0;
    while (std::getline(file, line)) {
        ++lineNumber;
        std::string trimmed = Utils::Utils::trim(line);
        if (trimmed.empty() || trimmed[0] == '#') {
            continue;
        }

        auto character = parseCharacterLine(line);
        if (character) {
            outCharacters.push_back(character);
        } else {
            std::cerr << "DataFileManager: Bỏ qua dòng nhân vật không hợp lệ tại dòng "
                      << lineNumber << ": Sai định dạng trường hoặc thuộc tính không hợp lệ." << std::endl;
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

bool DataFileManager::saveCharacters(const std::string& filePath, const std::vector<const Character*>& characters) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "DataFileManager: Error opening file for writing characters: " << filePath << std::endl;
        return false;
    }

    for (const Character* character : characters) {
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

bool DataFileManager::saveCharacters(const std::string& filePath, const CharacterRoster& roster) {
    return saveCharacters(filePath, roster.getAllCharacters());
}

bool DataFileManager::loadTeams(const std::string& filePath, std::vector<Team>& outTeams) {
    outTeams.clear();
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "DataFileManager: Error opening file for reading teams: " << filePath << std::endl;
        return false;
    }

    std::string line;
    std::size_t lineNumber = 0;
    while (std::getline(file, line)) {
        ++lineNumber;
        std::string trimmed = Utils::Utils::trim(line);
        if (trimmed.empty() || trimmed[0] == '#') {
            continue;
        }

        Team team;
        if (parseTeamLine(line, team)) {
            outTeams.push_back(team);
        } else {
            std::cerr << "DataFileManager: Bỏ qua dòng đội hình không hợp lệ tại dòng "
                      << lineNumber << ": Sai định dạng trường hoặc số token không hợp lệ." << std::endl;
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

bool DataFileManager::saveTeams(const std::string& filePath, const std::vector<std::unique_ptr<Team>>& teams) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "DataFileManager: Error opening file for writing teams: " << filePath << std::endl;
        return false;
    }

    for (const auto& team : teams) {
        if (team != nullptr) {
            std::string serialized = serializeTeam(*team);
            if (!serialized.empty()) {
                file << serialized << "\n";
            }
        }
    }
    file.close();
    return true;
}

bool DataFileManager::parseTeamStatsLine(const std::string& line, TeamRecord& outRecord) {
    std::string trimmed = Utils::Utils::trim(line);
    if (trimmed.empty() || trimmed[0] == '#') {
        return false;
    }

    std::vector<std::string> tokens = Utils::Utils::split(trimmed, '|');
    if (tokens.size() < 5) {
        return false;
    }

    int teamId = Utils::Utils::parseInt(tokens[0], 0);
    std::string teamName = Utils::Utils::trim(tokens[1]);
    if (teamId <= 0 || teamName.empty()) {
        return false;
    }

    std::vector<int> characterIds;
    std::string idsStr = Utils::Utils::trim(tokens[2]);
    if (!idsStr.empty() && idsStr != "-") {
        std::vector<std::string> idTokens = Utils::Utils::split(idsStr, ',');
        for (const auto& idTok : idTokens) {
            int id = Utils::Utils::parseInt(idTok, 0);
            if (id > 0) {
                characterIds.push_back(id);
            }
        }
    }

    int wins = Utils::Utils::parseInt(tokens[3], 0);
    int losses = Utils::Utils::parseInt(tokens[4], 0);
    if (wins < 0) wins = 0;
    if (losses < 0) losses = 0;

    outRecord.teamId = teamId;
    outRecord.teamName = teamName;
    outRecord.characterIds = characterIds;
    outRecord.wins = wins;
    outRecord.losses = losses;
    return true;
}

std::string DataFileManager::serializeTeamStats(const TeamRecord& record) {
    if (record.teamId <= 0 || record.teamName.empty()) {
        return "";
    }

    std::ostringstream builder;
    builder << record.teamId << "|"
            << record.teamName << "|";

    if (record.characterIds.empty()) {
        builder << "-";
    } else {
        for (std::size_t index = 0; index < record.characterIds.size(); ++index) {
            if (index > 0) {
                builder << ",";
            }
            builder << record.characterIds[index];
        }
    }

    builder << "|" << record.wins << "|" << record.losses;
    return builder.str();
}

bool DataFileManager::loadTeamStats(const std::string& filePath, std::vector<TeamRecord>& outStats) {
    outStats.clear();
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        TeamRecord rec;
        if (parseTeamStatsLine(line, rec)) {
            outStats.push_back(rec);
        }
    }
    file.close();
    return true;
}

bool DataFileManager::saveTeamStats(const std::string& filePath, const std::vector<TeamRecord>& stats) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        return false;
    }

    for (const auto& rec : stats) {
        std::string serialized = serializeTeamStats(rec);
        if (!serialized.empty()) {
            file << serialized << "\n";
        }
    }
    file.close();
    return true;
}

bool DataFileManager::recordBattleResult(const std::string& filePath, const Team& winnerTeam, const Team& loserTeam) {
    std::vector<TeamRecord> stats;
    loadTeamStats(filePath, stats);

    bool winnerFound = false;
    bool loserFound = false;

    for (auto& rec : stats) {
        if (rec.teamId == winnerTeam.getId()) {
            rec.teamName = winnerTeam.getName();
            rec.characterIds = winnerTeam.getCharacterIds();
            rec.wins += 1;
            winnerFound = true;
        } else if (rec.teamId == loserTeam.getId()) {
            rec.teamName = loserTeam.getName();
            rec.characterIds = loserTeam.getCharacterIds();
            rec.losses += 1;
            loserFound = true;
        }
    }

    if (!winnerFound) {
        TeamRecord wRec;
        wRec.teamId = winnerTeam.getId();
        wRec.teamName = winnerTeam.getName();
        wRec.characterIds = winnerTeam.getCharacterIds();
        wRec.wins = 1;
        wRec.losses = 0;
        stats.push_back(wRec);
    }

    if (!loserFound) {
        TeamRecord lRec;
        lRec.teamId = loserTeam.getId();
        lRec.teamName = loserTeam.getName();
        lRec.characterIds = loserTeam.getCharacterIds();
        lRec.wins = 0;
        lRec.losses = 1;
        stats.push_back(lRec);
    }

    return saveTeamStats(filePath, stats);
}

} // namespace TurnBasedGame
