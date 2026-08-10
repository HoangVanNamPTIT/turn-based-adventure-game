/**
 * @file TestCharacterAndSubclasses.cpp
 * @brief Unit tests for Character, Warrior, and Mage classes.
 */

#include "Character.h"
#include "Warrior.h"
#include "Mage.h"
#include "Healer.h"

#include <iostream>
#include <cassert>
#include <stdexcept>
#include <memory>
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
// 1. Character Base Class Tests
// ---------------------------------------------------------------------------

void testCharacter_ConstructorAndGetters() {
    Warrior warrior(1, "Arthur", 100, 25);
    ASSERT_EQ(warrior.getId(), 1);
    ASSERT_EQ(warrior.getName(), "Arthur");
    ASSERT_EQ(warrior.getMaxHp(), 100);
    ASSERT_EQ(warrior.getCurrentHp(), 100);
    ASSERT_EQ(warrior.getType(), "WARRIOR");
    ASSERT_TRUE(warrior.isAlive());

    // Invalid parameters clamping test
    Warrior invalidChar(-5, "", -10, -5);
    ASSERT_EQ(invalidChar.getId(), 0);
    ASSERT_EQ(invalidChar.getName(), "");
    ASSERT_EQ(invalidChar.getMaxHp(), 1);
    ASSERT_EQ(invalidChar.getCurrentHp(), 1);
}

void testCharacter_SettersAndValidation() {
    Warrior warrior(1, "Arthur", 100, 25);

    // setName
    ASSERT_TRUE(warrior.setName("King Arthur"));
    ASSERT_EQ(warrior.getName(), "King Arthur");
    ASSERT_FALSE(warrior.setName("")); // empty name rejected
    ASSERT_EQ(warrior.getName(), "King Arthur");

    // setMaxHp & Clamping
    ASSERT_TRUE(warrior.setMaxHp(150));
    ASSERT_EQ(warrior.getMaxHp(), 150);
    ASSERT_FALSE(warrior.setMaxHp(0)); // maxHp <= 0 rejected
    ASSERT_FALSE(warrior.setMaxHp(-20));

    // Clamp currentHp when maxHp is decreased below currentHp
    warrior.setCurrentHp(150);
    ASSERT_EQ(warrior.getCurrentHp(), 150);
    warrior.setMaxHp(80);
    ASSERT_EQ(warrior.getCurrentHp(), 80);
}

void testCharacter_TakeDamageAndHeal() {
    Warrior warrior(1, "Arthur", 100, 25);

    // Take damage
    ASSERT_TRUE(warrior.takeDamage(30));
    ASSERT_EQ(warrior.getCurrentHp(), 70);
    ASSERT_TRUE(warrior.isAlive());

    // Overkill damage clamps to 0
    ASSERT_TRUE(warrior.takeDamage(100));
    ASSERT_EQ(warrior.getCurrentHp(), 0);
    ASSERT_FALSE(warrior.isAlive());

    // Invalid negative damage
    ASSERT_FALSE(warrior.takeDamage(-10));
    ASSERT_FALSE(warrior.takeDamage(0));

    // Heal dead character
    ASSERT_TRUE(warrior.heal(50));
    ASSERT_EQ(warrior.getCurrentHp(), 50);
    ASSERT_TRUE(warrior.isAlive());

    // Overheal clamps to maxHp
    ASSERT_TRUE(warrior.heal(200));
    ASSERT_EQ(warrior.getCurrentHp(), 100);

    // Invalid negative heal
    ASSERT_FALSE(warrior.heal(-5));
    ASSERT_FALSE(warrior.heal(0));
}

void testCharacter_ResetHealth() {
    Warrior warrior(1, "Arthur", 100, 25);
    warrior.takeDamage(80);
    ASSERT_EQ(warrior.getCurrentHp(), 20);

    warrior.resetToFullHealth();
    ASSERT_EQ(warrior.getCurrentHp(), 100);
}

// ---------------------------------------------------------------------------
// 2. Warrior Subclass Tests
// ---------------------------------------------------------------------------

void testWarrior_AttackPowerSetters() {
    Warrior warrior(1, "Thor", 120, 35);
    ASSERT_EQ(warrior.getAttackPower(), 35);

    ASSERT_TRUE(warrior.setAttackPower(45));
    ASSERT_EQ(warrior.getAttackPower(), 45);

    ASSERT_FALSE(warrior.setAttackPower(0));
    ASSERT_FALSE(warrior.setAttackPower(-10));
    ASSERT_EQ(warrior.getAttackPower(), 45);
}

