/**
 * @file TestTeam.cpp
 * @brief Unit tests for Team class.
 */

#include "Team.h"
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
// 1. Team Constructor and ID/Name tests
// ---------------------------------------------------------------------------

void testTeam_ConstructorsAndSetters() {
    Team defaultTeam;
    ASSERT_EQ(defaultTeam.getId(), 0);
    ASSERT_EQ(defaultTeam.getName(), "");
    ASSERT_TRUE(defaultTeam.getCharacterIds().empty());

    Team team1(10, "Alpha Squad", {101, 102});
    ASSERT_EQ(team1.getId(), 10);
    ASSERT_EQ(team1.getName(), "Alpha Squad");
    ASSERT_EQ(team1.getCharacterIds().size(), static_cast<size_t>(2));

    // setId validation (id <= 0 clamped to 0)
    team1.setId(-5);
    ASSERT_EQ(team1.getId(), 0);
    team1.setId(15);
    ASSERT_EQ(team1.getId(), 15);

    // setName validation (empty name rejected)
    ASSERT_TRUE(team1.setName("Beta Squad"));
    ASSERT_EQ(team1.getName(), "Beta Squad");
    ASSERT_FALSE(team1.setName(""));
    ASSERT_EQ(team1.getName(), "Beta Squad");
}

// ---------------------------------------------------------------------------
// 2. Character ID Management tests
// ---------------------------------------------------------------------------

void testTeam_CharacterIdOperations() {
    Team team(1, "Red Team");

    // Add character IDs
    team.addCharacterId(101);
    team.addCharacterId(102);
    ASSERT_EQ(team.getCharacterIds().size(), static_cast<size_t>(2));
    ASSERT_TRUE(team.hasCharacterId(101));
    ASSERT_TRUE(team.hasCharacterId(102));

    // Duplicate addition ignored
    team.addCharacterId(101);
    ASSERT_EQ(team.getCharacterIds().size(), static_cast<size_t>(2));

    // Negative/Zero ID ignored
    team.addCharacterId(0);
    team.addCharacterId(-10);
    ASSERT_EQ(team.getCharacterIds().size(), static_cast<size_t>(2));

    // Remove character ID
    ASSERT_TRUE(team.removeCharacterId(101));
    ASSERT_FALSE(team.hasCharacterId(101));
    ASSERT_EQ(team.getCharacterIds().size(), static_cast<size_t>(1));

    // Remove non-existent ID
    ASSERT_FALSE(team.removeCharacterId(999));
    ASSERT_EQ(team.getCharacterIds().size(), static_cast<size_t>(1));

    // Bulk set character IDs
    std::vector<int> newIds = {201, 202, 203};
    team.setCharacterIds(newIds);
    ASSERT_EQ(team.getCharacterIds().size(), static_cast<size_t>(3));
    ASSERT_TRUE(team.hasCharacterId(202));
}

int main() {
    std::cout << "==================================================" << std::endl;
    std::cout << "          Team Unit Test Runner                   " << std::endl;
    std::cout << "==================================================" << std::endl;

    RUN_TEST(testTeam_ConstructorsAndSetters);
    RUN_TEST(testTeam_CharacterIdOperations);

    std::cout << "==================================================" << std::endl;
    std::cout << " Summary: " << g_testsPassed << " passed, " << g_testsFailed << " failed." << std::endl;
    std::cout << "==================================================" << std::endl;

    return g_testsFailed == 0 ? 0 : 1;
}
