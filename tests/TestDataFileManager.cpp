/**
 * @file TestDataFileManager.cpp
 * @brief Unit tests for TurnBasedGame::DataFileManager class.
 */

#include "DataFileManager.h"
#include "Warrior.h"
#include "Mage.h"
#include "Healer.h"
#include "Team.h"

#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <fstream>
#include <cstdio>
#include <memory>

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

// Helper to remove temporary files
static void removeTempFile(const std::string& path) {
    std::remove(path.c_str());
}

// ---------------------------------------------------------------------------
// 1. Tests for parseCharacterLine
// ---------------------------------------------------------------------------

void testParseCharacterLine_WarriorSuccess() {
    std::string line = "WARRIOR|101|Ares|100|30";
    auto charPtr = DataFileManager::parseCharacterLine(line);
    ASSERT_TRUE(charPtr != nullptr);
    ASSERT_EQ(charPtr->getType(), "WARRIOR");
    ASSERT_EQ(charPtr->getId(), 101);
    ASSERT_EQ(charPtr->getName(), "Ares");
    ASSERT_EQ(charPtr->getMaxHp(), 100);
    
    auto warrior = std::dynamic_pointer_cast<Warrior>(charPtr);
    ASSERT_TRUE(warrior != nullptr);
    ASSERT_EQ(warrior->getAttackPower(), 30);
}

void testParseCharacterLine_MageSuccess() {
    std::string line = "MAGE|102|Luna|80|50|40|10|15";
    auto charPtr = DataFileManager::parseCharacterLine(line);
    ASSERT_TRUE(charPtr != nullptr);
    ASSERT_EQ(charPtr->getType(), "MAGE");
    ASSERT_EQ(charPtr->getId(), 102);
    ASSERT_EQ(charPtr->getName(), "Luna");
    ASSERT_EQ(charPtr->getMaxHp(), 80);

    auto mage = std::dynamic_pointer_cast<Mage>(charPtr);
    ASSERT_TRUE(mage != nullptr);
    ASSERT_EQ(mage->getMaxMana(), 50);
    ASSERT_EQ(mage->getSpellDamage(), 40);
    ASSERT_EQ(mage->getManaCost(), 10);
    ASSERT_EQ(mage->getFallbackDamage(), 15);
}

void testParseCharacterLine_HealerSuccess() {
    std::string line = "HEALER|103|Mercy|100|40|25|15|10";
    auto charPtr = DataFileManager::parseCharacterLine(line);
    ASSERT_TRUE(charPtr != nullptr);
    ASSERT_EQ(charPtr->getType(), "HEALER");
    ASSERT_EQ(charPtr->getId(), 103);
    ASSERT_EQ(charPtr->getName(), "Mercy");
    ASSERT_EQ(charPtr->getMaxHp(), 100);

    auto healer = std::dynamic_pointer_cast<Healer>(charPtr);
    ASSERT_TRUE(healer != nullptr);
    ASSERT_EQ(healer->getMaxMana(), 40);
    ASSERT_EQ(healer->getHealAmount(), 25);
    ASSERT_EQ(healer->getManaCost(), 15);
    ASSERT_EQ(healer->getFallbackDamage(), 10);
}

void testParseCharacterLine_CommentsAndEmpty() {
    ASSERT_TRUE(DataFileManager::parseCharacterLine("") == nullptr);
    ASSERT_TRUE(DataFileManager::parseCharacterLine("   ") == nullptr);
    ASSERT_TRUE(DataFileManager::parseCharacterLine("# This is a comment") == nullptr);
    ASSERT_TRUE(DataFileManager::parseCharacterLine("  # Indented comment") == nullptr);
}

void testParseCharacterLine_InvalidType() {
    ASSERT_TRUE(DataFileManager::parseCharacterLine("ARCHER|103|Robin|90|25") == nullptr);
    ASSERT_TRUE(DataFileManager::parseCharacterLine("UNKNOWN|1|Test|10|10") == nullptr);
}

void testParseCharacterLine_MissingOrExtraTokens() {
    // Missing tokens
    ASSERT_TRUE(DataFileManager::parseCharacterLine("WARRIOR|101|Ares|100") == nullptr);
    ASSERT_TRUE(DataFileManager::parseCharacterLine("MAGE|102|Luna|80|50|40|10") == nullptr);

    // Extra tokens (corrupted format)
    ASSERT_TRUE(DataFileManager::parseCharacterLine("WARRIOR|101|Ares|100|30|EXTRA") == nullptr);
    ASSERT_TRUE(DataFileManager::parseCharacterLine("MAGE|102|Luna|80|50|40|10|15|EXTRA") == nullptr);
}

