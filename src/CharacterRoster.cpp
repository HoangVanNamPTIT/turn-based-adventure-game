/**
 * @file CharacterRoster.cpp
 * @module Roster
 * @brief Skeleton implementation for CharacterRoster. Business logic is intentionally omitted.
 */

#include "Archer.h"
#include "CharacterRoster.h"
#include "Healer.h"
#include "Mage.h"
#include "Warrior.h"

#include <algorithm>
#include <cctype>
#include <utility>

namespace TurnBasedGame {


bool CharacterRoster::isBlank(const std::string& text) {
    return std::all_of(
        text.begin(),
        text.end(),
        [](char character) {
            return std::isspace(
                       static_cast<unsigned char>(character)) != 0;
        });
}


std::string CharacterRoster::toLower(const std::string& text) {
    std::string result = text;

    std::transform(
        result.begin(),
        result.end(),
        result.begin(),
        [](char character) {
            return static_cast<char>(
                std::tolower(static_cast<unsigned char>(character)));
        });

    return result;
}


bool CharacterRoster::isValidCommonData(
    int id,
    const std::string& name,
    int maxHp) {
    if (id <= 0 || isBlank(name) || maxHp <= 0) {
        return false;
    }
    if (name.find('|') != std::string::npos || name.find(',') != std::string::npos) {
        return false;
    }
    return true;
}


std::size_t CharacterRoster::size() const noexcept {
    return m_characters.size();
}


bool CharacterRoster::empty() const noexcept {
    return m_characters.empty();
}


bool CharacterRoster::full() const noexcept {
    return m_characters.size() >= MAX_CHARACTERS;
}


Character* CharacterRoster::findById(int id) {
    if (id <= 0) {
        return nullptr;
    }

    const auto iterator = std::find_if(
        m_characters.begin(),
        m_characters.end(),
        [id](const std::unique_ptr<Character>& character) {
            return character != nullptr
                && character->getId() == id;
        });

    if (iterator == m_characters.end()) {
        return nullptr;
    }

    return iterator->get();
}


const Character* CharacterRoster::findById(int id) const {
    if (id <= 0) {
        return nullptr;
    }

    const auto iterator = std::find_if(
        m_characters.cbegin(),
        m_characters.cend(),
        [id](const std::unique_ptr<Character>& character) {
            return character != nullptr
                && character->getId() == id;
        });

    if (iterator == m_characters.cend()) {
        return nullptr;
    }

    return iterator->get();
}


bool CharacterRoster::addCharacter(
    std::unique_ptr<Character> character) {
    if (character == nullptr || full()) {
        return false;
    }

    if (!isValidCommonData(
            character->getId(),
            character->getName(),
            character->getMaxHp())) {
        return false;
    }

    if (const Warrior* w = dynamic_cast<const Warrior*>(character.get())) {
        if (w->getAttackPower() <= 0) {
            return false;
        }
    } else if (const Archer* a = dynamic_cast<const Archer*>(character.get())) {
        if (a->getAttackPower() <= 0) {
            return false;
        }
    } else if (const Mage* m = dynamic_cast<const Mage*>(character.get())) {
        if (m->getMaxMana() <= 0 || m->getSpellDamage() <= 0 || m->getManaCost() <= 0 || m->getFallbackDamage() <= 0) {
            return false;
        }
    } else if (const Healer* h = dynamic_cast<const Healer*>(character.get())) {
        if (h->getMaxMana() <= 0 || h->getHealAmount() <= 0 || h->getHealAmount() > h->getMaxHp() || h->getManaCost() <= 0 || h->getFallbackDamage() <= 0) {
            return false;
        }
    }

    if (findById(character->getId()) != nullptr) {
        return false;
    }

    m_characters.push_back(std::move(character));
    return true;
}


bool CharacterRoster::addWarrior(
    int id,
    const std::string& name,
    int maxHp,
    int attackPower) {
    if (!isValidCommonData(id, name, maxHp)
        || attackPower <= 0) {
        return false;
    }

    std::unique_ptr<Character> warrior(
        new Warrior(id, name, maxHp, attackPower));

    return addCharacter(std::move(warrior));
}


bool CharacterRoster::addMage(
    int id,
    const std::string& name,
    int maxHp,
    int maxMana,
    int spellDamage,
    int manaCost,
    int fallbackDamage) {
    if (!isValidCommonData(id, name, maxHp)
        || maxMana <= 0
        || spellDamage <= 0
        || manaCost <= 0
        || fallbackDamage <= 0) {
        return false;
    }

    std::unique_ptr<Character> mage(
        new Mage(
            id,
            name,
            maxHp,
            maxMana,
            spellDamage,
            manaCost,
            fallbackDamage));

    return addCharacter(std::move(mage));
}


bool CharacterRoster::addHealer(
    int id,
    const std::string& name,
    int maxHp,
    int maxMana,
    int healAmount,
    int manaCost,
    int fallbackDamage) {
    if (!isValidCommonData(id, name, maxHp)
        || maxMana <= 0
        || healAmount <= 0
        || healAmount > maxHp
        || manaCost <= 0
        || fallbackDamage <= 0) {
        return false;
    }

    std::unique_ptr<Character> healer(
        new Healer(
            id,
            name,
            maxHp,
            maxMana,
            healAmount,
            manaCost,
            fallbackDamage));

    return addCharacter(std::move(healer));
}


bool CharacterRoster::addArcher(
    int id,
    const std::string& name,
    int maxHp,
    int attackPower) {
    if (!isValidCommonData(id, name, maxHp)
        || attackPower <= 0) {
        return false;
    }

    std::unique_ptr<Character> archer(
        new Archer(id, name, maxHp, attackPower));

    return addCharacter(std::move(archer));
}


bool CharacterRoster::updateWarrior(
    int id,
    const std::string& name,
    int maxHp,
    int attackPower) {
    if (!isValidCommonData(id, name, maxHp)
        || attackPower <= 0) {
        return false;
    }

    Character* character = findById(id);
    Warrior* warrior = dynamic_cast<Warrior*>(character);

    if (warrior == nullptr) {
        return false;
    }

    warrior->setName(name);
    warrior->setMaxHp(maxHp);
    warrior->setAttackPower(attackPower);

    return true;
}


bool CharacterRoster::updateMage(
    int id,
    const std::string& name,
    int maxHp,
    int maxMana,
    int spellDamage,
    int manaCost,
    int fallbackDamage) {
    if (!isValidCommonData(id, name, maxHp)
        || maxMana <= 0
        || spellDamage <= 0
        || manaCost <= 0
        || fallbackDamage <= 0) {
        return false;
    }

    Character* character = findById(id);
    Mage* mage = dynamic_cast<Mage*>(character);

    if (mage == nullptr) {
        return false;
    }

    mage->setName(name);
    mage->setMaxHp(maxHp);
    mage->setMaxMana(maxMana);
    mage->setSpellDamage(spellDamage);
    mage->setManaCost(manaCost);
    mage->setFallbackDamage(fallbackDamage);

    return true;
}


bool CharacterRoster::updateHealer(
    int id,
    const std::string& name,
    int maxHp,
    int maxMana,
    int healAmount,
    int manaCost,
    int fallbackDamage) {
    if (!isValidCommonData(id, name, maxHp)
        || maxMana <= 0
        || healAmount <= 0
        || healAmount > maxHp
        || manaCost <= 0
        || fallbackDamage <= 0) {
        return false;
    }

    Character* character = findById(id);
    Healer* healer = dynamic_cast<Healer*>(character);

    if (healer == nullptr) {
        return false;
    }

    healer->setName(name);
    healer->setMaxHp(maxHp);
    healer->setMaxMana(maxMana);
    healer->setHealAmount(healAmount);
    healer->setManaCost(manaCost);
    healer->setFallbackDamage(fallbackDamage);

    return true;
}


bool CharacterRoster::updateArcher(
    int id,
    const std::string& name,
    int maxHp,
    int attackPower) {
    if (!isValidCommonData(id, name, maxHp)
        || attackPower <= 0) {
        return false;
    }

    Character* character = findById(id);
    Archer* archer = dynamic_cast<Archer*>(character);

    if (archer == nullptr) {
        return false;
    }

    archer->setName(name);
    archer->setMaxHp(maxHp);
    archer->setAttackPower(attackPower);

    return true;
}


bool CharacterRoster::removeById(int id) {
    if (id <= 0) {
        return false;
    }

    const auto iterator = std::find_if(
        m_characters.begin(),
        m_characters.end(),
        [id](const std::unique_ptr<Character>& character) {
            return character != nullptr
                && character->getId() == id;
        });

    if (iterator == m_characters.end()) {
        return false;
    }

    m_characters.erase(iterator);
    return true;
}


std::vector<const Character*>
CharacterRoster::findByNameSubstring(
    const std::string& keyword) const {
    std::vector<const Character*> results;

    if (isBlank(keyword)) {
        return results;
    }

    const std::string lowerKeyword = toLower(keyword);

    for (const auto& character : m_characters) {
        if (character == nullptr) {
            continue;
        }

        const std::string lowerName =
            toLower(character->getName());

        if (lowerName.find(lowerKeyword) != std::string::npos) {
            results.push_back(character.get());
        }
    }

    return results;
}


std::vector<const Character*>
CharacterRoster::getAllCharacters() const {
    std::vector<const Character*> results;
    results.reserve(m_characters.size());

    for (const auto& character : m_characters) {
        if (character != nullptr) {
            results.push_back(character.get());
        }
    }

    return results;
}
} // namespace TurnBasedGame
