/**
 * @file Archer.cpp
 * @module Domain Model
 * @brief Implementation for Archer character subclass.
 */

#include "Archer.h"

namespace TurnBasedGame {

Archer::Archer(int id, const std::string& name, int maxHp, int attackPower)
    : Character(id, name, maxHp, "ARCHER"),
      m_attackPower(attackPower > 0 ? attackPower : 1),
      m_turnCount(0) {
}

std::unique_ptr<Character> Archer::clone() const {
    return std::unique_ptr<Character>(new Archer(*this));
}

void Archer::performAction(Character& target) {
    if (!isAlive() || !target.isAlive()) {
        return;
    }

    ++m_turnCount;

    if (m_turnCount % 3 == 0) {
        const int criticalDamage = m_attackPower * 2;
        target.takeDamage(criticalDamage);
    } else {
        target.takeDamage(m_attackPower);
    }
}

void Archer::resetBattleState() {
    resetToFullHealth();
    m_turnCount = 0;
}

int Archer::getAttackPower() const {
    return m_attackPower;
}

bool Archer::setAttackPower(int attackPower) {
    if (attackPower <= 0) {
        return false;
    }
    m_attackPower = attackPower;
    return true;
}

int Archer::getTurnCount() const {
    return m_turnCount;
}

bool Archer::isNextAttackCritical() const {
    return (m_turnCount + 1) % 3 == 0;
}

} // namespace TurnBasedGame