void testParseCharacterLine_InvalidValues() {
    // Negative/Zero ID
    ASSERT_TRUE(DataFileManager::parseCharacterLine("WARRIOR|0|Ares|100|30") == nullptr);
    ASSERT_TRUE(DataFileManager::parseCharacterLine("WARRIOR|-5|Ares|100|30") == nullptr);
    
    // Empty Name
    ASSERT_TRUE(DataFileManager::parseCharacterLine("WARRIOR|101||100|30") == nullptr);
    
    // Invalid numeric string
    ASSERT_TRUE(DataFileManager::parseCharacterLine("WARRIOR|abc|Ares|100|30") == nullptr);
    ASSERT_TRUE(DataFileManager::parseCharacterLine("WARRIOR|101|Ares|abc|30") == nullptr);
    ASSERT_TRUE(DataFileManager::parseCharacterLine("WARRIOR|101|Ares|100|xyz") == nullptr);
    
    // Negative/Zero stats
    ASSERT_TRUE(DataFileManager::parseCharacterLine("WARRIOR|101|Ares|0|30") == nullptr);
    ASSERT_TRUE(DataFileManager::parseCharacterLine("WARRIOR|101|Ares|100|0") == nullptr);
    ASSERT_TRUE(DataFileManager::parseCharacterLine("MAGE|102|Luna|80|0|40|10|15") == nullptr);
    ASSERT_TRUE(DataFileManager::parseCharacterLine("MAGE|102|Luna|80|50|0|10|15") == nullptr);
}

// ---------------------------------------------------------------------------
// 2. Tests for serializeCharacter
// ---------------------------------------------------------------------------

void testSerializeCharacter_Success() {
    Warrior warrior(101, "Ares", 100, 30);
    std::string warriorStr = DataFileManager::serializeCharacter(warrior);
    ASSERT_EQ(warriorStr, "WARRIOR|101|Ares|100|30");

    Mage mage(102, "Luna", 80, 50, 40, 10, 15);
    std::string mageStr = DataFileManager::serializeCharacter(mage);
    ASSERT_EQ(mageStr, "MAGE|102|Luna|80|50|40|10|15");
}

void testSerializeCharacter_InvalidCharacter_ReturnsEmpty() {
    Warrior invalidWarrior(0, "Invalid", 100, 30);
    ASSERT_EQ(DataFileManager::serializeCharacter(invalidWarrior), "");

    Warrior emptyName(101, "", 100, 30);
    ASSERT_EQ(DataFileManager::serializeCharacter(emptyName), "");
}

// ---------------------------------------------------------------------------
// 3. Tests for loadCharacters & saveCharacters
// ---------------------------------------------------------------------------

void testLoadAndSaveCharacters_RoundTrip() {
    std::string tempFile = "temp_test_characters.txt";
    
    std::vector<std::shared_ptr<Character>> original;
    original.push_back(std::make_shared<Warrior>(101, "Ares", 100, 30));
    original.push_back(std::make_shared<Mage>(102, "Luna", 80, 50, 40, 10, 15));

    ASSERT_TRUE(DataFileManager::saveCharacters(tempFile, original));

    std::vector<std::shared_ptr<Character>> loaded;
    ASSERT_TRUE(DataFileManager::loadCharacters(tempFile, loaded));
    ASSERT_EQ(loaded.size(), static_cast<size_t>(2));

    ASSERT_EQ(loaded[0]->getId(), 101);
    ASSERT_EQ(loaded[0]->getName(), "Ares");
    ASSERT_EQ(loaded[1]->getId(), 102);
    ASSERT_EQ(loaded[1]->getName(), "Luna");

    removeTempFile(tempFile);
}

void testLoadCharacters_NonExistentFile() {
    std::vector<std::shared_ptr<Character>> loaded;
    ASSERT_FALSE(DataFileManager::loadCharacters("non_existent_file_xyz.txt", loaded));
}

// ---------------------------------------------------------------------------
// 4. Tests for parseTeamLine
// ---------------------------------------------------------------------------

