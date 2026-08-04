/**
 * @file Team.cpp
 * @module Team
 * @brief Skeleton implementation for Team. Keeps only basic constructors and stubs.
 */

#include "Team.h"
#include <algorithm>

namespace TurnBasedGame {

Team::Team(int id, const std::string& name, const std::vector<int>& characterIds)
    : m_id(id > 0 ? id : 0), m_name(name), m_characterIds(characterIds) {}

int Team::getId() const {
    return m_id;
}

void Team::setId(int id) {
    m_id = id > 0 ? id : 0;
}

const std::string& Team::getName() const {
    return m_name;
}

bool Team::setName(const std::string& name) {
    if (name.empty()) {
        return false;
    }
    m_name = name;
    return true;
}

const std::vector<int>& Team::getCharacterIds() const {
    return m_characterIds;
}

void Team::setCharacterIds(const std::vector<int>& ids) {
    m_characterIds = ids;
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
