/**
 * @file TestUtils.cpp
 * @brief Unit tests for TurnBasedGame::Utils module.
 */

#include "Utils.h"
#include <iostream>
#include <cassert>
#include <stdexcept>
#include <vector>
#include <string>

using namespace TurnBasedGame::Utils;

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
// 1. Tests for Utils::trim
// ---------------------------------------------------------------------------

void testTrim_BasicSpaces() {
    ASSERT_EQ(Utils::trim("  hello  "), "hello");
    ASSERT_EQ(Utils::trim("\tworld\n\r"), "world");
    ASSERT_EQ(Utils::trim("   multiple   words   "), "multiple   words");
}

void testTrim_EdgeCases() {
    ASSERT_EQ(Utils::trim(""), "");
    ASSERT_EQ(Utils::trim("   "), "");
    ASSERT_EQ(Utils::trim("\t\n\r "), "");
    ASSERT_EQ(Utils::trim("a"), "a");
}

// ---------------------------------------------------------------------------
// 2. Tests for Utils::split
// ---------------------------------------------------------------------------

void testSplit_Basic() {
    std::string input = "WARRIOR|101|Ares|100|30";
    auto tokens = Utils::split(input, '|');
    ASSERT_EQ(tokens.size(), static_cast<size_t>(5));
    ASSERT_EQ(tokens[0], "WARRIOR");
    ASSERT_EQ(tokens[1], "101");
    ASSERT_EQ(tokens[2], "Ares");
    ASSERT_EQ(tokens[3], "100");
    ASSERT_EQ(tokens[4], "30");
}

void testSplit_WithWhitespace() {
    std::string input = "  MAGE  | 102 |  Luna  ";
    auto tokens = Utils::split(input, '|');
    ASSERT_EQ(tokens.size(), static_cast<size_t>(3));
    ASSERT_EQ(tokens[0], "MAGE");
    ASSERT_EQ(tokens[1], "102");
    ASSERT_EQ(tokens[2], "Luna");
}

void testSplit_EmptyAndTrailing() {
    std::string input = "a,b,c,";
    auto tokens = Utils::split(input, ',');
    ASSERT_EQ(tokens.size(), static_cast<size_t>(4));
    ASSERT_EQ(tokens[3], "");

    std::string emptyStr = "";
    auto emptyTokens = Utils::split(emptyStr, ',');
    ASSERT_EQ(emptyTokens.size(), static_cast<size_t>(0));
}

// ---------------------------------------------------------------------------
// 3. Tests for Utils::isInteger
// ---------------------------------------------------------------------------

void testIsInteger_Valid() {
    ASSERT_TRUE(Utils::isInteger("123"));
    ASSERT_TRUE(Utils::isInteger("-456"));
    ASSERT_TRUE(Utils::isInteger("+789"));
    ASSERT_TRUE(Utils::isInteger("0"));
    ASSERT_TRUE(Utils::isInteger("  100  "));
}

void testIsInteger_Invalid() {
    ASSERT_FALSE(Utils::isInteger(""));
    ASSERT_FALSE(Utils::isInteger("   "));
    ASSERT_FALSE(Utils::isInteger("abc"));
    ASSERT_FALSE(Utils::isInteger("12.34"));
    ASSERT_FALSE(Utils::isInteger("12a3"));
    ASSERT_FALSE(Utils::isInteger("-"));
    ASSERT_FALSE(Utils::isInteger("+"));
}

// ---------------------------------------------------------------------------
// 4. Tests for Utils::parseInt
// ---------------------------------------------------------------------------

void testParseInt_Valid() {
    ASSERT_EQ(Utils::parseInt("123"), 123);
    ASSERT_EQ(Utils::parseInt("-456"), -456);
    ASSERT_EQ(Utils::parseInt("0"), 0);
    ASSERT_EQ(Utils::parseInt("  99  "), 99);
}

void testParseInt_InvalidFallback() {
    ASSERT_EQ(Utils::parseInt("abc", -1), -1);
    ASSERT_EQ(Utils::parseInt("", 42), 42);
    ASSERT_EQ(Utils::parseInt("12.34", 0), 0);
    ASSERT_EQ(Utils::parseInt("99999999999999999999999", 0), 0); // Overflow fallback
}

// ---------------------------------------------------------------------------
// Main runner
// ---------------------------------------------------------------------------

int main() {
    std::cout << "==================================================" << std::endl;
    std::cout << "         Utils Unit Test Runner                   " << std::endl;
    std::cout << "==================================================" << std::endl;

    RUN_TEST(testTrim_BasicSpaces);
    RUN_TEST(testTrim_EdgeCases);
    RUN_TEST(testSplit_Basic);
    RUN_TEST(testSplit_WithWhitespace);
    RUN_TEST(testSplit_EmptyAndTrailing);
    RUN_TEST(testIsInteger_Valid);
    RUN_TEST(testIsInteger_Invalid);
    RUN_TEST(testParseInt_Valid);
    RUN_TEST(testParseInt_InvalidFallback);

    std::cout << "==================================================" << std::endl;
    std::cout << " Summary: " << g_testsPassed << " passed, " << g_testsFailed << " failed." << std::endl;
    std::cout << "==================================================" << std::endl;

    return g_testsFailed == 0 ? 0 : 1;
}
