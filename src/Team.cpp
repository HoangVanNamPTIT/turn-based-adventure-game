/**
 * @file Team.cpp
 * @module Team
 * @brief Skeleton implementation for Team. Keeps only basic constructors and stubs.
 */

#include "Team.h"
#include <algorithm>

namespace TurnBasedGame {

namespace {

bool isValidTeamName(const std::string& name) {
    const std::size_t first = name.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return false;
    }
    if (name.find('|') != std::string::npos || name.find(',') != std::string::npos) {
        return false;
    }
    return true;
}

} // namespace

Team::Team(int id, const std::string& name, const std::vector<int>& characterIds)
    : m_id(id), m_name(name), m_characterIds(characterIds) {}

int Team::getId() const {
    return m_id;
}

bool Team::setId(int id) {
    if (id <= 0) {
        return false;
    }
    m_id = id;
    return true;
}

const std::string& Team::getName() const {
    return m_name;
}

bool Team::setName(const std::string& name) {
    if (!isValidTeamName(name)) {
        return false;
    }
    m_name = name;
    return true;
}

const std::vector<int>& Team::getCharacterIds() const {
    return m_characterIds;
}

bool Team::setCharacterIds(const std::vector<int>& ids) {
    if (ids.size() > 5) {
        return false;
    }
    std::vector<int> uniqueIds;
    for (int id : ids) {
        if (id <= 0) {
            return false;
        }
        if (std::find(uniqueIds.begin(), uniqueIds.end(), id) != uniqueIds.end()) {
            return false;
        }
        uniqueIds.push_back(id);
    }
    m_characterIds = ids;
    return true;
}

void Team::addCharacterId(int id) {
    if (id <= 0) return;
    if (!hasCharacterId(id)) {
        m_characterIds.push_back(id);
    }
}

bool Team::removeCharacterId(int id) {
    auto it = std::find(m_characterIds.begin(), m_characterIds.end(), id);
    if (it != m_characterIds.end()) {
        m_characterIds.erase(it);
        return true;
    }
    return false;
}

bool Team::hasCharacterId(int id) const {
    return std::find(m_characterIds.begin(), m_characterIds.end(), id) != m_characterIds.end();
}

} // namespace TurnBasedGame
