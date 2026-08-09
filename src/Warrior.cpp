/**
 * @file Warrior.cpp
 * @module Domain Model
 * @brief Implementation of the Warrior character.
 */

#include "Warrior.h"

namespace TurnBasedGame {

Warrior::Warrior(int id, const std::string& name, int maxHp, int attackPower) : 
    Character(id, name, maxHp, "WARRIOR"), m_attackPower(attackPower) {
}

std::unique_ptr<Character> Warrior::clone() const {
    return std::unique_ptr<Character>(new Warrior(*this));
}

int Warrior::getAttackPower() const {
    return m_attackPower;
}

bool Warrior::setAttackPower(int attackPower) {
    if (attackPower <= 0) {
        return false;
    }
    m_attackPower = attackPower;
    return true;
}

void Warrior::performAction(Character& target) {
    if (!isAlive() || !target.isAlive()) {
        return;
    }

    target.takeDamage(m_attackPower);
}

} // namespace TurnBasedGame
