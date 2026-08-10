/**
 * @file TestTeamManager.cpp
 * @brief Unit tests for TeamManager class.
 */

#include "TeamManager.h"
#include "CharacterRoster.h"
#include "Warrior.h"
#include "Mage.h"

#include <iostream>
#include <cassert>
#include <stdexcept>
#include <vector>
#include <string>
#include <fstream>
#include <cstdio>

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

static void removeTempFile(const std::string& path) {
    std::remove(path.c_str());
}

// ---------------------------------------------------------------------------
// 1. Team CRUD Operations Tests
// ---------------------------------------------------------------------------

void testTeamManager_CreateAndDelete() {
    TeamManager tm;
    ASSERT_TRUE(tm.getAllTeams().empty());

    // Valid team creation
    ASSERT_TRUE(tm.createTeam(201, "Red Team"));
    ASSERT_TRUE(tm.createTeam(202, "Blue Team"));
    ASSERT_EQ(tm.getAllTeams().size(), static_cast<size_t>(2));

    ASSERT_TRUE(tm.teamExists(201));
    ASSERT_TRUE(tm.teamNameExists("Red Team"));

    // Duplicate ID rejected
    ASSERT_FALSE(tm.createTeam(201, "Green Team"));

    // Duplicate Name rejected (case-exact and case-insensitive)
    ASSERT_FALSE(tm.createTeam(203, "Red Team"));
    ASSERT_FALSE(tm.createTeam(203, "red team"));
    ASSERT_FALSE(tm.createTeam(203, "RED TEAM"));

    // Invalid parameters rejected
    ASSERT_FALSE(tm.createTeam(0, "Invalid ID"));
    ASSERT_FALSE(tm.createTeam(-5, "Negative ID"));
    ASSERT_FALSE(tm.createTeam(204, ""));
    ASSERT_FALSE(tm.createTeam(205, "   "));

    // Name containing delimiter characters rejected
    ASSERT_FALSE(tm.createTeam(206, "Name|WithPipe"));
    ASSERT_FALSE(tm.createTeam(207, "Name,WithComma"));
    ASSERT_FALSE(tm.createTeam(208, "Alpha|Beta,Gamma"));

    // Delete team
    ASSERT_TRUE(tm.deleteTeam(201));
    ASSERT_FALSE(tm.teamExists(201));
    ASSERT_EQ(tm.getAllTeams().size(), static_cast<size_t>(1));

    // Delete non-existent team
    ASSERT_FALSE(tm.deleteTeam(999));
}

void testTeamManager_RenameTeam() {
    TeamManager tm;
    tm.createTeam(201, "Red Team");
    tm.createTeam(202, "Blue Team");

    // Rename success
    ASSERT_TRUE(tm.renameTeam(201, "Crimson Team"));
    ASSERT_EQ(tm.getTeamById(201)->getName(), "Crimson Team");

    // Rename to same name returns true
    ASSERT_TRUE(tm.renameTeam(201, "Crimson Team"));

    // Rename to existing name of another team returns false (case-exact and case-insensitive)
    ASSERT_FALSE(tm.renameTeam(201, "Blue Team"));
    ASSERT_FALSE(tm.renameTeam(201, "blue team"));
    ASSERT_FALSE(tm.renameTeam(201, "BLUE TEAM"));

    // Rename with invalid name returns false
    ASSERT_FALSE(tm.renameTeam(201, ""));
    ASSERT_FALSE(tm.renameTeam(201, "   "));

    // Rename with delimiter characters returns false
    ASSERT_FALSE(tm.renameTeam(201, "Bad|Name"));
    ASSERT_FALSE(tm.renameTeam(201, "Bad,Name"));
    ASSERT_FALSE(tm.renameTeam(201, "Pipe|Comma,Mix"));
}

// ---------------------------------------------------------------------------
// 2. Character-to-Team Assignment Tests
// ---------------------------------------------------------------------------

