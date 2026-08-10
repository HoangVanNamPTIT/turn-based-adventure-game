/**
 * @file RunAllTests.cpp
 * @brief Master Unit Test Runner for Turn-Based Adventure Game system.
 */

#include "BattleEngine.h"
#include "BattleState.h"
#include "Character.h"
#include "CharacterRoster.h"
#include "DataFileManager.h"
#include "GameApp.h"
#include "Mage.h"
#include "Menu.h"
#include "Team.h"
#include "TeamManager.h"
#include "Utils.h"
#include "Warrior.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>
#include <stdexcept>
#include <memory>
#include <vector>
#include <string>
#include <cstdio>

using namespace TurnBasedGame;

static int g_testsPassed = 0;
static int g_testsFailed = 0;

struct TestResultInfo {
    std::string moduleName;
    std::string testCaseId;
    std::string testDescription;
    bool passed;
    std::string errorMsg;
};

static std::vector<TestResultInfo> g_testResults;

#define RUN_SYSTEM_TEST(module, tcId, desc, testFunc) \
    do { \
        std::cout << "  [" << tcId << "] " << desc << " ... "; \
        bool success = true; \
        std::string errStr = ""; \
        try { \
            testFunc(); \
            std::cout << "\033[32mPASSED\033[0m" << std::endl; \
            g_testsPassed++; \
        } catch (const std::exception& ex) { \
            std::cout << "\033[31mFAILED (" << ex.what() << ")\033[0m" << std::endl; \
            g_testsFailed++; \
            success = false; \
            errStr = ex.what(); \
        } catch (...) { \
            std::cout << "\033[31mFAILED (Unknown exception)\033[0m" << std::endl; \
            g_testsFailed++; \
            success = false; \
            errStr = "Unknown exception"; \
        } \
        g_testResults.push_back({module, tcId, desc, success, errStr}); \
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

// ===========================================================================
// 1. Utils Module Tests
// ===========================================================================

void testTrim_BasicSpaces() {
    ASSERT_EQ(Utils::Utils::trim("  hello  "), "hello");
    ASSERT_EQ(Utils::Utils::trim("\tworld\n\r"), "world");
    ASSERT_EQ(Utils::Utils::trim("   multiple   words   "), "multiple   words");
}

void testTrim_EdgeCases() {
    ASSERT_EQ(Utils::Utils::trim(""), "");
    ASSERT_EQ(Utils::Utils::trim("   "), "");
}

void testSplit_Basic() {
    auto tokens = Utils::Utils::split("WARRIOR|101|Ares|100|30", '|');
    ASSERT_EQ(tokens.size(), static_cast<size_t>(5));
    ASSERT_EQ(tokens[0], "WARRIOR");
}

void testSplit_WithWhitespace() {
    auto tokens = Utils::Utils::split("  MAGE  | 102 |  Luna  ", '|');
    ASSERT_EQ(tokens.size(), static_cast<size_t>(3));
    ASSERT_EQ(tokens[0], "MAGE");
}

void testSplit_EmptyAndTrailing() {
    auto tokens = Utils::Utils::split("a,b,c,", ',');
    ASSERT_EQ(tokens.size(), static_cast<size_t>(4));
    ASSERT_EQ(tokens[3], "");
}

void testIsInteger_Valid() {
    ASSERT_TRUE(Utils::Utils::isInteger("123"));
    ASSERT_TRUE(Utils::Utils::isInteger("-456"));
    ASSERT_TRUE(Utils::Utils::isInteger("+789"));
}

void testIsInteger_Invalid() {
    ASSERT_FALSE(Utils::Utils::isInteger(""));
    ASSERT_FALSE(Utils::Utils::isInteger("abc"));
    ASSERT_FALSE(Utils::Utils::isInteger("12.34"));
}

void testParseInt_Valid() {
    ASSERT_EQ(Utils::Utils::parseInt("123"), 123);
    ASSERT_EQ(Utils::Utils::parseInt("-456"), -456);
}

void testParseInt_InvalidFallback() {
    ASSERT_EQ(Utils::Utils::parseInt("abc", -1), -1);
    ASSERT_EQ(Utils::Utils::parseInt("", 42), 42);
}

// ===========================================================================
// 2. Character Domain Model Tests
// ===========================================================================

void testCharacter_ConstructorAndGetters() {
    Warrior warrior(1, "Arthur", 100, 25);
    ASSERT_EQ(warrior.getId(), 1);
    ASSERT_EQ(warrior.getName(), "Arthur");
    ASSERT_EQ(warrior.getMaxHp(), 100);
    ASSERT_TRUE(warrior.isAlive());

    // OOP-TC-01: Setters với tên whitespace/delimiter, maxHp<=0, damage<=0 -> State cũ giữ nguyên!
    ASSERT_FALSE(warrior.setName("   "));
    ASSERT_FALSE(warrior.setName("Arthur|King"));
    ASSERT_FALSE(warrior.setName("Arthur,King"));
    ASSERT_EQ(warrior.getName(), "Arthur"); // State cũ không đổi!

    ASSERT_FALSE(warrior.setMaxHp(0));
    ASSERT_FALSE(warrior.setMaxHp(-10));
    ASSERT_EQ(warrior.getMaxHp(), 100); // State cũ không đổi!

    ASSERT_FALSE(warrior.setAttackPower(0));
    ASSERT_FALSE(warrior.setAttackPower(-5));
    ASSERT_EQ(warrior.getAttackPower(), 25); // State cũ không đổi!
}

void testCharacter_SettersAndValidation() {
    Warrior warrior(1, "Arthur", 100, 25);
    ASSERT_TRUE(warrior.setName("King Arthur"));
    ASSERT_FALSE(warrior.setName(""));
    ASSERT_TRUE(warrior.setMaxHp(150));
    ASSERT_FALSE(warrior.setMaxHp(0));
    warrior.setCurrentHp(150);
    warrior.setMaxHp(80);
    ASSERT_EQ(warrior.getCurrentHp(), 80);
}

void testCharacter_TakeDamageAndHeal() {
    Warrior warrior(1, "Arthur", 100, 25);
    ASSERT_TRUE(warrior.takeDamage(30));
    ASSERT_EQ(warrior.getCurrentHp(), 70);
    ASSERT_TRUE(warrior.takeDamage(100));
    ASSERT_EQ(warrior.getCurrentHp(), 0);
    ASSERT_FALSE(warrior.isAlive());
    ASSERT_TRUE(warrior.heal(50));
    ASSERT_EQ(warrior.getCurrentHp(), 50);
}

void testCharacter_ResetHealth() {
    Warrior warrior(1, "Arthur", 100, 25);
    warrior.takeDamage(80);
    warrior.resetToFullHealth();
    ASSERT_EQ(warrior.getCurrentHp(), 100);
}

void testWarrior_AttackPowerSetters() {
    Warrior warrior(1, "Thor", 120, 35);
    ASSERT_TRUE(warrior.setAttackPower(45));
    ASSERT_FALSE(warrior.setAttackPower(0));
}

void testWarrior_PerformAction() {
    Warrior warrior(1, "Thor", 100, 30);
    Warrior dummyTarget(2, "Goblin", 50, 10);
    warrior.performAction(dummyTarget);
    ASSERT_EQ(dummyTarget.getCurrentHp(), 20);
}

void testMage_ManaAndSpellSetters() {
    Mage mage(1, "Merlin", 80, 50, 40, 15, 10);
    ASSERT_EQ(mage.getMaxMana(), 50);
    ASSERT_TRUE(mage.setMaxMana(60));
    ASSERT_TRUE(mage.setCurrentMana(30));
    ASSERT_FALSE(mage.setMaxMana(0));
}

void testMage_PerformAction_SpellVsFallback() {
    Mage mage(1, "Gandalf", 80, 30, 40, 20, 10);
    Warrior target(2, "Orc", 100, 15);
    mage.performAction(target);
    ASSERT_EQ(mage.getCurrentMana(), 10);
    ASSERT_EQ(target.getCurrentHp(), 60);

    mage.performAction(target);
    ASSERT_EQ(mage.getCurrentMana(), 10);
    ASSERT_EQ(target.getCurrentHp(), 50);

    mage.resetBattleState();
    ASSERT_EQ(mage.getCurrentMana(), 30);
}

// ===========================================================================
// 3. Team Entity Tests
// ===========================================================================

void testTeam_ConstructorsAndSetters() {
    Team team(10, "Alpha Squad", {101, 102});
    ASSERT_EQ(team.getId(), 10);
    ASSERT_EQ(team.getName(), "Alpha Squad");

    // OOP-TC-02: Team luôn giữ ID dương, tên hợp lệ, member dương/unique/không quá 5.
    // Public setter từ chối ID âm/0 (không tạo ID=0)
    ASSERT_FALSE(team.setId(-5));
    ASSERT_FALSE(team.setId(0));
    ASSERT_EQ(team.getId(), 10); // ID cũ không đổi (không thành 0)!

    ASSERT_FALSE(team.setName("   "));
    ASSERT_FALSE(team.setName("Alpha|Squad"));
    ASSERT_FALSE(team.setName("Alpha,Squad"));
    ASSERT_EQ(team.getName(), "Alpha Squad"); // Tên cũ không đổi!

    // Invariant member list: từ chối duplicate, âm, >5
    ASSERT_FALSE(team.setCharacterIds({101, 101})); // duplicate
    ASSERT_FALSE(team.setCharacterIds({-1})); // âm
    ASSERT_FALSE(team.setCharacterIds({1, 2, 3, 4, 5, 6})); // > 5
    ASSERT_EQ(team.getCharacterIds().size(), static_cast<size_t>(2));
}

void testTeam_CharacterIdOperations() {
    Team team(1, "Red Team");
    team.addCharacterId(101);
    team.addCharacterId(102);
    ASSERT_EQ(team.getCharacterIds().size(), static_cast<size_t>(2));
    team.addCharacterId(101); // Duplicate ignored
    ASSERT_EQ(team.getCharacterIds().size(), static_cast<size_t>(2));
    ASSERT_TRUE(team.removeCharacterId(101));
    ASSERT_FALSE(team.hasCharacterId(101));
}

// ===========================================================================
// 4. CharacterRoster Tests
// ===========================================================================

void testRoster_AddWarriorAndMage() {
    CharacterRoster roster;
    ASSERT_TRUE(roster.addWarrior(101, "Ares", 100, 30));
    ASSERT_TRUE(roster.addMage(102, "Luna", 80, 50, 40, 10, 15));
    ASSERT_EQ(roster.size(), static_cast<size_t>(2));
    ASSERT_TRUE(roster.findById(101) != nullptr);
}

void testRoster_AddDuplicatesAndInvalid() {
    CharacterRoster roster;
    roster.addWarrior(101, "Ares", 100, 30);
    ASSERT_FALSE(roster.addWarrior(101, "Duplicate Ares", 100, 30));
    ASSERT_FALSE(roster.addWarrior(0, "Invalid ID", 100, 30));

    // OOP-TC-03: Generic insertion addCharacter(unique_ptr<Character>) từ chối subtype có dữ liệu gốc không hợp lệ (attackPower = 0)
    std::unique_ptr<Character> invalidWarrior(new Warrior(200, "Arthur", 100, 0));
    ASSERT_FALSE(roster.addCharacter(std::move(invalidWarrior)));
}

void testRoster_UpdateWarriorAndMage() {
    CharacterRoster roster;
    roster.addWarrior(101, "Ares", 100, 30);
    roster.addMage(102, "Luna", 80, 50, 40, 10, 15);
    ASSERT_TRUE(roster.updateWarrior(101, "Ares Supreme", 120, 35));
    ASSERT_FALSE(roster.updateWarrior(102, "Fake Warrior", 100, 30));
}

void testRoster_RemoveById() {
    CharacterRoster roster;
    roster.addWarrior(101, "Ares", 100, 30);
    ASSERT_TRUE(roster.removeById(101));
    ASSERT_TRUE(roster.findById(101) == nullptr);
}

void testRoster_FindByNameSubstring() {
    CharacterRoster roster;
    roster.addWarrior(101, "Shadow Knight", 100, 30);
    roster.addMage(102, "Light Priest", 70, 60, 30, 10, 10);
    auto results = roster.findByNameSubstring("shadow");
    ASSERT_EQ(results.size(), static_cast<size_t>(1));
}

void testRoster_GetAllCharactersAndCapacity() {
    CharacterRoster roster;
    roster.addWarrior(101, "Ares", 100, 30);
    ASSERT_EQ(roster.getAllCharacters().size(), static_cast<size_t>(1));
    ASSERT_FALSE(roster.full());
}

// ===========================================================================
// 5. TeamManager Tests
// ===========================================================================

void testTeamManager_CreateAndDelete() {
    TeamManager tm;
    ASSERT_TRUE(tm.createTeam(201, "Red Team"));
    ASSERT_FALSE(tm.createTeam(201, "Green Team"));
    ASSERT_FALSE(tm.createTeam(202, "Red Team"));
    ASSERT_TRUE(tm.deleteTeam(201));
    ASSERT_FALSE(tm.teamExists(201));
}

void testTeamManager_RenameTeam() {
    TeamManager tm;
    tm.createTeam(201, "Red Team");
    tm.createTeam(202, "Blue Team");
    ASSERT_TRUE(tm.renameTeam(201, "Crimson Team"));
    ASSERT_FALSE(tm.renameTeam(201, "Blue Team"));
}

void testTeamManager_AddAndRemoveCharacter() {
    CharacterRoster roster;
    roster.addWarrior(101, "Ares", 100, 30);
    TeamManager tm;
    tm.createTeam(201, "Red Team");
    ASSERT_TRUE(tm.addCharacterToTeam(201, 101, roster));
    ASSERT_FALSE(tm.addCharacterToTeam(201, 101, roster));
    ASSERT_TRUE(tm.removeCharacterFromTeam(201, 101));
}

void testTeamManager_RemoveCharacterFromAllTeams() {
    CharacterRoster roster;
    roster.addWarrior(101, "Ares", 100, 30);
    TeamManager tm;
    tm.createTeam(201, "Team A");
    tm.createTeam(202, "Team B");
    tm.addCharacterToTeam(201, 101, roster);
    tm.addCharacterToTeam(202, 101, roster);
    ASSERT_TRUE(tm.removeCharacterFromAllTeams(101));
    ASSERT_FALSE(tm.getTeamById(201)->hasCharacterId(101));
}

void testTeamManager_LoadAndSave() {
    std::string tempFile = "temp_all_teams.txt";
    CharacterRoster roster;
    roster.addWarrior(101, "Ares", 100, 30);
    TeamManager tmSave;
    tmSave.createTeam(201, "Red Team");
    tmSave.addCharacterToTeam(201, 101, roster);
    ASSERT_TRUE(tmSave.save(tempFile));

    TeamManager tmLoad;
    ASSERT_TRUE(tmLoad.load(tempFile, roster));
    ASSERT_TRUE(tmLoad.teamExists(201));
    removeTempFile(tempFile);
}

// ===========================================================================
// 6. DataFileManager Persistence Tests
// ===========================================================================

void testParseCharacterLine_WarriorSuccess() {
    auto charPtr = DataFileManager::parseCharacterLine("WARRIOR|101|Ares|100|30");
    ASSERT_TRUE(charPtr != nullptr);
    ASSERT_EQ(charPtr->getName(), "Ares");
}

void testParseCharacterLine_MageSuccess() {
    auto charPtr = DataFileManager::parseCharacterLine("MAGE|102|Luna|80|50|40|10|15");
    ASSERT_TRUE(charPtr != nullptr);
    ASSERT_EQ(charPtr->getName(), "Luna");
}

void testParseCharacterLine_CommentsAndEmpty() {
    ASSERT_TRUE(DataFileManager::parseCharacterLine("") == nullptr);
    ASSERT_TRUE(DataFileManager::parseCharacterLine("# Comment") == nullptr);
}

void testParseCharacterLine_InvalidType() {
    ASSERT_TRUE(DataFileManager::parseCharacterLine("ARCHER|103|Robin|90|25") == nullptr);
}

void testParseCharacterLine_MissingOrExtraTokens() {
    ASSERT_TRUE(DataFileManager::parseCharacterLine("WARRIOR|101|Ares|100") == nullptr);
    ASSERT_TRUE(DataFileManager::parseCharacterLine("WARRIOR|101|Ares|100|30|EXTRA") == nullptr);
}

void testParseCharacterLine_InvalidValues() {
    ASSERT_TRUE(DataFileManager::parseCharacterLine("WARRIOR|0|Ares|100|30") == nullptr);
    ASSERT_TRUE(DataFileManager::parseCharacterLine("WARRIOR|101||100|30") == nullptr);
}

void testSerializeCharacter_Success() {
    Warrior warrior(101, "Ares", 100, 30);
    ASSERT_EQ(DataFileManager::serializeCharacter(warrior), "WARRIOR|101|Ares|100|30");
}

void testSerializeCharacter_InvalidCharacter_ReturnsEmpty() {
    Warrior invalidWarrior(0, "Invalid", 100, 30);
    ASSERT_EQ(DataFileManager::serializeCharacter(invalidWarrior), "");
}

void testLoadAndSaveCharacters_RoundTrip() {
    std::string tempFile = "temp_all_chars.txt";
    std::vector<std::shared_ptr<Character>> original;
    original.push_back(std::make_shared<Warrior>(101, "Ares", 100, 30));
    ASSERT_TRUE(DataFileManager::saveCharacters(tempFile, original));
    std::vector<std::shared_ptr<Character>> loaded;
    ASSERT_TRUE(DataFileManager::loadCharacters(tempFile, loaded));
    ASSERT_EQ(loaded.size(), static_cast<size_t>(1));
    removeTempFile(tempFile);
}

void testLoadCharacters_NonExistentFile() {
    std::vector<std::shared_ptr<Character>> loaded;
    ASSERT_FALSE(DataFileManager::loadCharacters("non_existent_xyz.txt", loaded));
}

void testParseTeamLine_Success() {
    Team team;
    ASSERT_TRUE(DataFileManager::parseTeamLine("201|Red Team|101,102", team));
    ASSERT_EQ(team.getId(), 201);
}

void testParseTeamLine_CommentsAndEmpty() {
    Team team;
    ASSERT_FALSE(DataFileManager::parseTeamLine("", team));
    ASSERT_FALSE(DataFileManager::parseTeamLine("# Comment", team));
}

void testParseTeamLine_MissingOrExtraTokens() {
    Team team;
    ASSERT_FALSE(DataFileManager::parseTeamLine("201", team));
    ASSERT_FALSE(DataFileManager::parseTeamLine("201|Red Team", team));
    ASSERT_FALSE(DataFileManager::parseTeamLine("201|Red Team|101|EXTRA", team));
}

void testParseTeamLine_InvalidValues() {
    Team team;
    ASSERT_FALSE(DataFileManager::parseTeamLine("0|Red Team|101", team));
    ASSERT_FALSE(DataFileManager::parseTeamLine("201||101", team));
}

void testSerializeTeam_Success() {
    Team team(201, "Red Team", {101, 102});
    ASSERT_EQ(DataFileManager::serializeTeam(team), "201|Red Team|101,102");
}

void testSerializeTeam_InvalidTeam_ReturnsEmpty() {
    Team invalidTeam(0, "Invalid");
    ASSERT_EQ(DataFileManager::serializeTeam(invalidTeam), "");
}

void testLoadAndSaveTeams_RoundTrip() {
    std::string tempFile = "temp_all_teams_rt.txt";
    std::vector<Team> original = {Team(201, "Red Team", {101, 102})};
    ASSERT_TRUE(DataFileManager::saveTeams(tempFile, original));
    std::vector<Team> loaded;
    ASSERT_TRUE(DataFileManager::loadTeams(tempFile, loaded));
    ASSERT_EQ(loaded.size(), static_cast<size_t>(1));
    removeTempFile(tempFile);
}

void testLoadTeams_NonExistentFile() {
    std::vector<Team> loaded;
    ASSERT_FALSE(DataFileManager::loadTeams("non_existent_teams_xyz.txt", loaded));
}

// ===========================================================================
// 7. BattleEngine Tests
// ===========================================================================

void testBattleState_ToString() {
    ASSERT_EQ(battleStateToString(BattleState::READY), "READY");
    ASSERT_EQ(battleStateToString(BattleState::IN_PROGRESS), "IN_PROGRESS");
    ASSERT_EQ(battleStateToString(BattleState::FINISHED), "FINISHED");
}

void testBattleEngine_SetupValidation() {
    CharacterRoster roster;
    roster.addWarrior(101, "Ares", 100, 30);
    roster.addMage(102, "Luna", 80, 50, 40, 10, 15);
    Team teamA(201, "Red Team", {101});
    Team teamB(202, "Blue Team", {102});
    BattleEngine engine;
    ASSERT_TRUE(engine.startBattle(teamA, teamB, roster));
    ASSERT_EQ(engine.getState(), BattleState::IN_PROGRESS);
}

void testBattleEngine_FullCombatFlow() {
    CharacterRoster roster;
    roster.addWarrior(101, "Ares", 100, 60);
    roster.addMage(102, "Luna", 80, 50, 50, 10, 15);
    Team teamA(201, "Red Team", {101});
    Team teamB(202, "Blue Team", {102});
    BattleEngine engine;
    engine.startBattle(teamA, teamB, roster);

    ASSERT_TRUE(engine.performAction(101, 102));
    ASSERT_TRUE(engine.performAction(102, 101));
    ASSERT_TRUE(engine.performAction(101, 102)); // Luna dies
    ASSERT_EQ(engine.getState(), BattleState::FINISHED);
    ASSERT_EQ(engine.getWinnerTeam()->getId(), 201);
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

void testBattleEngine_TeamReallocationSafety() {
    CharacterRoster roster;
    roster.addWarrior(101, "Ares", 100, 30);
    roster.addMage(102, "Luna", 80, 50, 40, 10, 15);

    TeamManager teamManager;
    teamManager.createTeam(1, "Team One");
    teamManager.createTeam(2, "Team Two");
    teamManager.addCharacterToTeam(1, 101, roster);
    teamManager.addCharacterToTeam(2, 102, roster);

    const Team* teamA = teamManager.getTeamById(1);
    const Team* teamB = teamManager.getTeamById(2);
    ASSERT_TRUE(teamA != nullptr);
    ASSERT_TRUE(teamB != nullptr);

    BattleEngine engine;
    ASSERT_TRUE(engine.startBattle(*teamA, *teamB, roster));
    ASSERT_EQ(engine.getTeamA()->getId(), 1);
    ASSERT_EQ(engine.getTeamB()->getId(), 2);

    // Thêm hàng loạt team mới vào TeamManager để kích hoạt reallocation std::vector<Team>
    for (int i = 10; i < 50; ++i) {
        teamManager.createTeam(i, "Extra Team " + std::to_string(i));
    }

    // Đọc battle.getTeamA() và battle.getTeamB() để xác nhận KHÔNG BỊ Use-After-Free
    ASSERT_TRUE(engine.getTeamA() != nullptr);
    ASSERT_EQ(engine.getTeamA()->getId(), 1);
    ASSERT_EQ(engine.getTeamA()->getName(), "Team One");

    ASSERT_TRUE(engine.getTeamB() != nullptr);
    ASSERT_EQ(engine.getTeamB()->getId(), 2);
    ASSERT_EQ(engine.getTeamB()->getName(), "Team Two");

    ASSERT_TRUE(engine.performAction(101, 102));
    ASSERT_EQ(engine.getRoundsPlayed(), static_cast<size_t>(1));
}

// ===========================================================================
// 8. Menu & GameApp Component Tests
// ===========================================================================

void testMenu_ReadHelpersAndDisplay() {
    std::stringstream input("2\n101\nHero\ny\n1\n");
    std::stringstream output;
    Menu menu(input, output);

    int val = 0;
    ASSERT_TRUE(menu.readInt("Prompt: ", 1, 5, val));
    ASSERT_EQ(val, 2);
    ASSERT_TRUE(menu.readPositiveInt("Prompt: ", val));
    ASSERT_EQ(val, 101);
    ASSERT_EQ(menu.showAddCharacterMenu(), 1);
}

void testGameApp_ExecutionFlow() {
    std::stringstream input("4\n");
    std::stringstream output;
    GameApp app(input, output);
    ASSERT_EQ(app.run(), 0);
}

// ===========================================================================
// Master Suite Execution
// ===========================================================================

int main() {
    std::cout << "======================================================================" << std::endl;
    std::cout << "   🚀 MASTER SYSTEM UNIT TEST RUNNER - TURN-BASED ADVENTURE GAME      " << std::endl;
    std::cout << "======================================================================" << std::endl;

    std::cout << "\n--- [MODULE 1: Utils] ---" << std::endl;
    RUN_SYSTEM_TEST("Utils", "TC_UTIL_01", "Trim basic spaces and whitespace", testTrim_BasicSpaces);
    RUN_SYSTEM_TEST("Utils", "TC_UTIL_02", "Trim edge cases empty string", testTrim_EdgeCases);
    RUN_SYSTEM_TEST("Utils", "TC_UTIL_03", "Split basic pipe separated string", testSplit_Basic);
    RUN_SYSTEM_TEST("Utils", "TC_UTIL_04", "Split with surrounding whitespace", testSplit_WithWhitespace);
    RUN_SYSTEM_TEST("Utils", "TC_UTIL_05", "Split empty and trailing delimiters", testSplit_EmptyAndTrailing);
    RUN_SYSTEM_TEST("Utils", "TC_UTIL_06", "isInteger valid positive and negative strings", testIsInteger_Valid);
    RUN_SYSTEM_TEST("Utils", "TC_UTIL_07", "isInteger invalid alphabetic and float strings", testIsInteger_Invalid);
    RUN_SYSTEM_TEST("Utils", "TC_UTIL_08", "parseInt valid string conversion", testParseInt_Valid);
    RUN_SYSTEM_TEST("Utils", "TC_UTIL_09", "parseInt invalid fallback value handling", testParseInt_InvalidFallback);

    std::cout << "\n--- [MODULE 2: Domain Model (Character, Warrior, Mage)] ---" << std::endl;
    RUN_SYSTEM_TEST("Character", "TC_CHAR_MOD_01", "Constructor initial values and parameter clamping", testCharacter_ConstructorAndGetters);
    RUN_SYSTEM_TEST("Character", "TC_CHAR_MOD_02", "Setters validation and max HP clamping", testCharacter_SettersAndValidation);
    RUN_SYSTEM_TEST("Character", "TC_CHAR_MOD_03", "Take damage clamping and heal calculation", testCharacter_TakeDamageAndHeal);
    RUN_SYSTEM_TEST("Character", "TC_CHAR_MOD_04", "Reset health to full HP", testCharacter_ResetHealth);
    RUN_SYSTEM_TEST("Warrior", "TC_WARRIOR_01", "Attack power getters and setters validation", testWarrior_AttackPowerSetters);
    RUN_SYSTEM_TEST("Warrior", "TC_WARRIOR_02", "Perform action dealing physical damage", testWarrior_PerformAction);
    RUN_SYSTEM_TEST("Mage", "TC_MAGE_01", "Mana and spell stat getters and setters", testMage_ManaAndSpellSetters);
    RUN_SYSTEM_TEST("Mage", "TC_MAGE_02", "Spell damage vs fallback damage based on mana", testMage_PerformAction_SpellVsFallback);

    std::cout << "\n--- [MODULE 3: Team Entity] ---" << std::endl;
    RUN_SYSTEM_TEST("Team", "TC_TEAM_MOD_01", "Team constructors, ID clamping and name setters", testTeam_ConstructorsAndSetters);
    RUN_SYSTEM_TEST("Team", "TC_TEAM_MOD_02", "Add, remove and check character IDs in team", testTeam_CharacterIdOperations);

    std::cout << "\n--- [MODULE 4: CharacterRoster] ---" << std::endl;
    RUN_SYSTEM_TEST("CharacterRoster", "TC_ROSTER_01", "Add valid Warrior and Mage characters", testRoster_AddWarriorAndMage);
    RUN_SYSTEM_TEST("CharacterRoster", "TC_ROSTER_02", "Reject duplicate IDs and invalid attributes", testRoster_AddDuplicatesAndInvalid);
    RUN_SYSTEM_TEST("CharacterRoster", "TC_ROSTER_03", "Update existing character details and type checks", testRoster_UpdateWarriorAndMage);
    RUN_SYSTEM_TEST("CharacterRoster", "TC_ROSTER_04", "Remove character by ID", testRoster_RemoveById);
    RUN_SYSTEM_TEST("CharacterRoster", "TC_ROSTER_05", "Search characters by case-insensitive name substring", testRoster_FindByNameSubstring);
    RUN_SYSTEM_TEST("CharacterRoster", "TC_ROSTER_06", "Retrieve all characters and check capacity limits", testRoster_GetAllCharactersAndCapacity);

    std::cout << "\n--- [MODULE 5: TeamManager] ---" << std::endl;
    RUN_SYSTEM_TEST("TeamManager", "TC_TM_01", "Create and delete teams with unique constraints", testTeamManager_CreateAndDelete);
    RUN_SYSTEM_TEST("TeamManager", "TC_TM_02", "Rename team and prevent duplicate names", testTeamManager_RenameTeam);
    RUN_SYSTEM_TEST("TeamManager", "TC_TM_03", "Add/remove character to team with size 5 limit", testTeamManager_AddAndRemoveCharacter);
    RUN_SYSTEM_TEST("TeamManager", "TC_TM_04", "Cascading character removal from all teams", testTeamManager_RemoveCharacterFromAllTeams);
    RUN_SYSTEM_TEST("TeamManager", "TC_TM_05", "Load and save teams to file", testTeamManager_LoadAndSave);

    std::cout << "\n--- [MODULE 6: DataFileManager (Persistence)] ---" << std::endl;
    RUN_SYSTEM_TEST("DataFileManager", "TC_DFM_01", "Parse Warrior line", testParseCharacterLine_WarriorSuccess);
    RUN_SYSTEM_TEST("DataFileManager", "TC_DFM_02", "Parse Mage line", testParseCharacterLine_MageSuccess);
    RUN_SYSTEM_TEST("DataFileManager", "TC_DFM_03", "Parse comment or empty line", testParseCharacterLine_CommentsAndEmpty);
    RUN_SYSTEM_TEST("DataFileManager", "TC_DFM_04", "Parse unknown character type", testParseCharacterLine_InvalidType);
    RUN_SYSTEM_TEST("DataFileManager", "TC_DFM_05", "Parse line with missing or extra tokens", testParseCharacterLine_MissingOrExtraTokens);
    RUN_SYSTEM_TEST("DataFileManager", "TC_DFM_06", "Parse line with invalid values", testParseCharacterLine_InvalidValues);
    RUN_SYSTEM_TEST("DataFileManager", "TC_DFM_07", "Serialize character to pipe string", testSerializeCharacter_Success);
    RUN_SYSTEM_TEST("DataFileManager", "TC_DFM_08", "Serialize invalid character returns empty string", testSerializeCharacter_InvalidCharacter_ReturnsEmpty);
    RUN_SYSTEM_TEST("DataFileManager", "TC_DFM_09", "Load and Save characters round-trip IO", testLoadAndSaveCharacters_RoundTrip);
    RUN_SYSTEM_TEST("DataFileManager", "TC_DFM_10", "Load characters from non-existent file", testLoadCharacters_NonExistentFile);
    RUN_SYSTEM_TEST("DataFileManager", "TC_DFM_11", "Parse team line", testParseTeamLine_Success);
    RUN_SYSTEM_TEST("DataFileManager", "TC_DFM_12", "Parse team comment or empty line", testParseTeamLine_CommentsAndEmpty);
    RUN_SYSTEM_TEST("DataFileManager", "TC_DFM_13", "Parse team line missing or extra tokens", testParseTeamLine_MissingOrExtraTokens);
    RUN_SYSTEM_TEST("DataFileManager", "TC_DFM_14", "Parse team line invalid values", testParseTeamLine_InvalidValues);
    RUN_SYSTEM_TEST("DataFileManager", "TC_DFM_15", "Serialize team to pipe string", testSerializeTeam_Success);
    RUN_SYSTEM_TEST("DataFileManager", "TC_DFM_16", "Serialize invalid team returns empty string", testSerializeTeam_InvalidTeam_ReturnsEmpty);
    RUN_SYSTEM_TEST("DataFileManager", "TC_DFM_17", "Load and Save teams round-trip IO", testLoadAndSaveTeams_RoundTrip);
    RUN_SYSTEM_TEST("DataFileManager", "TC_DFM_18", "Load teams from non-existent file", testLoadTeams_NonExistentFile);

    std::cout << "\n--- [MODULE 7: BattleEngine & BattleState] ---" << std::endl;
    RUN_SYSTEM_TEST("BattleEngine", "TC_BAT_01", "Convert BattleState enum to string", testBattleState_ToString);
    RUN_SYSTEM_TEST("BattleEngine", "TC_BAT_02", "Validate battle setup rules and team status", testBattleEngine_SetupValidation);
    RUN_SYSTEM_TEST("BattleEngine", "TC_BAT_03", "Full combat round flow, turn transitions and victory", testBattleEngine_FullCombatFlow);
    RUN_SYSTEM_TEST("BattleEngine", "TC_BAT_04", "Reset battle state back to READY", testBattleEngine_ResetBattle);
    RUN_SYSTEM_TEST("BattleEngine", "TC_BAT_05", "Team vector reallocation safety without use-after-free (OOP-TC-08)", testBattleEngine_TeamReallocationSafety);

    std::cout << "\n--- [MODULE 8: Menu & Application Orchestrator] ---" << std::endl;
    RUN_SYSTEM_TEST("Menu", "TC_APP_01", "Menu input parsing helpers and screen displays", testMenu_ReadHelpersAndDisplay);
    RUN_SYSTEM_TEST("GameApp", "TC_APP_02", "GameApp orchestrator startup and clean exit flow", testGameApp_ExecutionFlow);

    std::cout << "\n======================================================================" << std::endl;
    std::cout << "                      FULL SYSTEM TEST SUMMARY                        " << std::endl;
    std::cout << "======================================================================" << std::endl;
    std::cout << " Total Test Cases  : " << g_testResults.size() << std::endl;
    std::cout << " Passed Test Cases : " << g_testsPassed << " (" << std::fixed << std::setprecision(1) << (g_testResults.empty() ? 0.0 : (double)g_testsPassed / g_testResults.size() * 100.0) << "%)" << std::endl;
    std::cout << " Failed Test Cases : " << g_testsFailed << std::endl;
    std::cout << " Overall Status    : " << (g_testsFailed == 0 ? "PASSED (100%)" : "FAILED") << std::endl;
    std::cout << "======================================================================" << std::endl;

    return g_testsFailed == 0 ? 0 : 1;
}