void testWarrior_PerformAction() {
    Warrior warrior(1, "Thor", 100, 30);
    Warrior dummyTarget(2, "Goblin", 50, 10);

    warrior.performAction(dummyTarget);
    ASSERT_EQ(dummyTarget.getCurrentHp(), 20);

    // Perform action again kills dummyTarget
    warrior.performAction(dummyTarget);
    ASSERT_EQ(dummyTarget.getCurrentHp(), 0);
    ASSERT_FALSE(dummyTarget.isAlive());

    // Attacking a dead target should do nothing
    warrior.performAction(dummyTarget);
    ASSERT_EQ(dummyTarget.getCurrentHp(), 0);

    // Dead warrior performing action does nothing
    Warrior deadWarrior(3, "Ghost", 100, 50);
    deadWarrior.takeDamage(100);
    Warrior newTarget(4, "Target", 100, 10);

    deadWarrior.performAction(newTarget);
    ASSERT_EQ(newTarget.getCurrentHp(), 100);
}

// ---------------------------------------------------------------------------
// 3. Mage Subclass Tests
// ---------------------------------------------------------------------------

void testMage_ManaAndSpellSetters() {
    Mage mage(1, "Merlin", 80, 50, 40, 15, 10);
    ASSERT_EQ(mage.getMaxMana(), 50);
    ASSERT_EQ(mage.getCurrentMana(), 50);
    ASSERT_EQ(mage.getSpellDamage(), 40);
    ASSERT_EQ(mage.getManaCost(), 15);
    ASSERT_EQ(mage.getFallbackDamage(), 10);

    // Setters valid
    ASSERT_TRUE(mage.setMaxMana(60));
    ASSERT_EQ(mage.getMaxMana(), 60);
    ASSERT_TRUE(mage.setCurrentMana(30));
    ASSERT_EQ(mage.getCurrentMana(), 30);

    ASSERT_TRUE(mage.setSpellDamage(50));
    ASSERT_EQ(mage.getSpellDamage(), 50);
    ASSERT_TRUE(mage.setManaCost(20));
    ASSERT_EQ(mage.getManaCost(), 20);
    ASSERT_TRUE(mage.setFallbackDamage(15));
    ASSERT_EQ(mage.getFallbackDamage(), 15);

    // Invalid setters
    ASSERT_FALSE(mage.setMaxMana(0));
    ASSERT_FALSE(mage.setCurrentMana(-5));
    ASSERT_FALSE(mage.setSpellDamage(0));
    ASSERT_FALSE(mage.setManaCost(0));
    ASSERT_FALSE(mage.setFallbackDamage(0));
}

void testMage_PerformAction_SpellVsFallback() {
    Mage mage(1, "Gandalf", 80, 30, 40, 20, 10);
    Warrior target(2, "Orc", 100, 15);

    // Round 1: Has 30 mana >= 20 cost. Uses spell, spends 20 mana. Target takes 40 damage.
    mage.performAction(target);
    ASSERT_EQ(mage.getCurrentMana(), 10);
    ASSERT_EQ(target.getCurrentHp(), 60);

    // Round 2: Has 10 mana < 20 cost. Uses fallback attack (10 dmg), spends 0 mana.
    mage.performAction(target);
    ASSERT_EQ(mage.getCurrentMana(), 10); // Mana unchanged
    ASSERT_EQ(target.getCurrentHp(), 50);

    // Reset battle state restores HP and Mana
    mage.takeDamage(30);
    mage.resetBattleState();
    ASSERT_EQ(mage.getCurrentHp(), 80);
    ASSERT_EQ(mage.getCurrentMana(), 30);
}

// ---------------------------------------------------------------------------
// 4. Healer Subclass Tests
// ---------------------------------------------------------------------------

