/**
 * @file BattleState.h
 * @module Battle
 * @brief Defines the battle lifecycle state enumeration used across modules.
 * @features Enumeration for battle phases and simple helpers.
 * @input None
 * @output Enum values indicating battle state.
 */

#ifndef TURN_BASED_ADVENTURE_GAME_BATTLE_STATE_H
#define TURN_BASED_ADVENTURE_GAME_BATTLE_STATE_H

#include <string>

namespace TurnBasedGame {

enum class BattleState {
    READY,
    IN_PROGRESS,
    FINISHED
};

std::string battleStateToString(BattleState state);

} // namespace TurnBasedGame

#endif // TURN_BASED_ADVENTURE_GAME_BATTLE_STATE_H