void testParseTeamLine_Success() {
    Team team;
    std::string lineWithChars = "201|Red Team|101,102";
    ASSERT_TRUE(DataFileManager::parseTeamLine(lineWithChars, team));
    ASSERT_EQ(team.getId(), 201);
    ASSERT_EQ(team.getName(), "Red Team");
    ASSERT_EQ(team.getCharacterIds().size(), static_cast<size_t>(2));
    ASSERT_EQ(team.getCharacterIds()[0], 101);
    ASSERT_EQ(team.getCharacterIds()[1], 102);

    Team trailingPipeTeam;
    std::string lineTrailingPipe = "203|Green Team|";
    ASSERT_TRUE(DataFileManager::parseTeamLine(lineTrailingPipe, trailingPipeTeam));
    ASSERT_EQ(trailingPipeTeam.getId(), 203);
    ASSERT_EQ(trailingPipeTeam.getName(), "Green Team");
    ASSERT_TRUE(trailingPipeTeam.getCharacterIds().empty());
}

void testParseTeamLine_CommentsAndEmpty() {
    Team team;
    ASSERT_FALSE(DataFileManager::parseTeamLine("", team));
    ASSERT_FALSE(DataFileManager::parseTeamLine("   ", team));
    ASSERT_FALSE(DataFileManager::parseTeamLine("# Team comment", team));
}

void testParseTeamLine_MissingOrExtraTokens() {
    Team team;
    // Missing team name (1 token)
    ASSERT_FALSE(DataFileManager::parseTeamLine("201", team));

    // Missing character list field (2 tokens, missing 3rd field delimiter)
    ASSERT_FALSE(DataFileManager::parseTeamLine("201|Red Team", team));

    // Extra token field (4 tokens)
    ASSERT_FALSE(DataFileManager::parseTeamLine("201|Red Team|101,102|EXTRA", team));
}

void testParseTeamLine_InvalidValues() {
    Team team;
    // Invalid ID / Name
    ASSERT_FALSE(DataFileManager::parseTeamLine("0|Red Team|101", team));
    ASSERT_FALSE(DataFileManager::parseTeamLine("-5|Red Team|101", team));
    ASSERT_FALSE(DataFileManager::parseTeamLine("abc|Red Team|101", team));
    ASSERT_FALSE(DataFileManager::parseTeamLine("201||101", team));

    // Malformed Character IDs
    ASSERT_FALSE(DataFileManager::parseTeamLine("201|Red Team|101,abc,102", team));
    ASSERT_FALSE(DataFileManager::parseTeamLine("201|Red Team|101,-5,102", team));
    ASSERT_FALSE(DataFileManager::parseTeamLine("201|Red Team|101,10.5,102", team));

    // Duplicate Character IDs
    ASSERT_FALSE(DataFileManager::parseTeamLine("201|Red Team|101,101", team));
}

// ---------------------------------------------------------------------------
// 5. Tests for serializeTeam
// ---------------------------------------------------------------------------

void testSerializeTeam_Success() {
    Team team1(201, "Red Team", {101, 102});
    ASSERT_EQ(DataFileManager::serializeTeam(team1), "201|Red Team|101,102");

    Team team2(202, "Blue Team", {});
    ASSERT_EQ(DataFileManager::serializeTeam(team2), "202|Blue Team|");
}

void testSerializeTeam_InvalidTeam_ReturnsEmpty() {
    Team invalidTeam1(0, "Invalid", {101});
    ASSERT_EQ(DataFileManager::serializeTeam(invalidTeam1), "");

    Team invalidTeam2(201, "", {101});
    ASSERT_EQ(DataFileManager::serializeTeam(invalidTeam2), "");
}

// ---------------------------------------------------------------------------
// 6. Tests for loadTeams & saveTeams
// ---------------------------------------------------------------------------

void testLoadAndSaveTeams_RoundTrip() {
    std::string tempFile = "temp_test_teams.txt";

    std::vector<Team> original;
    original.push_back(Team(201, "Red Team", {101, 102}));
    original.push_back(Team(202, "Blue Team", {}));

    ASSERT_TRUE(DataFileManager::saveTeams(tempFile, original));

    std::vector<Team> loaded;
    ASSERT_TRUE(DataFileManager::loadTeams(tempFile, loaded));
    ASSERT_EQ(loaded.size(), static_cast<size_t>(2));

    ASSERT_EQ(loaded[0].getId(), 201);
    ASSERT_EQ(loaded[0].getName(), "Red Team");
    ASSERT_EQ(loaded[0].getCharacterIds().size(), static_cast<size_t>(2));

    ASSERT_EQ(loaded[1].getId(), 202);
    ASSERT_EQ(loaded[1].getName(), "Blue Team");
    ASSERT_TRUE(loaded[1].getCharacterIds().empty());

    removeTempFile(tempFile);
}

