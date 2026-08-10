/**
 * @file TestCharacterRoster.cpp
 * @brief Unit tests for CharacterRoster class.
 */

#include "CharacterRoster.h"
#include "Warrior.h"
#include "Mage.h"
#include "Healer.h"

#include <iostream>
#include <cassert>
#include <stdexcept>
#include <memory>
#include <vector>
#include <string>

using namespace TurnBasedGame;

static int g_testsPassed = 0;
static int g_testsFailed = 0;

#define RUN_TEST(testFunc) \
    do { \
        std::cout << "[ RUN      ] " << #testFunc << " ... "; \
        try { \
            testFunc(); \
            std::cout << "PASSED" << std::endl; \
            g_testsPassed++; \
        } catch (const std::exception& ex) { \
            std::cout << "FAILED (" << ex.what() << ")" << std::endl; \
            g_testsFailed++; \
        } catch (...) { \
            std::cout << "FAILED (Unknown exception)" << std::endl; \
            g_testsFailed++; \
        } \
    } while (0)

#define ASSERT_TRUE(cond) \
    if (!(cond)) { \
        throw std::runtime_error("Assertion failed: " #cond); \
    }

#define ASSERT_FALSE(cond) \
    if (cond) { \
        throw std::runtime_error("Assertion failed (expected false): " #cond); \
    }

#define ASSERT_EQ(val1, val2) \
    if ((val1) != (val2)) { \
        throw std::runtime_error("Assertion failed: " #val1 " == " #val2); \
    }

// ---------------------------------------------------------------------------
// 1. Character Roster Addition and Query Tests
// ---------------------------------------------------------------------------

void testRoster_AddWarriorAndMage() {
    CharacterRoster roster;
    ASSERT_TRUE(roster.empty());
    ASSERT_EQ(roster.size(), static_cast<size_t>(0));

    // Add Warrior valid
    ASSERT_TRUE(roster.addWarrior(101, "Ares", 100, 30));
    ASSERT_EQ(roster.size(), static_cast<size_t>(1));
    ASSERT_FALSE(roster.empty());

    // Add Mage valid
    ASSERT_TRUE(roster.addMage(102, "Luna", 80, 50, 40, 10, 15));
    ASSERT_EQ(roster.size(), static_cast<size_t>(2));

    // Find by ID
    Character* c1 = roster.findById(101);
    ASSERT_TRUE(c1 != nullptr);
    ASSERT_EQ(c1->getName(), "Ares");

    const CharacterRoster& constRoster = roster;
    const Character* c2 = constRoster.findById(102);
    ASSERT_TRUE(c2 != nullptr);
    ASSERT_EQ(c2->getName(), "Luna");

    // Non-existent or invalid ID
    ASSERT_TRUE(roster.findById(999) == nullptr);
    ASSERT_TRUE(roster.findById(0) == nullptr);
    ASSERT_TRUE(roster.findById(-10) == nullptr);
}

void testRoster_AddAndUpdateHealer() {
    CharacterRoster roster;

    // Add Healer valid (healAmount 25 <= maxHp 100)
    ASSERT_TRUE(roster.addHealer(103, "Mercy", 100, 40, 25, 15, 10));
    ASSERT_EQ(roster.size(), static_cast<size_t>(1));

    const Character* healerChar = roster.findById(103);
    ASSERT_TRUE(healerChar != nullptr);
    ASSERT_EQ(healerChar->getName(), "Mercy");
    ASSERT_EQ(healerChar->getType(), "HEALER");

    // Add Healer with healAmount > maxHp rejected!
    ASSERT_FALSE(roster.addHealer(104, "OverHealer", 50, 40, 100, 15, 10));

    // Update Healer success
    ASSERT_TRUE(roster.updateHealer(103, "Mercy Archhealer", 120, 50, 35, 18, 12));
    const Character* updated = roster.findById(103);
    ASSERT_TRUE(updated != nullptr);
    ASSERT_EQ(updated->getName(), "Mercy Archhealer");
    ASSERT_EQ(updated->getMaxHp(), 120);

    // Update Healer with healAmount > maxHp rejected
    ASSERT_FALSE(roster.updateHealer(103, "Mercy Bad", 120, 50, 200, 18, 12));
    ASSERT_EQ(roster.findById(103)->getName(), "Mercy Archhealer");
}

void testRoster_AddDuplicatesAndInvalid() {
    CharacterRoster roster;
    ASSERT_TRUE(roster.addWarrior(101, "Ares", 100, 30));

    // Duplicate ID rejected
    ASSERT_FALSE(roster.addWarrior(101, "Ares Duplicate", 100, 30));
    ASSERT_FALSE(roster.addMage(101, "Mage Duplicate", 80, 50, 40, 10, 15));
    ASSERT_EQ(roster.size(), static_cast<size_t>(1));

    // Invalid parameters rejected
    ASSERT_FALSE(roster.addWarrior(0, "Invalid ID", 100, 30));
    ASSERT_FALSE(roster.addWarrior(102, "", 100, 30)); // empty name
    ASSERT_FALSE(roster.addWarrior(103, "   ", 100, 30)); // blank name
    ASSERT_FALSE(roster.addWarrior(104, "Invalid HP", 0, 30));
    ASSERT_FALSE(roster.addWarrior(105, "Invalid ATK", 100, 0));
    ASSERT_FALSE(roster.addMage(106, "Invalid Mana", 80, 0, 40, 10, 15));

    // FR-01.3B: Name containing delimiter characters rejected
    ASSERT_FALSE(roster.addWarrior(110, "Name|Pipe", 100, 30));
    ASSERT_FALSE(roster.addWarrior(111, "Name,Comma", 100, 30));
    ASSERT_FALSE(roster.addMage(112, "Mage|Pipe", 80, 50, 40, 10, 15));
    ASSERT_FALSE(roster.addMage(113, "Mage,Comma", 80, 50, 40, 10, 15));

    ASSERT_EQ(roster.size(), static_cast<size_t>(1));
}

// ---------------------------------------------------------------------------
// 2. Character Roster Update and Delete Tests
// ---------------------------------------------------------------------------

void testRoster_UpdateWarriorAndMage() {
    CharacterRoster roster;
    roster.addWarrior(101, "Ares", 100, 30);
    roster.addMage(102, "Luna", 80, 50, 40, 10, 15);

    // Update Warrior success
    ASSERT_TRUE(roster.updateWarrior(101, "Ares Supreme", 120, 35));
    Character* warrior = roster.findById(101);
    ASSERT_EQ(warrior->getName(), "Ares Supreme");
    ASSERT_EQ(warrior->getMaxHp(), 120);

    // Update Mage success
    ASSERT_TRUE(roster.updateMage(102, "Luna Archmage", 90, 60, 50, 12, 18));
    Character* mage = roster.findById(102);
    ASSERT_EQ(mage->getName(), "Luna Archmage");
    ASSERT_EQ(mage->getMaxHp(), 90);

    // Updating Warrior with Mage ID (type mismatch) returns false
    ASSERT_FALSE(roster.updateWarrior(102, "Fake Warrior", 100, 30));

    // Updating non-existent ID returns false
    ASSERT_FALSE(roster.updateWarrior(999, "Unknown", 100, 30));

    // FR-01.3B: Update with delimiter name rejected
    ASSERT_FALSE(roster.updateWarrior(101, "Ares|Bad", 100, 30));
    ASSERT_FALSE(roster.updateWarrior(101, "Ares,Bad", 100, 30));
    ASSERT_FALSE(roster.updateMage(102, "Luna|Bad", 90, 60, 50, 12, 18));
    ASSERT_FALSE(roster.updateMage(102, "Luna,Bad", 90, 60, 50, 12, 18));
    // Verify names unchanged after failed update
    ASSERT_EQ(roster.findById(101)->getName(), "Ares Supreme");
    ASSERT_EQ(roster.findById(102)->getName(), "Luna Archmage");
}

void testRoster_RemoveById() {
    CharacterRoster roster;
    roster.addWarrior(101, "Ares", 100, 30);
    roster.addMage(102, "Luna", 80, 50, 40, 10, 15);

    ASSERT_TRUE(roster.removeById(101));
    ASSERT_EQ(roster.size(), static_cast<size_t>(1));
    ASSERT_TRUE(roster.findById(101) == nullptr);

    // Remove non-existent ID
    ASSERT_FALSE(roster.removeById(101));
    ASSERT_FALSE(roster.removeById(999));
    ASSERT_FALSE(roster.removeById(0));
}

// ---------------------------------------------------------------------------
// 3. Search and Retrieval Tests
// ---------------------------------------------------------------------------

void testRoster_FindByNameSubstring() {
    CharacterRoster roster;
    roster.addWarrior(101, "Shadow Knight", 100, 30);
    roster.addWarrior(102, "Shadow Assassin", 90, 35);
    roster.addMage(103, "Light Priest", 70, 60, 30, 10, 10);

    // Substring search case-insensitive
    auto matches = roster.findByNameSubstring("shadow");
    ASSERT_EQ(matches.size(), static_cast<size_t>(2));

    auto matchPriest = roster.findByNameSubstring("PRIEST");
    ASSERT_EQ(matchPriest.size(), static_cast<size_t>(1));
    ASSERT_EQ(matchPriest[0]->getName(), "Light Priest");

    // Empty/blank search returns empty list
    auto emptySearch = roster.findByNameSubstring("   ");
    ASSERT_TRUE(emptySearch.empty());

    // No matches
    auto noMatches = roster.findByNameSubstring("Dragon");
    ASSERT_TRUE(noMatches.empty());
}

void testRoster_GetAllCharactersAndCapacity() {
    CharacterRoster roster;
    roster.addWarrior(101, "Ares", 100, 30);
    roster.addMage(102, "Luna", 80, 50, 40, 10, 15);

    auto all = roster.getAllCharacters();
    ASSERT_EQ(all.size(), static_cast<size_t>(2));

    // Full capacity test check
    ASSERT_FALSE(roster.full());
}

int main() {
    std::cout << "==================================================" << std::endl;
    std::cout << "     Character Roster Unit Test Runner            " << std::endl;
    std::cout << "==================================================" << std::endl;

    RUN_TEST(testRoster_AddWarriorAndMage);
    RUN_TEST(testRoster_AddAndUpdateHealer);
    RUN_TEST(testRoster_AddDuplicatesAndInvalid);
    RUN_TEST(testRoster_UpdateWarriorAndMage);
    RUN_TEST(testRoster_RemoveById);
    RUN_TEST(testRoster_FindByNameSubstring);
    RUN_TEST(testRoster_GetAllCharactersAndCapacity);

    std::cout << "==================================================" << std::endl;
    std::cout << " Summary: " << g_testsPassed << " passed, " << g_testsFailed << " failed." << std::endl;
    std::cout << "==================================================" << std::endl;

    return g_testsFailed == 0 ? 0 : 1;
}
