/**
 * @file Warrior.h
 * @module Domain Model
 * @brief Warrior character - melee attacker with attackPower attribute.
 * @features performAction implementation that deals attackPower damage.
 * @input target Character reference.
 * @output modifies target HP via TakeDamage equivalent.
 */

#ifndef TURN_BASED_ADVENTURE_GAME_WARRIOR_H
#define TURN_BASED_ADVENTURE_GAME_WARRIOR_H

#include <string>
#include "Character.h"

namespace TurnBasedGame {

class Warrior : public Character {
public:
    Warrior(int id, const std::string& name, int maxHp, int attackPower);
    ~Warrior() override = default;

    void performAction(Character& target) override;

    int getAttackPower() const;
    bool setAttackPower(int attackPower);

private:
    int m_attackPower;
};

} // namespace TurnBasedGame

#endif // TURN_BASED_ADVENTURE_GAME_WARRIOR_H
