/**
 * @file Healer.cpp
 * @module Domain Model
 * @brief Implementation of the Healer character.
 */

#include "Healer.h"

#include <algorithm>
#include <memory>

namespace TurnBasedGame {

Healer::Healer(int id, const std::string& name, int maxHp, int maxMana,
               int healAmount, int manaCost, int fallbackDamage) :
            Character(id, name, maxHp, "HEALER"),
            m_maxMana(maxMana),
            m_currentMana(maxMana),
            m_healAmount(healAmount),
            m_manaCost(manaCost),
            m_fallbackDamage(fallbackDamage) {
}

std::unique_ptr<Character> Healer::clone() const {
    return std::unique_ptr<Character>(new Healer(*this));
}

int Healer::getMaxMana() const {
    return m_maxMana;
}

int Healer::getCurrentMana() const {
    return m_currentMana;
}

int Healer::getHealAmount() const {
    return m_healAmount;
}

int Healer::getManaCost() const {
    return m_manaCost;
}

int Healer::getFallbackDamage() const {
    return m_fallbackDamage;
}

bool Healer::setMaxMana(int maxMana) {
    if (maxMana <= 0) {
        return false;
    }
    m_maxMana = maxMana;
    m_currentMana = std::min(m_currentMana, m_maxMana);
    return true;
}

bool Healer::setCurrentMana(int currentMana) {
    if (currentMana < 0) {
        return false;
    }
    m_currentMana = std::min(currentMana, m_maxMana);
    return true;
}

bool Healer::setHealAmount(int healAmount) {
    if (healAmount <= 0 || healAmount > getMaxHp()) {
        return false;
    }
    m_healAmount = healAmount;
    return true;
}

bool Healer::setManaCost(int manaCost) {
    if (manaCost <= 0) {
        return false;
    }
    m_manaCost = manaCost;
    return true;
}

bool Healer::setFallbackDamage(int fallbackDamage) {
    if (fallbackDamage <= 0) {
        return false;
    }
    m_fallbackDamage = fallbackDamage;
    return true;
}

void Healer::resetMana() {
    m_currentMana = m_maxMana;
}

void Healer::resetBattleState() {
    resetToFullHealth();
    resetMana();
}

void Healer::performAction(Character& target) {
    if (!isAlive() || !target.isAlive()) {
        return;
    }

    if (m_currentMana >= m_manaCost) {
        m_currentMana -= m_manaCost;
        target.heal(m_healAmount);
    } else {
        target.takeDamage(m_fallbackDamage);
        m_currentMana = std::min(m_maxMana, m_currentMana + (m_fallbackDamage / 2));
    }
}

} // namespace TurnBasedGame
