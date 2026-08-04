/**
 * @file Utils.h
 * @module Utils
 * @brief Small helper utilities for string parsing and validation.
 * @features parsing CSV lines, trimming, splitting, and integer checks.
 * @input raw file lines and user input.
 * @output parsed tokens and validation booleans.
 */

#ifndef TURN_BASED_ADVENTURE_GAME_UTILS_H
#define TURN_BASED_ADVENTURE_GAME_UTILS_H

#include <string>
#include <vector>

namespace TurnBasedGame {

namespace Utils {

class Utils {
public:
    static std::string trim(const std::string& str);
    static std::vector<std::string> split(const std::string& str, char delimiter);
    static bool isInteger(const std::string& str);
    static int parseInt(const std::string& str, int defaultValue = 0);
};

} // namespace Utils

} // namespace TurnBasedGame

#endif // TURN_BASED_ADVENTURE_GAME_UTILS_H
