/**
 * @file BattleEngine.h
 * @module Battle
 * @brief Orchestrates turn-based battles between two teams using Character pointers.
 */

#ifndef TURN_BASED_ADVENTURE_GAME_BATTLE_ENGINE_H
#define TURN_BASED_ADVENTURE_GAME_BATTLE_ENGINE_H

#include <cstddef>
#include <string>

#include "BattleState.h"

namespace TurnBasedGame {

class Character;
class CharacterRoster;
class Team;
class TeamManager;

class BattleEngine {
public:
    BattleEngine() = default;

    bool startBattle(const Team& teamA, const Team& teamB, CharacterRoster& roster);

    bool startBattle(int teamAId, int teamBId, const TeamManager& teamManager, CharacterRoster& roster);

    bool performAction(int actorId, int targetId);

    void resetBattle();

    BattleState getState() const;
    const Team* getTeamA() const;
    const Team* getTeamB() const;
    const Team* getActiveTeam() const;
    const Team* getWinnerTeam() const;
    const Character* getCurrentActor() const;
    int getCurrentActorId() const;
    std::size_t getRoundsPlayed() const;
    std::string getStatusMessage() const;
    bool teamContainsCharacter(const Team& team, int characterId) const;

private:
    bool startBattleInternal(const Team& teamA, const Team& teamB, CharacterRoster& roster);
    bool validateBattleSetup(const Team& teamA, const Team& teamB, const CharacterRoster& roster) const;

    Character* resolveCharacter(int characterId);
    const Character* resolveCharacter(int characterId) const;
    Character* findNextAliveCharacter(const Team& team, std::size_t& cursor);

    const Team* currentTeam() const;
    std::size_t& currentCursor();
    bool prepareCurrentActor(const Team* winnerIfCurrentTeamEmpty);
    void moveToNextTeam();

    CharacterRoster* m_roster{nullptr};
    const Team* m_teamA{nullptr};
    const Team* m_teamB{nullptr};
    const Team* m_winnerTeam{nullptr};
    BattleState m_state{BattleState::READY};
    std::size_t m_roundsPlayed{0};
    // 0 = team A, 1 = team B; dùng để luân phiên lượt đánh.
    int m_activeTeamIndex{0};
    std::size_t m_nextIndexA{0};
    std::size_t m_nextIndexB{0};
    int m_currentActorId{0};
};

} // namespace TurnBasedGame

#endif // TURN_BASED_ADVENTURE_GAME_BATTLE_ENGINE_H
