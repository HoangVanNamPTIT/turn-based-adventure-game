/**
 * @file Archer.h
 * @module Domain Model
 * @brief Archer character - ranged physical attacker with critical damage on every 3rd turn.
 * @features Normal attackPower on turns 1 and 2, criticalDamage (attackPower * 2) on every 3rd turn.
 * @input target Character reference.
 * @output modifies target HP via takeDamage.
 */

#ifndef TURN_BASED_ADVENTURE_GAME_ARCHER_H
#define TURN_BASED_ADVENTURE_GAME_ARCHER_H

#include <string>
#include "Character.h"

namespace TurnBasedGame {

class Archer : public Character {
public:
    Archer(int id, const std::string& name, int maxHp, int attackPower);
    ~Archer() override = default;

    std::unique_ptr<Character> clone() const override;
    void performAction(Character& target) override;
    void resetBattleState() override;

    int getAttackPower() const;
    bool setAttackPower(int attackPower);

    int getTurnCount() const;
    bool isNextAttackCritical() const;

private:
    int m_attackPower;
    int m_turnCount{0};
};

} // namespace TurnBasedGame

#endif // TURN_BASED_ADVENTURE_GAME_ARCHER_H
