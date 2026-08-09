/**
 * @file TestBattleEngine.cpp
 * @brief Unit tests for BattleEngine and BattleState module.
 */

#include "BattleEngine.h"
#include "BattleState.h"
#include "CharacterRoster.h"
#include "TeamManager.h"
#include "Warrior.h"
#include "Mage.h"

#include <iostream>
#include <cassert>
#include <stdexcept>
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
// 1. BattleState Helper Tests
// ---------------------------------------------------------------------------

void testBattleState_ToString() {
    ASSERT_EQ(battleStateToString(BattleState::READY), "READY");
    ASSERT_EQ(battleStateToString(BattleState::IN_PROGRESS), "IN_PROGRESS");
    ASSERT_EQ(battleStateToString(BattleState::FINISHED), "FINISHED");
}

// ---------------------------------------------------------------------------
// 2. Battle Setup Validation Tests
// ---------------------------------------------------------------------------

void testBattleEngine_SetupValidation() {
    CharacterRoster roster;
    roster.addWarrior(101, "Ares", 100, 30);
    roster.addMage(102, "Luna", 80, 50, 40, 10, 15);

    Team teamA(201, "Red Team", {101});
    Team teamB(202, "Blue Team", {102});
    Team emptyTeam(203, "Empty Team", {});

    BattleEngine engine;
    ASSERT_EQ(engine.getState(), BattleState::READY);

    // Same team battle rejected
    ASSERT_FALSE(engine.startBattle(teamA, teamA, roster));

    // Empty team battle rejected
    ASSERT_FALSE(engine.startBattle(teamA, emptyTeam, roster));

    // Character not in roster rejected
    Team badTeam(204, "Bad Team", {999});
    ASSERT_FALSE(engine.startBattle(teamA, badTeam, roster));

    // Successful setup
    ASSERT_TRUE(engine.startBattle(teamA, teamB, roster));
    ASSERT_EQ(engine.getState(), BattleState::IN_PROGRESS);
    ASSERT_EQ(engine.getTeamA()->getId(), 201);
    ASSERT_EQ(engine.getTeamB()->getId(), 202);
    ASSERT_EQ(engine.getCurrentActorId(), 101);
    ASSERT_EQ(engine.getRoundsPlayed(), static_cast<size_t>(0));
}

// ---------------------------------------------------------------------------
// 3. Battle Execution and Turn-Based Combat Tests
// ---------------------------------------------------------------------------

void testBattleEngine_FullCombatFlow() {
    CharacterRoster roster;
    // Warrior: 100 HP, 60 ATK
    roster.addWarrior(101, "Ares", 100, 60);
    // Mage: 80 HP, 50 Mana, 50 Spell damage, 10 Mana cost, 15 Fallback
    roster.addMage(102, "Luna", 80, 50, 50, 10, 15);

    Team teamA(201, "Red Team", {101});
    Team teamB(202, "Blue Team", {102});

    BattleEngine engine;
    ASSERT_TRUE(engine.startBattle(teamA, teamB, roster));

    // Round 1: Active actor is 101 (Team A - Ares). Attacks 102 (Luna).
    // Invalid action attempts:
    // Out of turn actor (102 trying to attack 101) rejected
    ASSERT_FALSE(engine.performAction(102, 101));
    // Self attack rejected
    ASSERT_FALSE(engine.performAction(101, 101));
    // Attacking team mate (not enemy) rejected
    ASSERT_FALSE(engine.performAction(101, 999));

    // Valid attack: 101 attacks 102
    ASSERT_TRUE(engine.performAction(101, 102));
    ASSERT_EQ(engine.getRoundsPlayed(), static_cast<size_t>(1));
    // Luna takes 60 damage -> HP 20 remaining
    Character* luna = roster.findById(102);
    ASSERT_EQ(luna->getCurrentHp(), 20);

    // Turn automatically switches to Team B actor: 102 (Luna).
    ASSERT_EQ(engine.getCurrentActorId(), 102);

    // Round 2: 102 (Luna) attacks 101 (Ares) with spell (50 damage).
    ASSERT_TRUE(engine.performAction(102, 101));
    ASSERT_EQ(engine.getRoundsPlayed(), static_cast<size_t>(2));
    Character* ares = roster.findById(101);
    ASSERT_EQ(ares->getCurrentHp(), 50);

    // Turn switches back to Team A actor: 101 (Ares).
    ASSERT_EQ(engine.getCurrentActorId(), 101);

    // Round 3: 101 (Ares) attacks 102 (Luna) dealing 60 damage -> Luna dies (HP 0).
    ASSERT_TRUE(engine.performAction(101, 102));
    ASSERT_EQ(engine.getRoundsPlayed(), static_cast<size_t>(3));
    ASSERT_FALSE(luna->isAlive());

    // Team B has no living characters left! Battle finishes.
    ASSERT_EQ(engine.getState(), BattleState::FINISHED);
    ASSERT_TRUE(engine.getWinnerTeam() != nullptr);
    ASSERT_EQ(engine.getWinnerTeam()->getId(), 201); // Red Team wins!
}

void testBattleEngine_ResetBattle() {
    CharacterRoster roster;
    roster.addWarrior(101, "Ares", 100, 30);
    roster.addMage(102, "Luna", 80, 50, 40, 10, 15);

    Team teamA(201, "Red Team", {101});
    Team teamB(202, "Blue Team", {102});

    BattleEngine engine;
    engine.startBattle(teamA, teamB, roster);
    engine.performAction(101, 102);

    engine.resetBattle();
    ASSERT_EQ(engine.getState(), BattleState::READY);
    ASSERT_TRUE(engine.getTeamA() == nullptr);
    ASSERT_TRUE(engine.getTeamB() == nullptr);
    ASSERT_TRUE(engine.getCurrentActor() == nullptr);
    ASSERT_EQ(engine.getRoundsPlayed(), static_cast<size_t>(0));
}

int main() {
    std::cout << "==================================================" << std::endl;
    std::cout << "       BattleEngine Unit Test Runner              " << std::endl;
    std::cout << "==================================================" << std::endl;

    RUN_TEST(testBattleState_ToString);
    RUN_TEST(testBattleEngine_SetupValidation);
    RUN_TEST(testBattleEngine_FullCombatFlow);
    RUN_TEST(testBattleEngine_ResetBattle);

    std::cout << "==================================================" << std::endl;
    std::cout << " Summary: " << g_testsPassed << " passed, " << g_testsFailed << " failed." << std::endl;
    std::cout << "==================================================" << std::endl;

    return g_testsFailed == 0 ? 0 : 1;
}