void testTeamManager_AddAndRemoveCharacter() {
    CharacterRoster roster;
    roster.addWarrior(101, "Ares", 100, 30);
    roster.addWarrior(102, "Thor", 120, 35);
    roster.addMage(103, "Luna", 80, 50, 40, 10, 15);
    roster.addWarrior(104, "Odin", 150, 40);
    roster.addWarrior(105, "Loki", 90, 25);
    roster.addWarrior(106, "Freya", 110, 28); // 6th character

    TeamManager tm;
    tm.createTeam(201, "Avengers");

    // Add characters up to limit (5)
    ASSERT_TRUE(tm.addCharacterToTeam(201, 101, roster));
    ASSERT_TRUE(tm.addCharacterToTeam(201, 102, roster));
    ASSERT_TRUE(tm.addCharacterToTeam(201, 103, roster));
    ASSERT_TRUE(tm.addCharacterToTeam(201, 104, roster));
    ASSERT_TRUE(tm.addCharacterToTeam(201, 105, roster));

    // Duplicate character in team rejected
    ASSERT_FALSE(tm.addCharacterToTeam(201, 101, roster));

    // 6th character rejected (max 5 members per team)
    ASSERT_FALSE(tm.addCharacterToTeam(201, 106, roster));

    // Add character not in roster rejected
    ASSERT_FALSE(tm.addCharacterToTeam(201, 999, roster));

    // Remove character from team
    ASSERT_TRUE(tm.removeCharacterFromTeam(201, 103));
    ASSERT_FALSE(tm.getTeamById(201)->hasCharacterId(103));

    // Now 106 can be added
    ASSERT_TRUE(tm.addCharacterToTeam(201, 106, roster));
}

void testTeamManager_RemoveCharacterFromAllTeams() {
    CharacterRoster roster;
    roster.addWarrior(101, "Ares", 100, 30);
    roster.addMage(102, "Luna", 80, 50, 40, 10, 15);

    TeamManager tm;
    tm.createTeam(201, "Team A");
    tm.createTeam(202, "Team B");

    tm.addCharacterToTeam(201, 101, roster);
    tm.addCharacterToTeam(201, 102, roster);
    tm.addCharacterToTeam(202, 101, roster);

    // Cascading deletion of character 101 from all teams
    ASSERT_TRUE(tm.removeCharacterFromAllTeams(101));

    ASSERT_FALSE(tm.getTeamById(201)->hasCharacterId(101));
    ASSERT_FALSE(tm.getTeamById(202)->hasCharacterId(101));
    ASSERT_TRUE(tm.getTeamById(201)->hasCharacterId(102));
}

// ---------------------------------------------------------------------------
// 3. Team Persistence (Load/Save) Tests
// ---------------------------------------------------------------------------

void testTeamManager_LoadAndSave() {
    std::string tempTeamFile = "temp_test_teams.txt";

    CharacterRoster roster;
    roster.addWarrior(101, "Ares", 100, 30);
    roster.addMage(102, "Luna", 80, 50, 40, 10, 15);

    TeamManager tmSave;
    tmSave.createTeam(201, "Red Team");
    tmSave.addCharacterToTeam(201, 101, roster);
    tmSave.addCharacterToTeam(201, 102, roster);

    tmSave.createTeam(202, "Blue Team");

    ASSERT_TRUE(tmSave.save(tempTeamFile));

    TeamManager tmLoad;
    ASSERT_TRUE(tmLoad.load(tempTeamFile, roster));

    ASSERT_EQ(tmLoad.getAllTeams().size(), static_cast<size_t>(2));
    ASSERT_TRUE(tmLoad.teamExists(201));
    ASSERT_TRUE(tmLoad.getTeamById(201)->hasCharacterId(101));

    removeTempFile(tempTeamFile);
}

int main() {
    std::cout << "==================================================" << std::endl;
    std::cout << "        TeamManager Unit Test Runner              " << std::endl;
    std::cout << "==================================================" << std::endl;

    RUN_TEST(testTeamManager_CreateAndDelete);
    RUN_TEST(testTeamManager_RenameTeam);
    RUN_TEST(testTeamManager_AddAndRemoveCharacter);
    RUN_TEST(testTeamManager_RemoveCharacterFromAllTeams);
    RUN_TEST(testTeamManager_LoadAndSave);

    std::cout << "==================================================" << std::endl;
    std::cout << " Summary: " << g_testsPassed << " passed, " << g_testsFailed << " failed." << std::endl;
    std::cout << "==================================================" << std::endl;

    return g_testsFailed == 0 ? 0 : 1;
}
