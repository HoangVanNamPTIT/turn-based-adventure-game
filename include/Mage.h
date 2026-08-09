/**
 * @file Mage.h
 * @module Domain Model
 * @brief Mage character - ranged spellcaster with mana and spell damage.
 * @features performAction consumes mana to deal spellDamage or fallbackDamage.
 * @input target Character reference and internal mana state.
 * @output modifies target HP and own mana state.
 */

#ifndef TURN_BASED_ADVENTURE_GAME_MAGE_H
#define TURN_BASED_ADVENTURE_GAME_MAGE_H

#include <string>
#include "Character.h"

namespace TurnBasedGame {

class Mage : public Character {
public:
    Mage(int id, const std::string& name, int maxHp, int maxMana,
         int spellDamage, int manaCost, int fallbackDamage);
    ~Mage() override = default;

    std::unique_ptr<Character> clone() const override;
    void performAction(Character& target) override;

    int getMaxMana() const;
    int getCurrentMana() const;
    int getSpellDamage() const;
    int getManaCost() const;
    int getFallbackDamage() const;

    bool setMaxMana(int maxMana);
    bool setCurrentMana(int currentMana);
    bool setSpellDamage(int spellDamage);
    bool setManaCost(int manaCost);
    bool setFallbackDamage(int fallbackDamage);
    void resetMana();
    void resetBattleState() override;

private:
    int m_maxMana;
    int m_currentMana;
    int m_spellDamage;
    int m_manaCost;
    int m_fallbackDamage;
};

} // namespace TurnBasedGame

#endif // TURN_BASED_ADVENTURE_GAME_MAGE_H
