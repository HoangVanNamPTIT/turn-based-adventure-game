/**
 * @file Character.cpp
 * @module Domain Model
 * @brief Implementation of the abstract Character base class.
 */

#include "Character.h"
#include <algorithm>

namespace TurnBasedGame {

namespace {

bool isValidName(const std::string& name) {
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

Character::Character(int id, const std::string& name, int maxHp, const std::string& type):
      m_id(id),
      m_name(name),
      m_maxHp(maxHp),
      m_currentHp(maxHp),
      m_type(type) {
}

int Character::getId() const {
    return m_id;
}

const std::string& Character::getName() const {
    return m_name;
}

int Character::getMaxHp() const {
    return m_maxHp;
}

int Character::getCurrentHp() const {
    return m_currentHp;
}

const std::string& Character::getType() const {
    return m_type;
}

bool Character::isAlive() const {
    return m_currentHp > 0;
}

bool Character::setName(const std::string& name) {
    if (!isValidName(name)) {
        return false;
    }
    m_name = name;
    return true;
}

bool Character::setMaxHp(int maxHp) {
    if (maxHp <= 0) {
        return false;
    }
    m_maxHp = maxHp;
    if (m_currentHp > m_maxHp) {
        m_currentHp = m_maxHp;
    }
    return true;
}

void Character::setCurrentHp(int currentHp) {
    m_currentHp = std::max(0, std::min(currentHp, m_maxHp));
}

bool Character::takeDamage(int amount) {
    if (amount <= 0) {
        return false;
    }
    m_currentHp = std::max(0, m_currentHp - amount);
    return true;
}

bool Character::heal(int amount) {
    if (amount <= 0) {
        return false;
    }
    m_currentHp = std::min(m_maxHp, m_currentHp + amount);
    return true;
}

void Character::resetToFullHealth() {
    m_currentHp = m_maxHp;
}

void Character::resetBattleState() {
    resetToFullHealth();
}

} // namespace TurnBasedGame
