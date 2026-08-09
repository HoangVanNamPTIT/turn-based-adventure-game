/**
 * @file TestGameApp.cpp
 * @brief Unit tests for GameApp orchestrator component.
 */

#include "GameApp.h"

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
// 1. GameApp Execution Flow Tests
// ---------------------------------------------------------------------------

void testGameApp_ExitImmediately() {
    // Input choice 0 -> Exit program
    std::stringstream input("0\n");
    std::stringstream output;

    GameApp app(input, output);
    int exitCode = app.run();

    ASSERT_EQ(exitCode, 0);
    std::string outStr = output.str();
    ASSERT_TRUE(outStr.find("TURN-BASED ADVENTURE GAME") != std::string::npos);
}

void testGameApp_SubMenuNavigationAndExit() {
    // 1 -> Character menu, 0 -> Back to main menu, 2 -> Team menu, 0 -> Back to main menu, 0 -> Exit
    std::stringstream input("1\n0\n2\n0\n0\n");
    std::stringstream output;

    GameApp app(input, output);
    int exitCode = app.run();

    ASSERT_EQ(exitCode, 0);
}

int main() {
    std::cout << "==================================================" << std::endl;
    std::cout << "        GameApp Unit Test Runner                  " << std::endl;
    std::cout << "==================================================" << std::endl;

    RUN_TEST(testGameApp_ExitImmediately);
    RUN_TEST(testGameApp_SubMenuNavigationAndExit);

    std::cout << "==================================================" << std::endl;
    std::cout << " Summary: " << g_testsPassed << " passed, " << g_testsFailed << " failed." << std::endl;
    std::cout << "==================================================" << std::endl;

    return g_testsFailed == 0 ? 0 : 1;
}
