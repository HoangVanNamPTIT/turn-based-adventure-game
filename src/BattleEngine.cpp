#include "BattleEngine.h"

#include "Character.h"
#include "CharacterRoster.h"
#include "Team.h"
#include "TeamManager.h"

#include <algorithm>
#include <sstream>
#include <vector>

namespace TurnBasedGame {

BattleState BattleEngine::getState() const {
    return m_state;
}

const Team* BattleEngine::getTeamA() const {
    return m_teamA;
}

const Team* BattleEngine::getTeamB() const {
    return m_teamB;
}

const Team* BattleEngine::getWinnerTeam() const {
    return m_winnerTeam;
}

const Team* BattleEngine::getActiveTeam() const {
    return currentTeam();
}

const Character* BattleEngine::getCurrentActor() const {
    if (m_state != BattleState::IN_PROGRESS) {
        return nullptr;
    }

    return resolveCharacterInTeam(currentTeam(), m_currentActorId);
}

int BattleEngine::getCurrentActorId() const {
    return m_currentActorId;
}

std::size_t BattleEngine::getRoundsPlayed() const {
    return m_roundsPlayed;
}

std::string BattleEngine::getStatusMessage() const {
    std::ostringstream builder;
    builder << "State=" << battleStateToString(m_state);
    builder << ", rounds=" << m_roundsPlayed;

    if (m_teamA != nullptr) {
        builder << ", teamA=" << m_teamA->getName();
    }

    if (m_teamB != nullptr) {
        builder << ", teamB=" << m_teamB->getName();
    }

    if (m_winnerTeam != nullptr) {
        builder << ", winner=" << m_winnerTeam->getName();
    }

    if (m_state == BattleState::IN_PROGRESS && m_currentActorId > 0) {
        builder << ", currentActor=" << m_currentActorId;
    }

    return builder.str();
}

const Character* BattleEngine::getBattleCharacter(const Team& team, int characterId) const {
    return getBattleCharacter(&team, characterId);
}

const Character* BattleEngine::getBattleCharacter(const Team* team, int characterId) const {
    return resolveCharacterInTeam(team, characterId);
}

void BattleEngine::resetBattle() {
    m_teamACharacters.clear();
    m_teamBCharacters.clear();
    m_roster = nullptr;
    m_teamA = nullptr;
    m_teamB = nullptr;
    m_winnerTeam = nullptr;
    m_state = BattleState::READY;
    m_roundsPlayed = 0U;
    m_activeTeamIndex = 0;
    m_nextIndexA = 0U;
    m_nextIndexB = 0U;
    m_currentActorId = 0;
}

bool BattleEngine::startBattle(const Team& teamA, const Team& teamB, CharacterRoster& roster) {
    return startBattleInternal(teamA, teamB, roster);
}

bool BattleEngine::startBattle(int teamAId, int teamBId, const TeamManager& teamManager, CharacterRoster& roster) {
    const Team* teamA = teamManager.getTeamById(teamAId);
    const Team* teamB = teamManager.getTeamById(teamBId);

    if (teamA == nullptr || teamB == nullptr) {
        return false;
    }

    return startBattleInternal(*teamA, *teamB, roster);
}

bool BattleEngine::performAction(int actorId, int targetId) {
    if (m_state != BattleState::IN_PROGRESS
        || actorId <= 0
        || targetId <= 0
        || m_teamA == nullptr
        || m_teamB == nullptr) {
        return false;
    }

    const Team* activeTeam = currentTeam();
    const Team* enemyTeam = (activeTeam == m_teamA) ? m_teamB : m_teamA;

    if (activeTeam == nullptr || enemyTeam == nullptr) {
        return false;
    }

    if (m_currentActorId != actorId) {
        return false;
    }

    Character* actor = resolveCharacterInTeam(activeTeam, actorId);
    Character* target = resolveCharacterInTeam(enemyTeam, targetId);

    if (actor == nullptr
        || target == nullptr
        || actor == target
        || !actor->isAlive()
        || !target->isAlive()) {
        return false;
    }

    actor->performAction(*target);
    ++m_roundsPlayed;

    moveToNextTeam();

    return true;
}

bool BattleEngine::startBattleInternal(const Team& teamA, const Team& teamB, CharacterRoster& roster) {
    if (!validateBattleSetup(teamA, teamB, roster)) {
        return false;
    }

    resetBattle();

    m_roster = &roster;
    m_teamA = &teamA;
    m_teamB = &teamB;

    m_teamACharacters.clear();
    for (int characterId : teamA.getCharacterIds()) {
        const Character* original = roster.findById(characterId);
        if (original == nullptr) {
            resetBattle();
            return false;
        }
        std::unique_ptr<Character> cloned = original->clone();
        cloned->resetBattleState();
        m_teamACharacters.push_back(std::move(cloned));
    }

    m_teamBCharacters.clear();
    for (int characterId : teamB.getCharacterIds()) {
        const Character* original = roster.findById(characterId);
        if (original == nullptr) {
            resetBattle();
            return false;
        }
        std::unique_ptr<Character> cloned = original->clone();
        cloned->resetBattleState();
        m_teamBCharacters.push_back(std::move(cloned));
    }

    m_state = BattleState::IN_PROGRESS;
    m_activeTeamIndex = 0;
    m_nextIndexA = 0U;
    m_nextIndexB = 0U;

    if (!prepareCurrentActor(nullptr)) {
        resetBattle();
        return false;
    }

    return true;
}

bool BattleEngine::validateBattleSetup(const Team& teamA, const Team& teamB, const CharacterRoster& roster) const {
    if (&teamA == &teamB
        || teamA.getId() == teamB.getId()
        || roster.empty()
        || teamA.getCharacterIds().empty()
        || teamB.getCharacterIds().empty()) {
        return false;
    }

    for (int characterId : teamA.getCharacterIds()) {
        if (roster.findById(characterId) == nullptr) {
            return false;
        }
    }

    for (int characterId : teamB.getCharacterIds()) {
        if (roster.findById(characterId) == nullptr) {
            return false;
        }
    }

    return true;
}

bool BattleEngine::teamContainsCharacter(const Team& team, int characterId) const {
    const auto& ids = team.getCharacterIds();
    return std::find(ids.begin(), ids.end(), characterId) != ids.end();
}

const std::vector<std::unique_ptr<Character>>& BattleEngine::getTeamCharacterInstances(const Team* team) const {
    static const std::vector<std::unique_ptr<Character>> emptyVec;
    if (team == nullptr) return emptyVec;
    if (m_teamA != nullptr && team->getId() == m_teamA->getId()) {
        return m_teamACharacters;
    }
    if (m_teamB != nullptr && team->getId() == m_teamB->getId()) {
        return m_teamBCharacters;
    }
    return emptyVec;
}

std::vector<std::unique_ptr<Character>>& BattleEngine::getTeamCharacterInstances(const Team* team) {
    static std::vector<std::unique_ptr<Character>> emptyVec;
    if (team == nullptr) return emptyVec;
    if (m_teamA != nullptr && team->getId() == m_teamA->getId()) {
        return m_teamACharacters;
    }
    if (m_teamB != nullptr && team->getId() == m_teamB->getId()) {
        return m_teamBCharacters;
    }
    return emptyVec;
}

Character* BattleEngine::resolveCharacterInTeam(const Team* team, int characterId) {
    auto& instances = getTeamCharacterInstances(team);
    for (auto& character : instances) {
        if (character != nullptr && character->getId() == characterId) {
            return character.get();
        }
    }
    return nullptr;
}

const Character* BattleEngine::resolveCharacterInTeam(const Team* team, int characterId) const {
    const auto& instances = getTeamCharacterInstances(team);
    for (const auto& character : instances) {
        if (character != nullptr && character->getId() == characterId) {
            return character.get();
        }
    }
    return nullptr;
}

Character* BattleEngine::findNextAliveCharacter(const Team& team,
                                                std::size_t& cursor) {
    auto& instances = getTeamCharacterInstances(&team);
    if (instances.empty()) {
        return nullptr;
    }

    const std::size_t startCursor = cursor % instances.size();
    std::size_t checked = 0;

    while (checked < instances.size()) {
        std::size_t index = (startCursor + checked) % instances.size();
        ++checked;

        Character* character = instances[index].get();
        if (character != nullptr && character->isAlive()) {
            cursor = (index + 1) % instances.size();
            return character;
        }
    }

    return nullptr;
}

const Team* BattleEngine::currentTeam() const {
    if (m_activeTeamIndex == 0) {
        return m_teamA;
    }

    return m_teamB;
}

std::size_t& BattleEngine::currentCursor() {
    return (m_activeTeamIndex == 0) ? m_nextIndexA : m_nextIndexB;
}

bool BattleEngine::prepareCurrentActor(const Team* winnerIfCurrentTeamEmpty) {
    const Team* activeTeam = currentTeam();
    if (activeTeam == nullptr) {
        m_state = BattleState::FINISHED;
        m_winnerTeam = winnerIfCurrentTeamEmpty;
        m_currentActorId = 0;
        return false;
    }

    Character* actor = findNextAliveCharacter(*activeTeam, currentCursor());
    if (actor == nullptr) {
        m_state = BattleState::FINISHED;
        m_winnerTeam = winnerIfCurrentTeamEmpty;
        m_currentActorId = 0;
        return false;
    }

    m_currentActorId = actor->getId();
    return true;
}

void BattleEngine::moveToNextTeam() {
    if (m_state != BattleState::IN_PROGRESS) {
        return;
    }

    const Team* previousTeam = currentTeam();
    m_activeTeamIndex = (m_activeTeamIndex == 0) ? 1 : 0;

    prepareCurrentActor(previousTeam);
}

} // namespace TurnBasedGame
