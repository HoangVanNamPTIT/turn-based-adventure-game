/**
 * @file Mage.cpp
 * @module Domain Model
 * @brief Implementation of the Mage character.
 */

#include "Mage.h"

#include <algorithm>

namespace TurnBasedGame {

Mage::Mage(int id, const std::string& name, int maxHp, int maxMana,
           int spellDamage, int manaCost, int fallbackDamage) : 
            Character(id, name, maxHp, "MAGE"),
            m_maxMana(maxMana > 0 ? maxMana : 1),
            m_currentMana(maxMana > 0 ? maxMana : 1),
            m_spellDamage(spellDamage > 0 ? spellDamage : 1),
            m_manaCost(manaCost > 0 ? manaCost : 1),
            m_fallbackDamage(fallbackDamage > 0 ? fallbackDamage : 1) {
}

int Mage::getMaxMana() const {
    return m_maxMana;
}

int Mage::getCurrentMana() const {
    return m_currentMana;
}

int Mage::getSpellDamage() const {
    return m_spellDamage;
}

int Mage::getManaCost() const {
    return m_manaCost;
}

int Mage::getFallbackDamage() const {
    return m_fallbackDamage;
}

bool Mage::setMaxMana(int maxMana) {
    if (maxMana <= 0) {
        return false;
    }
    m_maxMana = maxMana;
    m_currentMana = std::min(m_currentMana, m_maxMana);
    return true;
}

bool Mage::setCurrentMana(int currentMana) {
    if (currentMana < 0) {
        return false;
    }
    m_currentMana = std::min(currentMana, m_maxMana);
    return true;
}

bool Mage::setSpellDamage(int spellDamage) {
    if (spellDamage <= 0) {
        return false;
    }
    m_spellDamage = spellDamage;
    return true;
}

bool Mage::setManaCost(int manaCost) {
    if (manaCost <= 0) {
        return false;
    }
    m_manaCost = manaCost;
    return true;
}

bool Mage::setFallbackDamage(int fallbackDamage) {
    if (fallbackDamage <= 0) {
        return false;
    }
    m_fallbackDamage = fallbackDamage;
    return true;
}

void Mage::resetMana() {
    m_currentMana = m_maxMana;
}

void Mage::resetBattleState() {
    resetToFullHealth();
    resetMana();
}

void Mage::performAction(Character& target) {
    if (!isAlive() || !target.isAlive()) {
        return;
    }

    if (m_currentMana >= m_manaCost) {
        m_currentMana -= m_manaCost;
        target.takeDamage(m_spellDamage);
    } else {
        target.takeDamage(m_fallbackDamage);
    }
}

} // namespace TurnBasedGame
