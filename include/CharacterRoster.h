/**
 * @file CharacterRoster.h
 * @module Roster
 * @brief Manages lifecycle of Character instances and persistence integration.
 * @features Load/Save characters, Add/Remove/Update/Search operations, validation.
 * @input File lines or Character creation parameters.
 * @output Collection of characters accessible by ID or name.
 */

#ifndef TURN_BASED_ADVENTURE_GAME_CHARACTER_ROSTER_H
#define TURN_BASED_ADVENTURE_GAME_CHARACTER_ROSTER_H

#include "Character.h"

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace TurnBasedGame {

class CharacterRoster {
public:
    static constexpr std::size_t MAX_CHARACTERS = 100U;

    CharacterRoster() = default;
    ~CharacterRoster() = default;

    
    CharacterRoster(const CharacterRoster&) = delete;
    CharacterRoster& operator=(const CharacterRoster&) = delete;

    CharacterRoster(CharacterRoster&&) noexcept = default;
    CharacterRoster& operator=(CharacterRoster&&) noexcept = default;

    bool addCharacter(std::unique_ptr<Character> character);

    bool addWarrior(int id,
                    const std::string& name,
                    int maxHp,
                    int attackPower);

    bool addMage(int id,
                 const std::string& name,
                 int maxHp,
                 int maxMana,
                 int spellDamage,
                 int manaCost,
                 int fallbackDamage);

    bool updateWarrior(int id,
                       const std::string& name,
                       int maxHp,
                       int attackPower);

    bool updateMage(int id,
                    const std::string& name,
                    int maxHp,
                    int maxMana,
                    int spellDamage,
                    int manaCost,
                    int fallbackDamage);

    bool removeById(int id);

    Character* findById(int id);
    const Character* findById(int id) const;

    std::vector<const Character*> findByNameSubstring(
        const std::string& keyword) const;

    std::vector<const Character*> getAllCharacters() const;

    std::size_t size() const noexcept;
    bool empty() const noexcept;
    bool full() const noexcept;

private:
    static bool isBlank(const std::string& text);
    static std::string toLower(const std::string& text);
    static bool isValidCommonData(int id,
                                  const std::string& name,
                                  int maxHp);

    std::vector<std::unique_ptr<Character>> m_characters;
};

} // namespace TurnBasedGame

#endif // TURN_BASED_ADVENTURE_GAME_CHARACTER_ROSTER_H
