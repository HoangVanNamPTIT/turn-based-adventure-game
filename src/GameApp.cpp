/**
 * @file GameApp.cpp
 * @module Application
 * @brief Application layer implementation for console interaction and module orchestration.
 */

#include "BattleEngine.h"
#include "CharacterRoster.h"
#include "DataFileManager.h"
#include "GameApp.h"
#include "Mage.h"
#include "Menu.h"
#include "TeamManager.h"
#include "Warrior.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <memory>
#include <sstream>
#include <unordered_set>
#include <vector>

namespace TurnBasedGame {

namespace {

std::string trimString(const std::string& text) {
    const std::size_t first = text.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return "";
    }
    const std::size_t last = text.find_last_not_of(" \t\r\n");
    return text.substr(first, last - first + 1);
}

std::string toLowerString(const std::string& value) {
    std::string normalized = value;
    std::transform(normalized.begin(), normalized.end(), normalized.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return normalized;
}

} // namespace

GameApp::GameApp(std::istream& input, std::ostream& output)
    : m_menu(input, output) {
}

int GameApp::run() {
    initialize();

    while (true) {
        int choice = m_menu.showMainMenu();
        switch (choice) {
            case 1:
                runCharacterMenu();
                break;
            case 2:
                runTeamMenu();
                break;
            case 3:
                runBattleMenu();
                break;
            case 4:
                handleShowTeamStatistics();
                break;
            case 5:
                if (saveData()) {
                    m_menu.showGoodbye();
                    return 0;
                }
                break;
            default:
                break;
        }
    }
}

bool GameApp::initialize() {
    loadCharacters();
    loadTeams();
    return true;
}

bool GameApp::checkBattleLock() {
    if (m_battleEngine.getState() == BattleState::IN_PROGRESS) {
        m_menu.showError("Trận đấu đang diễn ra. Vui lòng kết thúc hoặc reset trận đấu trước khi thực hiện thao tác này.");
        return true;
    }
    return false;
}

bool GameApp::loadCharacters() {
    std::vector<std::shared_ptr<Character>> loadedCharacters;
    if (!DataFileManager::loadCharacters(m_charactersFilePath,
                                         loadedCharacters)) {
        m_menu.showInfo(
            "Không tìm thấy file nhân vật. Bắt đầu với danh sách nhân vật trống.");
        return false;
    }

    for (const auto& character : loadedCharacters) {
        if (character == nullptr) {
            continue;
        }

        if (const Warrior* warrior = dynamic_cast<const Warrior*>(character.get())) {
            if (!m_roster.addWarrior(warrior->getId(),
                                     warrior->getName(),
                                     warrior->getMaxHp(),
                                     warrior->getAttackPower())) {
                std::ostringstream message;
                message << "Không thêm được chiến binh từ file với ID "
                        << warrior->getId() << ".";
                m_menu.showError(message.str());
            }
        } else if (const Mage* mage = dynamic_cast<const Mage*>(character.get())) {
            if (!m_roster.addMage(mage->getId(),
                                  mage->getName(),
                                  mage->getMaxHp(),
                                  mage->getMaxMana(),
                                  mage->getSpellDamage(),
                                  mage->getManaCost(),
                                  mage->getFallbackDamage())) {
                std::ostringstream message;
                message << "Không thêm được pháp sư từ file với ID "
                        << mage->getId() << ".";
                m_menu.showError(message.str());
            }
        } else {
            m_menu.showError("Bỏ qua nhân vật không hợp lệ trong file nhân vật.");
        }
    }

    return true;
}

void GameApp::loadTeams() {
    if (!m_teamManager.load(m_teamsFilePath, m_roster)) {
        m_menu.showInfo(
            "Không tìm thấy file đội hình hoặc không thể đọc file. Bắt đầu với danh sách đội trống.");
    }
}

bool GameApp::saveCharacters() const {
    return DataFileManager::saveCharacters(m_charactersFilePath, m_roster);
}

bool GameApp::saveTeams() const {
    return m_teamManager.save(m_teamsFilePath);
}

bool GameApp::saveData() {
    const bool savedCharacters = saveCharacters();
    const bool savedTeams = saveTeams();

    if (!savedCharacters) {
        m_menu.showError("Lưu danh sách nhân vật thất bại.");
    }
    if (!savedTeams) {
        m_menu.showError("Lưu đội hình thất bại.");
    }

    return savedCharacters && savedTeams;
}

void GameApp::runCharacterMenu() {
    while (true) {
        int choice = m_menu.showCharacterMenu();
        switch (choice) {
            case 1:
                handleListCharacters();
                break;
            case 2:
                handleAddCharacterMenu();
                break;
            case 3:
                handleUpdateCharacter();
                break;
            case 4:
                handleDeleteCharacter();
                break;
            case 5:
                handleSearchCharacterById();
                break;
            case 6:
                handleSearchCharacterByName();
                break;
            case 0:
                return;
            default:
                break;
        }
    }
}

void GameApp::handleAddCharacterMenu() {
    while (true) {
        int choice = m_menu.showAddCharacterMenu();
        switch (choice) {
            case 1:
                handleAddWarrior();
                break;
            case 2:
                handleAddMage();
                break;
            case 0:
                return;
            default:
                break;
        }
    }
}

void GameApp::runTeamMenu() {
    while (true) {
        int choice = m_menu.showTeamMenu();
        switch (choice) {
            case 1:
                handleListTeams();
                break;
            case 2:
                handleViewTeam();
                break;
            case 3:
                handleCreateTeam();
                break;
            case 4:
                handleRenameTeam();
                break;
            case 5:
                handleDeleteTeam();
                break;
            case 6:
                handleAddCharacterToTeam();
                break;
            case 7:
                handleRemoveCharacterFromTeam();
                break;
            case 0:
                return;
            default:
                break;
        }
    }
}

void GameApp::runBattleMenu() {
    while (true) {
        int choice = m_menu.showBattleMenu(m_battleEngine.getState());
        switch (choice) {
            case 1:
                handleStartBattle();
                break;
            case 2:
                if (m_battleEngine.getState() == BattleState::IN_PROGRESS) {
                    handleShowBattleStatus();
                }
                break;
            case 3:
                if (m_battleEngine.getState() == BattleState::IN_PROGRESS) {
                    handleContinueBattle();
                }
                break;
            case 4:
                if (m_battleEngine.getState() == BattleState::IN_PROGRESS) {
                    handleResetBattle();
                }
                break;
            case 0:
                return;
            default:
                break;
        }
    }
}

void GameApp::handleListCharacters() {
    m_menu.displayRoster(m_roster);
    m_menu.waitForEnter();
}

void GameApp::handleAddWarrior() {
    while (true) {
        int id = 0;
        std::string name;
        int maxHp = 0;
        int attackPower = 0;

        if (!m_menu.readPositiveInt("Nhập ID nhân vật: ", id)) {
            return;
        }

        if (m_roster.findById(id) != nullptr) {
            m_menu.showError("ID đã tồn tại. Nhập lại thông tin từ đầu.");
            continue;
        }

        if (!m_menu.readName("Nhập tên nhân vật: ", name)
            || !m_menu.readPositiveInt("Nhập HP tối đa: ", maxHp)
            || !m_menu.readPositiveInt("Nhập lực tấn công: ", attackPower)) {
            return;
        }

        if (m_roster.addWarrior(id, name, maxHp, attackPower)) {
            const Character* added = m_roster.findById(id);
            if (added != nullptr) {
                m_menu.displayCharacters({added});
            }
            if (!saveCharacters()) {
                m_menu.showError("Lưu dữ liệu nhân vật vào file thất bại.");
                return;
            }
            m_menu.showSuccess("Thêm chiến binh thành công.");
            return;
        } else {
            m_menu.showError("Thêm chiến binh thất bại. Kiểm tra ID hoặc dữ liệu.");
            return;
        }
    }
}

void GameApp::handleAddMage() {
    while (true) {
        int id = 0;
        std::string name;
        int maxHp = 0;
        int maxMana = 0;
        int spellDamage = 0;
        int manaCost = 0;
        int fallbackDamage = 0;

        if (!m_menu.readPositiveInt("Nhập ID nhân vật: ", id)) {
            return;
        }

        if (m_roster.findById(id) != nullptr) {
            m_menu.showError("ID đã tồn tại. Nhập lại thông tin từ đầu.");
            continue;
        }

        if (!m_menu.readName("Nhập tên nhân vật: ", name)
            || !m_menu.readPositiveInt("Nhập HP tối đa: ", maxHp)
            || !m_menu.readPositiveInt("Nhập mana tối đa: ", maxMana)
            || !m_menu.readPositiveInt("Nhập sát thương phép: ", spellDamage)
            || !m_menu.readPositiveInt("Nhập chi phí mana: ", manaCost)
            || !m_menu.readPositiveInt("Nhập sát thương dự phòng: ", fallbackDamage)) {
            return;
        }

        if (m_roster.addMage(id,
                             name,
                             maxHp,
                             maxMana,
                             spellDamage,
                             manaCost,
                             fallbackDamage)) {
            const Character* added = m_roster.findById(id);
            if (added != nullptr) {
                m_menu.displayCharacters({added});
            }
            if (!saveCharacters()) {
                m_menu.showError("Lưu dữ liệu nhân vật vào file thất bại.");
                return;
            }
            m_menu.showSuccess("Thêm pháp sư thành công.");
            return;
        } else {
            m_menu.showError("Thêm pháp sư thất bại. Kiểm tra ID hoặc dữ liệu.");
            return;
        }
    }
}

void GameApp::handleUpdateCharacter() {
    if (checkBattleLock()) {
        return;
    }
    int id = 0;
    if (!m_menu.readPositiveInt("Nhập ID nhân vật cần cập nhật: ", id)) {
        return;
    }

    Character* character = m_roster.findById(id);
    if (character == nullptr) {
        m_menu.showError("Không tìm thấy nhân vật với ID đã nhập.");
        return;
    }

    // show current info line
    m_menu.displayCharacters({character});

    std::string name;
    int maxHp = 0;
    if (!m_menu.readName("Nhập tên mới: ", name)
        || !m_menu.readPositiveInt("Nhập HP tối đa mới: ", maxHp)) {
        return;
    }

    if (Warrior* warrior = dynamic_cast<Warrior*>(character)) {
        int attackPower = 0;
        if (!m_menu.readPositiveInt("Nhập lực tấn công mới: ", attackPower)) {
            return;
        }

        if (m_roster.updateWarrior(id, name, maxHp, attackPower)) {
            const Character* updated = m_roster.findById(id);
            if (updated != nullptr) {
                m_menu.displayCharacters({updated});
            }
            if (!saveCharacters()) {
                m_menu.showError("Lưu dữ liệu nhân vật vào file thất bại.");
                return;
            }
            m_menu.showSuccess("Cập nhật chiến binh thành công cho ID " + std::to_string(id) + ".");
        } else {
            m_menu.showError("Cập nhật chiến binh thất bại.");
        }
        return;
    }

    if (Mage* mage = dynamic_cast<Mage*>(character)) {
        int maxMana = 0;
        int spellDamage = 0;
        int manaCost = 0;
        int fallbackDamage = 0;

        if (!m_menu.readPositiveInt("Nhập mana tối đa mới: ", maxMana)
            || !m_menu.readPositiveInt("Nhập sát thương phép mới: ", spellDamage)
            || !m_menu.readPositiveInt("Nhập chi phí mana mới: ", manaCost)
            || !m_menu.readPositiveInt("Nhập sát thương dự phòng mới: ", fallbackDamage)) {
            return;
        }

        if (m_roster.updateMage(id,
                                name,
                                maxHp,
                                maxMana,
                                spellDamage,
                                manaCost,
                                fallbackDamage)) {
            const Character* updated = m_roster.findById(id);
            if (updated != nullptr) {
                m_menu.displayCharacters({updated});
            }
            if (!saveCharacters()) {
                m_menu.showError("Lưu dữ liệu nhân vật vào file thất bại.");
                return;
            }
            m_menu.showSuccess("Cập nhật pháp sư thành công cho ID " + std::to_string(id) + ".");
        } else {
            m_menu.showError("Cập nhật pháp sư thất bại.");
        }
        return;
    }

    m_menu.showError("Không thể cập nhật nhân vật này. Kiểu nhân vật không hợp lệ.");
}

void GameApp::handleDeleteCharacter() {
    if (checkBattleLock()) {
        return;
    }
    int id = 0;
    if (!m_menu.readPositiveInt("Nhập ID nhân vật cần xóa: ", id)) {
        return;
    }

    if (!m_roster.removeById(id)) {
        m_menu.showError("Xóa nhân vật thất bại. Kiểm tra ID đã nhập.");
        return;
    }

    m_teamManager.removeCharacterFromAllTeams(id);
    if (!saveData()) {
        m_menu.showError("Lưu dữ liệu vào file thất bại.");
        return;
    }
    m_menu.showSuccess("Nhân vật đã được xóa và loại khỏi các đội hình.");
}

void GameApp::handleSearchCharacterById() {
    int id = 0;
    if (!m_menu.readPositiveInt("Nhập ID nhân vật cần tìm: ", id)) {
        return;
    }

    const Character* character = m_roster.findById(id);
    if (character == nullptr) {
        m_menu.showInfo("Không tìm thấy nhân vật với ID đã nhập.");
        return;
    }

    m_menu.displayCharacters({character});
    m_menu.waitForEnter();
}

void GameApp::handleSearchCharacterByName() {
    std::string keyword;
    if (!m_menu.readRequiredText("Nhập tên hoặc chuỗi tìm kiếm: ", keyword)) {
        return;
    }

    const std::vector<const Character*> matches =
        m_roster.findByNameSubstring(keyword);
    if (matches.empty()) {
        m_menu.showInfo("Không tìm thấy nhân vật phù hợp.");
        return;
    }

    m_menu.displayCharacters(matches);
    m_menu.waitForEnter();
}

void GameApp::handleListTeams() {
    m_menu.displayTeams(m_teamManager);
    m_menu.waitForEnter();
}

void GameApp::handleViewTeam() {
    int teamId = 0;
    if (!m_menu.readPositiveInt("Nhập ID đội cần xem: ", teamId)) {
        return;
    }

    const Team* team = m_teamManager.getTeamById(teamId);
    if (team == nullptr) {
        m_menu.showError("Không tìm thấy đội với ID đã nhập.");
        return;
    }

    m_menu.displayTeam(*team, m_roster);
    m_menu.waitForEnter();
}

void GameApp::handleCreateTeam() {
    int teamId = 0;
    std::string teamName;

    while (true) {
        if (!m_menu.readPositiveInt("Nhập ID đội mới: ", teamId)) {
            return;
        }

        if (m_teamManager.teamExists(teamId)) {
            m_menu.showError("ID đội đã tồn tại. Vui lòng nhập lại ID đội.");
            continue;
        }

        break;
    }

    while (true) {
        if (!m_menu.readName("Nhập tên đội mới: ", teamName)) {
            return;
        }

        if (teamName.empty()) {
            m_menu.showError("Vui lòng nhập tên đội hợp lệ.");
            continue;
        }

        if (m_teamManager.teamNameExists(teamName)) {
            m_menu.showError("Tên đội đã được sử dụng. Vui lòng nhập lại tên đội.");
            continue;
        }

        break;
    }

    if (m_teamManager.createTeam(teamId, teamName)) {
        if (!saveTeams()) {
            m_menu.showError("Lưu dữ liệu đội hình vào file thất bại.");
            return;
        }
        m_menu.showSuccess("Đội hình đã được tạo thành công.");
        const Team* team = m_teamManager.getTeamById(teamId);
        if (team != nullptr) {
            m_menu.showTeamInfo(*team);
        }
    } else {
        m_menu.showError("Tạo đội hình thất bại. Kiểm tra ID hoặc tên đội.");
    }
}

void GameApp::handleRenameTeam() {
    if (checkBattleLock()) {
        return;
    }
    int teamId = 0;
    std::string newName;

    Team* team = nullptr;
    while (true) {
        if (!m_menu.readPositiveInt("Nhập ID đội cần đổi tên: ", teamId)) {
            return;
        }

        team = m_teamManager.getTeamById(teamId);
        if (team == nullptr) {
            m_menu.showError("ID đội không tồn tại. Vui lòng nhập lại ID đội.");
            continue;
        }

        m_menu.showTeamInfo(*team);
        break;
    }

    while (true) {
        if (!m_menu.readName("Nhập tên đội mới: ", newName)) {
            return;
        }

        if (newName.empty()) {
            m_menu.showError("Tên đội không hợp lệ. Vui lòng nhập lại.");
            continue;
        }

        if (m_teamManager.isNameUsedByAnotherTeam(newName, teamId)) {
            m_menu.showError("Tên đội đã được sử dụng. Vui lòng nhập lại tên đội.");
            continue;
        }

        break;
    }

    if (m_teamManager.renameTeam(teamId, newName)) {
        if (!saveTeams()) {
            m_menu.showError("Lưu dữ liệu đội hình vào file thất bại.");
            return;
        }
        m_menu.showSuccess("Đội hình đã được đổi tên thành công.");
        const Team* renamedTeam = m_teamManager.getTeamById(teamId);
        if (renamedTeam != nullptr) {
            m_menu.showTeamInfo(*renamedTeam);
        }
    } else {
        m_menu.showError("Đổi tên đội hình thất bại. Kiểm tra ID hoặc tên mới.");
    }
}

void GameApp::handleDeleteTeam() {
    if (checkBattleLock()) {
        return;
    }
    int teamId = 0;
    if (!m_menu.readPositiveInt("Nhập ID đội cần xóa: ", teamId)) {
        return;
    }

    if (m_teamManager.deleteTeam(teamId)) {
        if (!saveTeams()) {
            m_menu.showError("Lưu dữ liệu đội hình vào file thất bại.");
            return;
        }
        m_menu.showSuccess("Đội hình đã được xóa thành công.");
    } else {
        m_menu.showError("Xóa đội hình thất bại. Kiểm tra ID đã nhập.");
    }
}

void GameApp::handleAddCharacterToTeam() {
    if (checkBattleLock()) {
        return;
    }
    int teamId = 0;
    while (true) {
        if (!m_menu.readPositiveInt("Nhập ID đội: ", teamId)) {
            return;
        }

        if (!m_teamManager.teamExists(teamId)) {
            m_menu.showError("ID đội không tồn tại. Vui lòng nhập lại ID đội.");
            continue;
        }

        break;
    }

    const Team* team = m_teamManager.getTeamById(teamId);
    if (team == nullptr) {
        m_menu.showError("Đội hình không tồn tại.");
        return;
    }

    m_menu.showTeamInfo(*team);
    while (true) {
        std::string line;
        if (!m_menu.readOptionalLine("Nhập ID nhân vật cần thêm (Enter để kết thúc): ", line)) {
            return;
        }

        const std::string trimmed = line;
        if (trimmed.empty()) {
            return;
        }

        std::istringstream parser(trimmed);
        long long parsedId = 0;
        char extra = '\0';
        if (!(parser >> parsedId) || (parser >> extra) || parsedId <= 0) {
            m_menu.showError("ID nhân vật không hợp lệ. Vui lòng nhập lại.");
            continue;
        }

        const int characterId = static_cast<int>(parsedId);
        if (team->hasCharacterId(characterId)) {
            m_menu.showError("ID nhân vật đã tồn tại trong đội. Vui lòng nhập lại.");
            continue;
        }

        const Character* character = m_roster.findById(characterId);
        if (character == nullptr) {
            m_menu.showError("Không tìm thấy nhân vật với ID đã nhập. Vui lòng nhập lại.");
            continue;
        }

        if (!m_teamManager.addCharacterToTeam(teamId, characterId, m_roster)) {
            m_menu.showError("Thêm nhân vật vào đội hình thất bại. Kiểm tra điều kiện đội.");
            continue;
        }

        if (!saveTeams()) {
            m_menu.showError("Lưu dữ liệu đội hình vào file thất bại.");
            continue;
        }

        m_menu.showSuccess("Thêm nhân vật ID " + std::to_string(characterId)
            + " (" + character->getName() + ") vào đội " + team->getName() + " thành công.");
    }
}

void GameApp::handleRemoveCharacterFromTeam() {
    if (checkBattleLock()) {
        return;
    }
    int teamId = 0;
    while (true) {
        if (!m_menu.readPositiveInt("Nhập ID đội: ", teamId)) {
            return;
        }

        if (!m_teamManager.teamExists(teamId)) {
            m_menu.showError("ID đội không tồn tại. Vui lòng nhập lại ID đội.");
            continue;
        }

        break;
    }

    const Team* team = m_teamManager.getTeamById(teamId);
    if (team == nullptr) {
        m_menu.showError("Đội hình không tồn tại.");
        return;
    }

    m_menu.showTeamInfo(*team);
    while (true) {
        std::string line;
        if (!m_menu.readOptionalLine("Nhập ID nhân vật cần xóa (Enter để kết thúc): ", line)) {
            return;
        }

        const std::string trimmed = line;
        if (trimmed.empty()) {
            return;
        }

        std::istringstream parser(trimmed);
        long long parsedId = 0;
        char extra = '\0';
        if (!(parser >> parsedId) || (parser >> extra) || parsedId <= 0) {
            m_menu.showError("ID nhân vật không hợp lệ. Vui lòng nhập lại.");
            continue;
        }

        const int characterId = static_cast<int>(parsedId);
        if (!team->hasCharacterId(characterId)) {
            m_menu.showError("ID nhân vật không thuộc đội này. Vui lòng nhập lại.");
            continue;
        }

        const Character* character = m_roster.findById(characterId);
        if (character == nullptr) {
            m_menu.showError("Không tìm thấy nhân vật với ID đã nhập. Vui lòng nhập lại.");
            continue;
        }

        if (!m_teamManager.removeCharacterFromTeam(teamId, characterId)) {
            m_menu.showError("Xóa nhân vật khỏi đội hình thất bại. Vui lòng kiểm tra lại.");
            continue;
        }

        if (!saveTeams()) {
            m_menu.showError("Lưu dữ liệu đội hình vào file thất bại.");
            continue;
        }

        m_menu.showSuccess("Xóa nhân vật ID " + std::to_string(characterId)
            + " (" + character->getName() + ") ra khỏi đội " + team->getName() + " thành công.");
    }
}

void GameApp::handleStartBattle() {
    if (m_teamManager.getAllTeams().empty()) {
        m_menu.showError("Chưa có đội hình để bắt đầu trận đấu.");
        return;
    }

    m_menu.displayTeamSummaries(m_teamManager);

    int teamAId = 0;
    while (true) {
        if (!m_menu.readPositiveInt("Nhập ID đội A: ", teamAId)) {
            return;
        }
        if (!m_teamManager.teamExists(teamAId)) {
            m_menu.showError("ID đội không tồn tại. Vui lòng nhập lại.");
            continue;
        }
        const Team* teamA = m_teamManager.getTeamById(teamAId);
        if (teamA == nullptr) {
            m_menu.showError("Lỗi nội bộ khi tìm đội A. Vui lòng thử lại.");
            continue;
        }
        if (teamA->getCharacterIds().empty()) {
            m_menu.showError("Team ID:" + std::to_string(teamAId) + " không có nhân vật. Vui lòng chọn team khác.");
            continue;
        }
        break;
    }

    int teamBId = 0;
    while (true) {
        if (!m_menu.readPositiveInt("Nhập ID đội B: ", teamBId)) {
            return;
        }
        if (teamBId == teamAId) {
            m_menu.showError("Không thể chọn cùng một đội A và đội B. Vui lòng nhập lại.");
            continue;
        }
        if (!m_teamManager.teamExists(teamBId)) {
            m_menu.showError("ID đội không tồn tại. Vui lòng nhập lại.");
            continue;
        }

        const Team* teamA = m_teamManager.getTeamById(teamAId);
        const Team* teamB = m_teamManager.getTeamById(teamBId);
        if (teamA == nullptr || teamB == nullptr) {
            m_menu.showError("Lỗi nội bộ khi tìm đội. Vui lòng thử lại.");
            continue;
        }
        if (teamB->getCharacterIds().empty()) {
            m_menu.showError("Team B không có nhân vật. Vui lòng chọn team khác.");
            continue;
        }

        // Kiểm tra hai đội không dùng chung nhân vật
        bool hasOverlap = false;
        for (int idA : teamA->getCharacterIds()) {
            for (int idB : teamB->getCharacterIds()) {
                if (idA == idB) {
                    hasOverlap = true;
                    break;
                }
            }
            if (hasOverlap) break;
        }
        if (hasOverlap) {
            m_menu.showError("Hai đội không được dùng chung nhân vật. Vui lòng chọn đội khác.");
            continue;
        }

        break;
    }

    if (m_battleEngine.getState() == BattleState::IN_PROGRESS) {
        if (!m_menu.confirm("Trận đấu hiện tại sẽ bị xóa nếu bắt đầu trận đấu mới. Bạn có muốn tiếp tục?")) {
            return;
        }
    }

    if (!m_battleEngine.startBattle(teamAId, teamBId, m_teamManager, m_roster)) {
        m_menu.showError("Khởi tạo trận đấu thất bại. Kiểm tra số lượng và tính hợp lệ của đội.");
        return;
    }

    handleContinueBattle();
}

void GameApp::handleContinueBattle() {
    if (m_battleEngine.getState() != BattleState::IN_PROGRESS) {
        m_menu.showError("Không có trận đấu đang diễn ra.");
        return;
    }

    while (true) {
        m_menu.displayBattle(m_battleEngine, m_roster);

        if (!m_menu.confirm("Tiếp tục trận đấu?")) {
            return;
        }

        const Character* currentActor = m_battleEngine.getCurrentActor();
        if (currentActor == nullptr) {
            m_menu.showError("Không có nhân vật hiện tại để thực hiện lượt.");
            return;
        }

        const Team* activeTeam = m_battleEngine.getActiveTeam();
        const Team* enemyTeam = (activeTeam == m_battleEngine.getTeamA())
            ? m_battleEngine.getTeamB()
            : m_battleEngine.getTeamA();

        if (activeTeam == nullptr || enemyTeam == nullptr) {
            m_menu.showError("Trạng thái trận đấu không hợp lệ.");
            return;
        }

        m_menu.showInfo("Lượt hiện tại: ID Team " + std::to_string(activeTeam->getId())
            + " | " + activeTeam->getName()
            + " | ID nhân vật " + std::to_string(currentActor->getId())
            + " | " + currentActor->getName());

        int targetId = 0;
        while (true) {
            if (!m_menu.readPositiveInt("Nhập ID nhân vật đội đối phương cần tấn công: ", targetId)) {
                return;
            }

            if (!m_battleEngine.teamContainsCharacter(*enemyTeam, targetId)) {
                m_menu.showError("ID nhân vật không thuộc đội đối phương. Vui lòng nhập lại.");
                continue;
            }

            const Character* target = m_battleEngine.getBattleCharacter(enemyTeam, targetId);
            if (target == nullptr) {
                m_menu.showError("Nhân vật mục tiêu không tồn tại trong trận đấu.");
                continue;
            }

            if (!target->isAlive()) {
                m_menu.showError("Nhân vật mục tiêu đã chết. Chọn mục tiêu khác.");
                continue;
            }

            break;
        }

        const Team* attackingTeam = activeTeam;
        int actorId = currentActor->getId();
        const Character* targetCharacter = m_battleEngine.getBattleCharacter(enemyTeam, targetId);
        if (targetCharacter == nullptr) {
            m_menu.showError("Đã xảy ra lỗi khi tìm mục tiêu.");
            return;
        }

        std::string actionType;
        int manaSpent = -1;
        if (const Warrior* warrior = dynamic_cast<const Warrior*>(currentActor)) {
            actionType = "Attack Power";
            manaSpent = -1;
        } else if (const Mage* mage = dynamic_cast<const Mage*>(currentActor)) {
            if (mage->getCurrentMana() >= mage->getManaCost()) {
                actionType = "Spell Damage";
                manaSpent = mage->getManaCost();
            } else {
                actionType = "Fallback Damage";
                manaSpent = 0;
            }
        } else {
            actionType = "Attack Power";
            manaSpent = -1;
        }

        const int targetHpBefore = targetCharacter->getCurrentHp();

        if (!m_battleEngine.performAction(actorId, targetId)) {
            m_menu.showError("Hành động không thành công. Vui lòng kiểm tra lại lượt và mục tiêu.");
            return;
        }

        const int targetHpAfter = targetCharacter->getCurrentHp();
        const int damageDealt = (targetHpBefore > targetHpAfter) ? (targetHpBefore - targetHpAfter) : 0;

        const std::string activeTeamTag = (attackingTeam == m_battleEngine.getTeamA()) ? "Đội A" : "Đội B";
        const std::string enemyTeamTag = (enemyTeam == m_battleEngine.getTeamA()) ? "Đội A" : "Đội B";
        const std::size_t turnNumber = m_battleEngine.getRoundsPlayed();

        std::ostringstream logLine1;
        logLine1 << "[LƯỢT " << turnNumber << "] "
                 << activeTeamTag << " (" << attackingTeam->getName() << " - ID " << attackingTeam->getId() << ") "
                 << "ID " << currentActor->getId() << " " << currentActor->getName()
                 << " TẤN CÔNG --> "
                 << enemyTeamTag << " (" << enemyTeam->getName() << " - ID " << enemyTeam->getId() << ") "
                 << "ID " << targetCharacter->getId() << " " << targetCharacter->getName();

        std::ostringstream logLine2;
        logLine2 << "Loại hành động: " << actionType
                 << " | Sát thương gây ra: " << damageDealt;
        if (manaSpent >= 0) {
            logLine2 << " | Mana tiêu tốn: " << manaSpent;
        }

        std::ostringstream logLine3;
        logLine3 << "Trạng thái mục tiêu sau hành động: "
                 << enemyTeamTag << " ID " << targetCharacter->getId() << " " << targetCharacter->getName()
                 << " | HP: " << targetCharacter->getCurrentHp() << "/" << targetCharacter->getMaxHp()
                 << " (" << (targetCharacter->isAlive() ? "Còn sống" : "Bị hạ") << ")";

        m_menu.showBattleLog(logLine1.str());
        m_menu.showBattleLog(logLine2.str());
        m_menu.showBattleLog(logLine3.str());

        if (m_battleEngine.getState() != BattleState::IN_PROGRESS) {
            const Team* winner = m_battleEngine.getWinnerTeam();
            if (winner != nullptr) {
                const Team* loser = (winner == m_battleEngine.getTeamA()) ? m_battleEngine.getTeamB() : m_battleEngine.getTeamA();
                if (loser != nullptr) {
                    DataFileManager::recordBattleResult(m_teamStatsFilePath, *winner, *loser);
                }
            }
            m_menu.displayBattleResult(m_battleEngine, m_roster);
            m_menu.waitForZeroToReturn("Nhấn 0 để quay về menu trận đấu: ");
            return;
        }
    }
}

void GameApp::handleShowBattleStatus() {
    if (m_battleEngine.getState() != BattleState::IN_PROGRESS) {
        m_menu.showError("Không có trận đấu đang diễn ra.");
        return;
    }

    m_menu.displayBattle(m_battleEngine, m_roster);
    m_menu.waitForEnter();
}

void GameApp::handleResetBattle() {
    if (m_battleEngine.getState() != BattleState::IN_PROGRESS) {
        m_menu.showError("Không có trận đấu đang diễn ra để xóa.");
        return;
    }

    m_battleEngine.resetBattle();
    m_menu.showSuccess("Trận đấu hiện tại đã được xóa.");
}

void GameApp::handleShowTeamStatistics() {
    std::vector<TeamRecord> stats;
    DataFileManager::loadTeamStats(m_teamStatsFilePath, stats);

    std::sort(stats.begin(), stats.end(), [](const TeamRecord& a, const TeamRecord& b) {
        if (a.wins != b.wins) {
            return a.wins > b.wins;
        }
        return a.losses < b.losses;
    });

    m_menu.displayTeamStatistics(stats);
    m_menu.waitForZeroToReturn("Nhấn 0 để quay lại menu chính: ");
}


} // namespace TurnBasedGame
