/**
 * @file BattleEngine.h
 * @module Battle
 * @brief Orchestrates turn-based battles between two teams using Character pointers.
 */

#ifndef TURN_BASED_ADVENTURE_GAME_BATTLE_ENGINE_H
#define TURN_BASED_ADVENTURE_GAME_BATTLE_ENGINE_H

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "BattleState.h"
#include "Team.h"

namespace TurnBasedGame {

class Character;
class CharacterRoster;
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
    const Character* getBattleCharacter(const Team& team, int characterId) const;
    const Character* getBattleCharacter(const Team* team, int characterId) const;

private:
    bool startBattleInternal(const Team& teamA, const Team& teamB, CharacterRoster& roster);
    bool validateBattleSetup(const Team& teamA, const Team& teamB, const CharacterRoster& roster) const;

    Character* resolveCharacterInTeam(const Team* team, int characterId);
    const Character* resolveCharacterInTeam(const Team* team, int characterId) const;
    Character* findNextAliveCharacter(const Team& team, std::size_t& cursor);

    const std::vector<std::unique_ptr<Character>>& getTeamCharacterInstances(const Team* team) const;
    std::vector<std::unique_ptr<Character>>& getTeamCharacterInstances(const Team* team);

    const Team* currentTeam() const;
    std::size_t& currentCursor();
    bool prepareCurrentActor(const Team* winnerIfCurrentTeamEmpty);
    void moveToNextTeam();

    CharacterRoster* m_roster{nullptr};
    const Team* m_teamA{nullptr};      // trỏ vào m_teamASnapshot
    const Team* m_teamB{nullptr};      // trỏ vào m_teamBSnapshot
    const Team* m_winnerTeam{nullptr}; // trỏ vào một trong hai snapshot
    Team m_teamASnapshot;              // bản sao của teamA tại thời điểm startBattle
    Team m_teamBSnapshot;              // bản sao của teamB tại thời điểm startBattle
    BattleState m_state{BattleState::READY};
    std::size_t m_roundsPlayed{0};
    // 0 = team A, 1 = team B; dùng để luân phiên lượt đánh.
    int m_activeTeamIndex{0};
    std::size_t m_nextIndexA{0};
    std::size_t m_nextIndexB{0};
    int m_currentActorId{0};

    std::vector<std::unique_ptr<Character>> m_teamACharacters;
    std::vector<std::unique_ptr<Character>> m_teamBCharacters;
};

} // namespace TurnBasedGame

#endif // TURN_BASED_ADVENTURE_GAME_BATTLE_ENGINE_H