void testHealer_ManaAndHealSetters() {
    Healer healer(1, "Mercy", 100, 50, 30, 20, 10);
    ASSERT_EQ(healer.getType(), "HEALER");
    ASSERT_EQ(healer.getMaxMana(), 50);
    ASSERT_EQ(healer.getCurrentMana(), 50);
    ASSERT_EQ(healer.getHealAmount(), 30);
    ASSERT_EQ(healer.getManaCost(), 20);
    ASSERT_EQ(healer.getFallbackDamage(), 10);

    // Setters valid
    ASSERT_TRUE(healer.setMaxMana(60));
    ASSERT_EQ(healer.getMaxMana(), 60);
    ASSERT_TRUE(healer.setCurrentMana(30));
    ASSERT_EQ(healer.getCurrentMana(), 30);

    ASSERT_TRUE(healer.setHealAmount(40));
    ASSERT_EQ(healer.getHealAmount(), 40);
    ASSERT_TRUE(healer.setManaCost(25));
    ASSERT_EQ(healer.getManaCost(), 25);
    ASSERT_TRUE(healer.setFallbackDamage(12));
    ASSERT_EQ(healer.getFallbackDamage(), 12);

    // Invalid setters
    ASSERT_FALSE(healer.setMaxMana(0));
    ASSERT_FALSE(healer.setCurrentMana(-5));
    ASSERT_FALSE(healer.setHealAmount(0));
    ASSERT_FALSE(healer.setHealAmount(150)); // healAmount > maxHp rejected!
    ASSERT_FALSE(healer.setManaCost(0));
    ASSERT_FALSE(healer.setFallbackDamage(0));
}

void testHealer_PerformAction_HealVsFallbackAndManaRegen() {
    Healer healer(1, "Mercy", 100, 30, 25, 20, 10);
    Warrior ally(2, "InjuredAlly", 100, 15);
    ally.takeDamage(40); // Ally HP = 60

    Warrior enemy(3, "Enemy", 100, 10);

    // Round 1: Has 30 mana >= 20 cost. Performs heal on ally. Spends 20 mana. Ally healed by 25.
    healer.performAction(ally);
    ASSERT_EQ(healer.getCurrentMana(), 10);
    ASSERT_EQ(ally.getCurrentHp(), 85);

    // Round 2: Has 10 mana < 20 cost. Performs fallback attack on enemy. Enemy takes 10 dmg. Healer regens 10/2 = 5 mana.
    healer.performAction(enemy);
    ASSERT_EQ(healer.getCurrentMana(), 15);
    ASSERT_EQ(enemy.getCurrentHp(), 90);

    // Round 3: Has 15 mana < 20 cost. Performs fallback attack again. Enemy HP = 80. Healer regens 5 mana -> 20 mana.
    healer.performAction(enemy);
    ASSERT_EQ(healer.getCurrentMana(), 20);
    ASSERT_EQ(enemy.getCurrentHp(), 80);

    // Round 4: Has 20 mana >= 20 cost. Can heal ally again! Ally HP was 85, healed by 25 -> capped at 100 maxHp.
    healer.performAction(ally);
    ASSERT_EQ(healer.getCurrentMana(), 0);
    ASSERT_EQ(ally.getCurrentHp(), 100);

    // Reset battle state restores HP and Mana
    healer.takeDamage(50);
    healer.resetBattleState();
    ASSERT_EQ(healer.getCurrentHp(), 100);
    ASSERT_EQ(healer.getCurrentMana(), 30);
}

int main() {
    std::cout << "==================================================" << std::endl;
    std::cout << "    Character & Subclasses Unit Test Runner       " << std::endl;
    std::cout << "==================================================" << std::endl;

    RUN_TEST(testCharacter_ConstructorAndGetters);
    RUN_TEST(testCharacter_SettersAndValidation);
    RUN_TEST(testCharacter_TakeDamageAndHeal);
    RUN_TEST(testCharacter_ResetHealth);
    RUN_TEST(testWarrior_AttackPowerSetters);
    RUN_TEST(testWarrior_PerformAction);
    RUN_TEST(testMage_ManaAndSpellSetters);
    RUN_TEST(testMage_PerformAction_SpellVsFallback);
    RUN_TEST(testHealer_ManaAndHealSetters);
    RUN_TEST(testHealer_PerformAction_HealVsFallbackAndManaRegen);

    std::cout << "==================================================" << std::endl;
    std::cout << " Summary: " << g_testsPassed << " passed, " << g_testsFailed << " failed." << std::endl;
    std::cout << "==================================================" << std::endl;

    return g_testsFailed == 0 ? 0 : 1;
}
