/**
 * @file BattleState.cpp
 * @module Battle
 * @brief Helper implementation for battle state strings.
 */

#include "BattleState.h"

namespace TurnBasedGame {

std::string battleStateToString(BattleState state) {
    switch (state) {
        case BattleState::READY:
            return "READY";
        case BattleState::IN_PROGRESS:
            return "IN_PROGRESS";
        case BattleState::FINISHED:
            return "FINISHED";
        default:
            return "UNKNOWN";
    }
}

} // namespace TurnBasedGame
