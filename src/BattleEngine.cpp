/**
 * @file BattleEngine.cpp
 * @module Battle
 * @brief Implementation of the battle flow controller.
 */

#include "BattleEngine.h"

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
    if (m_state != BattleState::IN_PROGRESS || m_roster == nullptr) {
        return nullptr;
    }

    return resolveCharacter(m_currentActorId);
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

void BattleEngine::resetBattle() {
    // Xóa toàn bộ state tạm của battle, nhưng không đụng vào dữ liệu gốc trong roster.
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
        || actorId == targetId
        || m_roster == nullptr
        || m_teamA == nullptr
        || m_teamB == nullptr) {
        return false;
    }

    Character* actor = resolveCharacter(actorId);
    Character* target = resolveCharacter(targetId);

    if (actor == nullptr
        || target == nullptr
        || !actor->isAlive()
        || !target->isAlive()) {
        return false;
    }

    const Team* activeTeam = currentTeam();
    const Team* enemyTeam = (activeTeam == m_teamA) ? m_teamB : m_teamA;

    if (activeTeam == nullptr
        || enemyTeam == nullptr
        || !teamContainsCharacter(*activeTeam, actorId)
        || !teamContainsCharacter(*enemyTeam, targetId)
        || m_currentActorId != actorId) {
        return false;
    }

    actor->performAction(*target);
    ++m_roundsPlayed;

    moveToNextTeam();

    return true;
}

bool BattleEngine::startBattleInternal(const Team& teamA, const Team& teamB, CharacterRoster& roster) {
    resetBattle();

    if (!validateBattleSetup(teamA, teamB, roster)) {
        return false;
    }

    m_roster = &roster;
    m_teamA = &teamA;
    m_teamB = &teamB;

    const auto& teamAIds = teamA.getCharacterIds();
    const auto& teamBIds = teamB.getCharacterIds();

    std::vector<int> resetIds;
    resetIds.reserve(teamAIds.size() + teamBIds.size());

    // Một character có thể nằm ở cả hai team mẫu, nên gom ID duy nhất để chỉ reset một lần.
    for (int characterId : teamAIds) {
        if (std::find(resetIds.begin(), resetIds.end(), characterId) == resetIds.end()) {
            resetIds.push_back(characterId);
        }
    }

    for (int characterId : teamBIds) {
        if (std::find(resetIds.begin(), resetIds.end(), characterId) == resetIds.end()) {
            resetIds.push_back(characterId);
        }
    }

    for (int characterId : resetIds) {
        Character* character = resolveCharacter(characterId);
        if (character == nullptr) {
            resetBattle();
            return false;
        }

        character->resetBattleState();
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
    // Không check m_state ở đây vì startBattleInternal đã reset trước khi gọi hàm này.
    if (&teamA == &teamB
        || teamA.getId() == teamB.getId()
        || roster.empty()
        || teamA.getCharacterIds().empty()
        || teamB.getCharacterIds().empty()) {
        return false;
    }

    // Cho phép hai team dùng chung character ID (một nhân vật nằm trong nhiều team mẫu)
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

Character* BattleEngine::resolveCharacter(int characterId) {
    if (m_roster == nullptr) {
        return nullptr;
    }

    return m_roster->findById(characterId);
}

const Character* BattleEngine::resolveCharacter(int characterId) const {
    if (m_roster == nullptr) {
        return nullptr;
    }

    return m_roster->findById(characterId);
}

Character* BattleEngine::findNextAliveCharacter(const Team& team,
                                                std::size_t& cursor) {
    const auto& ids = team.getCharacterIds();
    if (ids.empty()) {
        return nullptr;
    }

    // Duyệt tối đa ids.size() phần tử để tránh vòng lặp vô hạn khi tất cả đã chết.
    const std::size_t startCursor = cursor % ids.size();
    std::size_t checked = 0;

    while (checked < ids.size()) {
        std::size_t index = (startCursor + checked) % ids.size();
        ++checked;

        Character* character = resolveCharacter(ids[index]);
        if (character != nullptr && character->isAlive()) {
            // cursor trỏ tới phần tử KẾ TIẾP sau nhân vật vừa chọn, đảm bảo round sau bắt đầu từ đúng vị trí tiếp theo.
            cursor = (index + 1) % ids.size();
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

    // prepareCurrentActor nhận previousTeam làm winner nếu team kế tiếp hết người sống.
    // Không cần gán m_winnerTeam ở đây vì prepareCurrentActor đã xử lý đúng.
    prepareCurrentActor(previousTeam);
}

} // namespace TurnBasedGame
