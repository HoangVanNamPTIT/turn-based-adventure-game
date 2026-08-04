/**
 * @file Utils.cpp
 * @module Utils
 * @brief Skeleton implementations of utility functions.
 */

#include "Utils.h"
#include <algorithm>
#include <cctype>
/**
 * @file Utils.cpp
 * @module Utils
 * @brief Skeleton implementations of utility functions.
 */

#include "Utils.h"
#include <algorithm>
#include <cctype>
#include <sstream>

namespace TurnBasedGame {

namespace Utils {

std::string Utils::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

std::vector<std::string> Utils::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(trim(token));
    }
    if (!str.empty() && str.back() == delimiter) {
        tokens.push_back("");
    }
    return tokens;
}

bool Utils::isInteger(const std::string& str) {
    std::string trimmed = trim(str);
    if (trimmed.empty()) return false;
    size_t start = 0;
    if (trimmed[0] == '-' || trimmed[0] == '+') {
        if (trimmed.length() == 1) return false;
        start = 1;
    }
    for (size_t i = start; i < trimmed.length(); ++i) {
        if (!std::isdigit(static_cast<unsigned char>(trimmed[i]))) {
            return false;
        }
    }
    return true;
}

int Utils::parseInt(const std::string& str, int defaultValue) {
    if (!isInteger(str)) {
        return defaultValue;
    }
    try {
        return std::stoi(trim(str));
    } catch (...) {
        return defaultValue;
    }
}

} // namespace Utils
} // namespace TurnBasedGame
