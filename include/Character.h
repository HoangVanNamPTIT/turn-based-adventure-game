/**
 * @file Character.h
 * @module Domain Model
 * @brief Abstract base class representing a game character.
 * @features Encapsulates identity, HP, and provides polymorphic actions.
 * @input Instantiation parameters for derived types.
 * @output Virtual action execution and status inspection methods.
 */

#ifndef TURN_BASED_ADVENTURE_GAME_CHARACTER_H
#define TURN_BASED_ADVENTURE_GAME_CHARACTER_H

#include <string>

namespace TurnBasedGame {

class Character {
public:
    Character(int id, const std::string& name, int maxHp, const std::string& type);
    virtual ~Character() = default;

    virtual void performAction(Character& target) = 0;

    int getId() const;
    const std::string& getName() const;
    int getMaxHp() const;
    int getCurrentHp() const;
    const std::string& getType() const;
    bool isAlive() const;

    bool setName(const std::string& name);
    bool setMaxHp(int maxHp);
    void setCurrentHp(int currentHp);
    bool takeDamage(int amount);
    bool heal(int amount);
    void resetToFullHealth();
    virtual void resetBattleState();

private:
    int m_id;
    std::string m_name;
    int m_maxHp;
    int m_currentHp;
    std::string m_type;
};

} // namespace TurnBasedGame

#endif // TURN_BASED_ADVENTURE_GAME_CHARACTER_H
