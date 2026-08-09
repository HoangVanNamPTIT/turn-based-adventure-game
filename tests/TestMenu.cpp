/**
 * @file TestMenu.cpp
 * @brief Unit tests for Menu component using stream dependency injection.
 */

#include "Menu.h"
#include "CharacterRoster.h"
#include "TeamManager.h"
#include "BattleEngine.h"
#include "Warrior.h"
#include "Mage.h"

#include <iostream>
#include <sstream>
#include <cassert>
#include <stdexcept>
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
// 1. Input Reading Helpers Tests
// ---------------------------------------------------------------------------

void testMenu_ReadInt() {
    std::stringstream input("10\n-5\nabc\n2\n");
    std::stringstream output;
    Menu menu(input, output);

    int val = 0;
    // Reads 10 (range 1-5 -> out of range, retries with -5 -> out of range, retries with abc -> invalid, retries with 2 -> valid)
    ASSERT_TRUE(menu.readInt("Enter option: ", 1, 5, val));
    ASSERT_EQ(val, 2);
}

void testMenu_ReadPositiveInt() {
    std::stringstream input("0\n-10\n101\n");
    std::stringstream output;
    Menu menu(input, output);

    int val = 0;
    ASSERT_TRUE(menu.readPositiveInt("Enter ID: ", val));
    ASSERT_EQ(val, 101);
}

void testMenu_ReadRequiredTextAndName() {
    std::stringstream input("   \n  Hero Name  \n");
    std::stringstream output;
    Menu menu(input, output);

    std::string text;
    ASSERT_TRUE(menu.readRequiredText("Enter text: ", text));
    ASSERT_EQ(text, "Hero Name");
}

void testMenu_Confirm() {
    std::stringstream input1("y\n");
    std::stringstream output1;
    Menu menu1(input1, output1);
    ASSERT_TRUE(menu1.confirm("Proceed?"));

    std::stringstream input2("n\n");
    std::stringstream output2;
    Menu menu2(input2, output2);
    ASSERT_FALSE(menu2.confirm("Proceed?"));
}

// ---------------------------------------------------------------------------
// 2. Menu Screen Display Tests
// ---------------------------------------------------------------------------

void testMenu_Displays() {
    std::stringstream input("0\n");
    std::stringstream output;
    Menu menu(input, output);

    int choice = menu.showMainMenu();
    ASSERT_EQ(choice, 0);

    CharacterRoster roster;
    roster.addWarrior(101, "Ares", 100, 30);
    menu.displayRoster(roster);

    TeamManager tm;
    tm.createTeam(201, "Red Team");
    menu.displayTeams(tm);

    menu.showSuccess("Operation completed");
    menu.showError("Operation failed");

    std::string outStr = output.str();
    ASSERT_TRUE(outStr.find("TURN-BASED ADVENTURE GAME") != std::string::npos);
    ASSERT_TRUE(outStr.find("Ares") != std::string::npos);
    ASSERT_TRUE(outStr.find("Red Team") != std::string::npos);
}

int main() {
    std::cout << "==================================================" << std::endl;
    std::cout << "          Menu Unit Test Runner                   " << std::endl;
    std::cout << "==================================================" << std::endl;

    RUN_TEST(testMenu_ReadInt);
    RUN_TEST(testMenu_ReadPositiveInt);
    RUN_TEST(testMenu_ReadRequiredTextAndName);
    RUN_TEST(testMenu_Confirm);
    RUN_TEST(testMenu_Displays);

    std::cout << "==================================================" << std::endl;
    std::cout << " Summary: " << g_testsPassed << " passed, " << g_testsFailed << " failed." << std::endl;
    std::cout << "==================================================" << std::endl;

    return g_testsFailed == 0 ? 0 : 1;
}