void testLoadTeams_NonExistentFile() {
    std::vector<Team> loaded;
    ASSERT_FALSE(DataFileManager::loadTeams("non_existent_teams_xyz.txt", loaded));
}

void testLoadCharacters_CorruptedLines_SkipsAndLoadsValid() {
    std::string tempFile = "temp_test_corrupted_chars.txt";
    std::ofstream out(tempFile);
    out << "WARRIOR|101|Ares|100|30\n";
    out << "INVALID_LINE_CORRUPTED|999\n"; // corrupted line
    out << "MAGE|102|Luna|80|50|40|10|15\n";
    out.close();

    std::vector<std::shared_ptr<Character>> loaded;
    ASSERT_TRUE(DataFileManager::loadCharacters(tempFile, loaded));
    // Must load 2 valid characters and completely skip line 2 without crash
    ASSERT_EQ(loaded.size(), static_cast<size_t>(2));
    ASSERT_EQ(loaded[0]->getId(), 101);
    ASSERT_EQ(loaded[1]->getId(), 102);

    removeTempFile(tempFile);
}

void testLoadTeams_CorruptedLines_SkipsAndLoadsValid() {
    std::string tempFile = "temp_test_corrupted_teams.txt";
    std::ofstream out(tempFile);
    out << "201|Red Team|101,102\n";
    out << "202|Corrupted Team|101,abc,102\n"; // invalid character ID list token
    out << "203|Green Team|\n";
    out.close();

    std::vector<Team> loaded;
    ASSERT_TRUE(DataFileManager::loadTeams(tempFile, loaded));
    // Must load 2 valid teams and completely discard line 2 (not keeping partial IDs)
    ASSERT_EQ(loaded.size(), static_cast<size_t>(2));
    ASSERT_EQ(loaded[0].getId(), 201);
    ASSERT_EQ(loaded[1].getId(), 203);

    removeTempFile(tempFile);
}

// ---------------------------------------------------------------------------
// Main Runner
// ---------------------------------------------------------------------------

int main() {
    std::cout << "==================================================" << std::endl;
    std::cout << "      DataFileManager Unit Test Runner            " << std::endl;
    std::cout << "==================================================" << std::endl;

    RUN_TEST(testParseCharacterLine_WarriorSuccess);
    RUN_TEST(testParseCharacterLine_MageSuccess);
    RUN_TEST(testParseCharacterLine_HealerSuccess);
    RUN_TEST(testParseCharacterLine_CommentsAndEmpty);
    RUN_TEST(testParseCharacterLine_InvalidType);
    RUN_TEST(testParseCharacterLine_MissingOrExtraTokens);
    RUN_TEST(testParseCharacterLine_InvalidValues);

    RUN_TEST(testSerializeCharacter_Success);
    RUN_TEST(testSerializeCharacter_InvalidCharacter_ReturnsEmpty);

    RUN_TEST(testLoadAndSaveCharacters_RoundTrip);
    RUN_TEST(testLoadCharacters_NonExistentFile);
    RUN_TEST(testLoadCharacters_CorruptedLines_SkipsAndLoadsValid);

    RUN_TEST(testParseTeamLine_Success);
    RUN_TEST(testParseTeamLine_CommentsAndEmpty);
    RUN_TEST(testParseTeamLine_MissingOrExtraTokens);
    RUN_TEST(testParseTeamLine_InvalidValues);

    RUN_TEST(testSerializeTeam_Success);
    RUN_TEST(testSerializeTeam_InvalidTeam_ReturnsEmpty);

    RUN_TEST(testLoadAndSaveTeams_RoundTrip);
    RUN_TEST(testLoadTeams_NonExistentFile);
    RUN_TEST(testLoadTeams_CorruptedLines_SkipsAndLoadsValid);

    std::cout << "==================================================" << std::endl;
    std::cout << " Summary: " << g_testsPassed << " passed, " << g_testsFailed << " failed." << std::endl;
    std::cout << "==================================================" << std::endl;

    return g_testsFailed == 0 ? 0 : 1;
}
