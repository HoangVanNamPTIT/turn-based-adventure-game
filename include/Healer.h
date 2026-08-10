/**
 * @file Healer.h
 * @module Domain Model
 * @brief Healer character - support healer with mana, healing spell, and fallback physical attack.
 * @features performAction restores HP to an ally if mana >= manaCost, or deals fallbackDamage and recovers mana if out of mana.
 * @input target Character reference and internal mana state.
 * @output modifies target HP and own mana state.
 */

#ifndef TURN_BASED_ADVENTURE_GAME_HEALER_H
#define TURN_BASED_ADVENTURE_GAME_HEALER_H

#include <string>
#include "Character.h"

namespace TurnBasedGame {

class Healer : public Character {
public:
    Healer(int id, const std::string& name, int maxHp, int maxMana,
           int healAmount, int manaCost, int fallbackDamage);
    ~Healer() override = default;

    std::unique_ptr<Character> clone() const override;
    void performAction(Character& target) override;

    int getMaxMana() const;
    int getCurrentMana() const;
    int getHealAmount() const;
    int getManaCost() const;
    int getFallbackDamage() const;

    bool setMaxMana(int maxMana);
    bool setCurrentMana(int currentMana);
    bool setHealAmount(int healAmount);
    bool setManaCost(int manaCost);
    bool setFallbackDamage(int fallbackDamage);
    void resetMana();
    void resetBattleState() override;

private:
    int m_maxMana;
    int m_currentMana;
    int m_healAmount;
    int m_manaCost;
    int m_fallbackDamage;
};

} // namespace TurnBasedGame

#endif // TURN_BASED_ADVENTURE_GAME_HEALER_H
