/**
 * @file Menu.cpp
 * @module Application
 * @brief Triển khai giao diện console tiếng Việt và xử lý dữ liệu nhập.
 */

#include "Menu.h"

#include "BattleEngine.h"
#include "BattleState.h"
#include "Character.h"
#include "CharacterRoster.h"
#include "Mage.h"
#include "Team.h"
#include "TeamManager.h"
#include "Warrior.h"

#include <cctype>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

namespace TurnBasedGame {
namespace {

const std::size_t MENU_WIDTH = 68U;
const std::size_t CHARACTER_TABLE_WIDTH = 111U;
const std::size_t TEAM_TABLE_WIDTH = 57U;
const std::size_t BATTLE_TABLE_WIDTH = 82U;

const std::vector<std::size_t>& characterColumnWidths() {
    static const std::vector<std::size_t> widths = {
        5U, 11U, 19U, 10U, 11U, 10U, 8U, 8U, 10U
    };
    return widths;
}

const std::vector<std::size_t>& teamColumnWidths() {
    static const std::vector<std::size_t> widths = {8U, 28U, 14U};
    return widths;
}

const std::vector<std::size_t>& battleColumnWidths() {
    static const std::vector<std::size_t> widths = {
        5U, 20U, 11U, 11U, 11U, 11U
    };
    return widths;
}

std::string repeat(char character, std::size_t count) {
    return std::string(count, character);
}

bool isUtf8ContinuationByte(unsigned char byte) {
    return (byte & 0xC0U) == 0x80U;
}

std::size_t displayLength(const std::string& text) {
    std::size_t length = 0U;
    for (unsigned char byte : text) {
        if (!isUtf8ContinuationByte(byte)) {
            ++length;
        }
    }
    return length;
}

std::string utf8Prefix(const std::string& text,
                       std::size_t characterCount) {
    if (characterCount == 0U) {
        return "";
    }

    std::size_t charactersSeen = 0U;
    std::size_t byteIndex = 0U;
    while (byteIndex < text.size()) {
        if (!isUtf8ContinuationByte(
                static_cast<unsigned char>(text[byteIndex]))) {
            if (charactersSeen == characterCount) {
                break;
            }
            ++charactersSeen;
        }
        ++byteIndex;
    }
    return text.substr(0U, byteIndex);
}

std::string fitText(const std::string& text, std::size_t width) {
    const std::size_t length = displayLength(text);
    if (length <= width) {
        return text + repeat(' ', width - length);
    }

    if (width <= 3U) {
        return utf8Prefix(text, width);
    }
    return utf8Prefix(text, width - 3U) + "...";
}

std::string centerText(const std::string& text, std::size_t width) {
    const std::string fitted = displayLength(text) > width
        ? fitText(text, width)
        : text;
    const std::size_t length = displayLength(fitted);
    if (length >= width) {
        return fitted;
    }

    const std::size_t remaining = width - length;
    const std::size_t leftPadding = remaining / 2U;
    const std::size_t rightPadding = remaining - leftPadding;
    return repeat(' ', leftPadding) + fitted + repeat(' ', rightPadding);
}

void printFullBorder(std::ostream& output,
                     std::size_t width,
                     char fillCharacter) {
    output << '+' << repeat(fillCharacter, width - 2U) << "+\n";
}

void printBoxLine(std::ostream& output,
                  const std::string& text,
                  std::size_t width) {
    output << "| " << fitText(text, width - 4U) << " |\n";
}

void printCenteredBoxLine(std::ostream& output,
                          const std::string& text,
                          std::size_t width) {
    output << "| " << centerText(text, width - 4U) << " |\n";
}

void printMenuScreen(std::ostream& output,
                     const std::string& title,
                     const std::vector<std::string>& options) {
    output << '\n';
    printFullBorder(output, MENU_WIDTH, '=');
    printCenteredBoxLine(output, "TURN-BASED ADVENTURE GAME", MENU_WIDTH);
    printCenteredBoxLine(output, title, MENU_WIDTH);
    printFullBorder(output, MENU_WIDTH, '-');

    for (const std::string& option : options) {
        printBoxLine(output, "  " + option, MENU_WIDTH);
    }

    printFullBorder(output, MENU_WIDTH, '=');
}

void printSectionTitle(std::ostream& output,
                       const std::string& title,
                       std::size_t width) {
    output << '\n';
    printFullBorder(output, width, '=');
    printCenteredBoxLine(output, title, width);
    printFullBorder(output, width, '=');
}

void printThreeBlankLines(std::ostream& output) {
    output << "\n\n\n";
}

void printCenteredMessage(std::ostream& output,
                          const std::string& message,
                          std::size_t width) {
    printFullBorder(output, width, '-');
    printCenteredBoxLine(output, message, width);
    printFullBorder(output, width, '-');
}

void printTableBorder(std::ostream& output,
                      const std::vector<std::size_t>& widths) {
    output << '+';
    for (std::size_t width : widths) {
        output << repeat('-', width + 1U) << '+';
    }
    output << '\n';
}

void printTableRow(std::ostream& output,
                   const std::vector<std::string>& values,
                   const std::vector<std::size_t>& widths) {
    for (std::size_t index = 0U; index < widths.size(); ++index) {
        const std::string value = index < values.size()
            ? values[index]
            : "";
        output << "| " << fitText(value, widths[index]);
    }
    output << "|\n";
}

std::string numberToString(int value) {
    std::ostringstream builder;
    builder << value;
    return builder.str();
}

std::string numberToString(std::size_t value) {
    std::ostringstream builder;
    builder << value;
    return builder.str();
}

std::string ratioToString(int currentValue, int maxValue) {
    std::ostringstream builder;
    builder << currentValue << "/" << maxValue;
    return builder.str();
}

std::string characterTypeToVietnamese(const Character& character) {
    if (dynamic_cast<const Warrior*>(&character) != nullptr) {
        return "Chiến binh";
    }
    if (dynamic_cast<const Mage*>(&character) != nullptr) {
        return "Pháp sư";
    }
    return character.getType();
}

std::string battleStateToVietnamese(BattleState state) {
    switch (state) {
        case BattleState::READY:
            return "Sẵn sàng";
        case BattleState::IN_PROGRESS:
            return "Đang diễn ra";
        case BattleState::FINISHED:
            return "Đã kết thúc";
        default:
            return "Không xác định";
    }
}

void printCharacterTableHeader(std::ostream& output) {
    const std::vector<std::size_t>& widths = characterColumnWidths();
    printTableBorder(output, widths);
    printTableRow(output,
                  {"ID", "Loại", "Tên nhân vật", "HP", "Sát thương",
                   "Mana", "Phép", "Tốn MP", "Dự phòng"},
                  widths);
    printTableBorder(output, widths);
}

void printCharacterRow(std::ostream& output,
                       const Character& character) {
    std::string attackPower = "-";
    std::string mana = "-";
    std::string spellDamage = "-";
    std::string manaCost = "-";
    std::string fallbackDamage = "-";

    const Warrior* warrior = dynamic_cast<const Warrior*>(&character);
    if (warrior != nullptr) {
        attackPower = numberToString(warrior->getAttackPower());
    }

    const Mage* mage = dynamic_cast<const Mage*>(&character);
    if (mage != nullptr) {
        mana = ratioToString(mage->getCurrentMana(), mage->getMaxMana());
        spellDamage = numberToString(mage->getSpellDamage());
        manaCost = numberToString(mage->getManaCost());
        fallbackDamage = numberToString(mage->getFallbackDamage());
    }

    printTableRow(output,
                  {numberToString(character.getId()),
                   characterTypeToVietnamese(character),
                   character.getName(),
                   ratioToString(character.getCurrentHp(),
                                 character.getMaxHp()),
                   attackPower,
                   mana,
                   spellDamage,
                   manaCost,
                   fallbackDamage},
                  characterColumnWidths());
}

void printBattleTableHeader(std::ostream& output) {
    const std::vector<std::size_t>& widths = battleColumnWidths();
    printTableBorder(output, widths);
    printTableRow(output,
                  {"ID", "Tên nhân vật", "Loại", "HP", "Mana",
                   "Trạng thái"},
                  widths);
    printTableBorder(output, widths);
}

void printBattleCharacterRow(std::ostream& output,
                             const Character& character) {
    const Mage* mage = dynamic_cast<const Mage*>(&character);
    const std::string mana = mage == nullptr
        ? "-"
        : ratioToString(mage->getCurrentMana(), mage->getMaxMana());

    printTableRow(output,
                  {numberToString(character.getId()),
                   character.getName(),
                   characterTypeToVietnamese(character),
                   ratioToString(character.getCurrentHp(),
                                 character.getMaxHp()),
                   mana,
                   character.isAlive() ? "Còn sống" : "Bị hạ"},
                  battleColumnWidths());
}

void printBattleTeam(std::ostream& output,
                     const std::string& label,
                     const Team& team,
                     const BattleEngine& battle,
                     const CharacterRoster& roster) {
    std::ostringstream title;
    title << label << ": " << team.getName()
          << " (ID " << team.getId() << ")";

    printSectionTitle(output, title.str(), BATTLE_TABLE_WIDTH);
    printThreeBlankLines(output);
    printBattleTableHeader(output);

    for (int characterId : team.getCharacterIds()) {
        const Character* character = battle.getBattleCharacter(&team, characterId);
        if (character == nullptr) {
            character = roster.findById(characterId);
        }
        if (character == nullptr) {
            printTableRow(output,
                          {numberToString(characterId), "Không tồn tại", "-",
                           "-", "-", "Lỗi"},
                          battleColumnWidths());
        } else {
            printBattleCharacterRow(output, *character);
        }
    }

    printTableBorder(output, battleColumnWidths());
}

} // namespace

Menu::Menu()
    : m_input(std::cin), m_output(std::cout) {
}

Menu::Menu(std::istream& input, std::ostream& output)
    : m_input(input), m_output(output) {
}


int Menu::showMainMenu() {
    printMenuScreen(m_output,
                    "MENU CHÍNH",
                    {"1. Quản lý danh sách nhân vật",
                     "2. Quản lý đội hình",
                     "3. Chiến đấu",
                     "4. Lưu và thoát"});

    int choice = 0;
    return readInt("Nhập lựa chọn của bạn: ", 1, 4, choice)
        ? choice
        : 1;
}


int Menu::showCharacterMenu() {
    printMenuScreen(m_output,
                    "QUẢN LÝ NHÂN VẬT",
                    {"1. Hiển thị tất cả nhân vật",
                     "2. Thêm nhân vật",
                     "3. Cập nhật nhân vật",
                     "4. Xóa nhân vật",
                     "5. Tìm nhân vật theo ID",
                     "6. Tìm nhân vật theo tên",
                     "0. Quay lại menu chính"});

    int choice = 0;
    return readInt("Nhập lựa chọn của bạn: ", 0, 6, choice)
        ? choice
        : 0;
}


int Menu::showAddCharacterMenu() {
    printMenuScreen(m_output,
                    "THÊM NHÂN VẬT",
                    {"1. Thêm Chiến binh (Warrior)",
                     "2. Thêm Pháp sư (Mage)",
                     "0. Quay lại menu quản lý nhân vật"});

    int choice = 0;
    return readInt("Nhập lựa chọn của bạn: ", 0, 2, choice)
        ? choice
        : 0;
}


int Menu::showTeamMenu() {
    printMenuScreen(m_output,
                    "QUẢN LÝ ĐỘI HÌNH",
                    {"1. Hiển thị tất cả đội hình",
                     "2. Xem chi tiết một đội hình",
                     "3. Tạo đội hình mới",
                     "4. Đổi tên đội hình",
                     "5. Xóa đội hình",
                     "6. Thêm nhân vật vào đội hình",
                     "7. Xóa nhân vật khỏi đội hình",
                     "0. Quay lại menu chính"});

    int choice = 0;
    return readInt("Nhập lựa chọn của bạn: ", 0, 7, choice)
        ? choice
        : 0;
}


int Menu::showBattleMenu(BattleState state) {
    std::vector<std::string> options;
    options.emplace_back("1. Bắt đầu trận đấu mới");
    if (state == BattleState::IN_PROGRESS) {
        options.emplace_back("2. Xem trạng thái trận đấu");
        options.emplace_back("3. Tiếp tục trận đấu");
        options.emplace_back("4. Xóa trận đấu hiện tại");
    }
    options.emplace_back("0. Quay lại menu chính");

    printMenuScreen(m_output,
                    "CHIẾN ĐẤU",
                    options);

    int maxChoice = state == BattleState::IN_PROGRESS ? 4 : 1;
    int choice = 0;
    return readInt("Nhập lựa chọn của bạn: ", 0, maxChoice, choice)
        ? choice
        : 0;
}


bool Menu::readLine(const std::string& prompt, std::string& value) {
    m_output << prompt;
    if (!std::getline(m_input, value)) {
        m_output << '\n';
        showInfo("Đầu vào đã đóng. Quay lại menu trước.");
        return false;
    }
    return true;
}


bool Menu::readInt(const std::string& prompt,
                   int minValue,
                   int maxValue,
                   int& value) {
    if (minValue > maxValue) {
        showError("Khoảng giá trị nhập không hợp lệ.");
        return false;
    }

    while (true) {
        std::string line;
        if (!readLine(prompt, line)) {
            return false;
        }

        std::istringstream parser(line);
        long long parsedValue = 0;
        char extraCharacter = '\0';

        if (!(parser >> parsedValue) || (parser >> extraCharacter)) {
            showError("Vui lòng nhập một số nguyên hợp lệ.");
            continue;
        }

        if (parsedValue < minValue || parsedValue > maxValue) {
            std::ostringstream message;
            message << "Giá trị phải nằm trong khoảng từ "
                    << minValue << " đến " << maxValue << ".";
            showError(message.str());
            continue;
        }

        value = static_cast<int>(parsedValue);
        return true;
    }
}


bool Menu::readPositiveInt(const std::string& prompt, int& value) {
    return readInt(prompt, 1, std::numeric_limits<int>::max(), value);
}


bool Menu::readRequiredText(const std::string& prompt,
                            std::string& value) {
    while (true) {
        std::string line;
        if (!readLine(prompt, line)) {
            return false;
        }

        line = trim(line);
        if (line.empty()) {
            showError("Nội dung không được để trống.");
            continue;
        }

        value = line;
        return true;
    }
}


bool Menu::readOptionalLine(const std::string& prompt,
                            std::string& value) {
    std::string line;
    if (!readLine(prompt, line)) {
        return false;
    }
    value = trim(line);
    return true;
}


bool Menu::readName(const std::string& prompt, std::string& value) {
    while (true) {
        std::string line;
        if (!readLine(prompt, line)) {
            return false;
        }

        line = trim(line);
        if (line.empty()) {
            showError("Tên không được để trống.");
            continue;
        }

        if (line.find('|') != std::string::npos
            || line.find(',') != std::string::npos) {
            showError("Tên không được chứa ký tự '|' hoặc ','.");
            continue;
        }

        value = line;
        return true;
    }
}


bool Menu::confirm(const std::string& prompt) {
    while (true) {
        std::string answer;
        if (!readLine(prompt + " [c/k]: ", answer)) {
            return false;
        }

        answer = toLowerAscii(trim(answer));
        if (answer == "c" || answer == "co" || answer == "y"
            || answer == "yes") {
            return true;
        }

        if (answer == "k" || answer == "khong" || answer == "n"
            || answer == "no") {
            return false;
        }

        showError("Vui lòng nhập 'c' (có) hoặc 'k' (không).");
    }
}


void Menu::waitForEnter() {
    std::string ignored;
    readLine("Nhấn Enter để tiếp tục...", ignored);
}


void Menu::displayRoster(const CharacterRoster& roster) const {
    displayCharacters(roster.getAllCharacters());
}


void Menu::displayCharacters(
    const std::vector<const Character*>& characters) const {
    printSectionTitle(m_output,
                      "DANH SÁCH NHÂN VẬT",
                      CHARACTER_TABLE_WIDTH);
    printThreeBlankLines(m_output);

    if (characters.empty()) {
        printCenteredMessage(m_output,
                             "[THÔNG BÁO] Danh sách nhân vật đang trống.",
                             CHARACTER_TABLE_WIDTH);
        return;
    }

    printCharacterTableHeader(m_output);
    for (const Character* character : characters) {
        if (character != nullptr) {
            printCharacterRow(m_output, *character);
        }
    }
    printTableBorder(m_output, characterColumnWidths());
}


void Menu::displayTeams(const TeamManager& teamManager) const {
    const std::vector<Team> teams = teamManager.getAllTeams();

    printSectionTitle(m_output,
                      "DANH SÁCH ĐỘI HÌNH",
                      TEAM_TABLE_WIDTH);
    printThreeBlankLines(m_output);

    if (teams.empty()) {
        printCenteredMessage(m_output,
                             "[THÔNG BÁO] Chưa có đội hình nào.",
                             TEAM_TABLE_WIDTH);
        return;
    }

    const std::vector<std::size_t>& widths = teamColumnWidths();
    printTableBorder(m_output, widths);
    printTableRow(m_output,
                  {"ID đội", "Tên đội", "Số thành viên"},
                  widths);
    printTableBorder(m_output, widths);

    for (const Team& team : teams) {
        printTableRow(m_output,
                      {numberToString(team.getId()),
                       team.getName(),
                       numberToString(team.getCharacterIds().size())},
                      widths);
    }
    printTableBorder(m_output, widths);
}

void Menu::displayTeamSummaries(const TeamManager& teamManager) const {
    const std::vector<Team> teams = teamManager.getAllTeams();

    printSectionTitle(m_output,
                      "DANH SÁCH TEAM HIỆN CÓ",
                      TEAM_TABLE_WIDTH);
    printThreeBlankLines(m_output);

    if (teams.empty()) {
        printCenteredMessage(m_output,
                             "[THÔNG BÁO] Chưa có đội hình nào.",
                             TEAM_TABLE_WIDTH);
        return;
    }

    const std::vector<std::size_t>& widths = teamColumnWidths();
    printTableBorder(m_output, widths);
    printTableRow(m_output,
                  {"ID đội", "Tên đội", "Danh sách nhân vật"},
                  widths);
    printTableBorder(m_output, widths);

    for (const Team& team : teams) {
        std::ostringstream idsBuilder;
        const std::vector<int>& ids = team.getCharacterIds();
        if (ids.empty()) {
            idsBuilder << "(rỗng)";
        } else {
            for (std::size_t index = 0; index < ids.size(); ++index) {
                if (index > 0) {
                    idsBuilder << ", ";
                }
                idsBuilder << ids[index];
            }
        }

        printTableRow(m_output,
                      {numberToString(team.getId()),
                       team.getName(),
                       idsBuilder.str()},
                      widths);
    }
    printTableBorder(m_output, widths);
}


void Menu::displayTeam(const Team& team,
                       const CharacterRoster& roster) const {
    std::ostringstream title;
    title << "ĐỘI " << team.getId() << " - " << team.getName()
          << " (" << team.getCharacterIds().size() << " thành viên)";

    printSectionTitle(m_output,
                      title.str(),
                      CHARACTER_TABLE_WIDTH);
    printThreeBlankLines(m_output);

    if (team.getCharacterIds().empty()) {
        printCenteredMessage(m_output,
                             "[THÔNG BÁO] Đội hình này chưa có nhân vật.",
                             CHARACTER_TABLE_WIDTH);
        return;
    }

    printCharacterTableHeader(m_output);
    for (int characterId : team.getCharacterIds()) {
        const Character* character = roster.findById(characterId);
        if (character == nullptr) {
            std::ostringstream warning;
            warning << "[CẢNH BÁO] Không tìm thấy nhân vật ID "
                    << characterId << " trong danh sách nhân vật.";
            printTableRow(m_output,
                          {numberToString(characterId), "-", warning.str(),
                           "-", "-", "-", "-", "-", "-"},
                          characterColumnWidths());
        } else {
            printCharacterRow(m_output, *character);
        }
    }
    printTableBorder(m_output, characterColumnWidths());
}


void Menu::displayBattle(const BattleEngine& battle,
                         const CharacterRoster& roster) const {
    printSectionTitle(m_output,
                      "TRẠNG THÁI TRẬN ĐẤU",
                      BATTLE_TABLE_WIDTH);

    printBoxLine(m_output,
                 "Trạng thái: "
                     + battleStateToVietnamese(battle.getState()),
                 BATTLE_TABLE_WIDTH);
    printBoxLine(m_output,
                 "Số hành động đã thực hiện: "
                     + numberToString(battle.getRoundsPlayed()),
                 BATTLE_TABLE_WIDTH);

    const Character* currentActor = battle.getCurrentActor();
    if (currentActor != nullptr) {
        std::ostringstream currentTurn;
        currentTurn << "Lượt hiện tại: [" << currentActor->getId()
                    << "] " << currentActor->getName();
        printBoxLine(m_output, currentTurn.str(), BATTLE_TABLE_WIDTH);
    } else {
        printBoxLine(m_output,
                     "Lượt hiện tại: Chưa có",
                     BATTLE_TABLE_WIDTH);
    }

    const Team* winner = battle.getWinnerTeam();
    if (winner != nullptr) {
        std::ostringstream winnerText;
        winnerText << "Đội chiến thắng: [" << winner->getId()
                   << "] " << winner->getName();
        printBoxLine(m_output, winnerText.str(), BATTLE_TABLE_WIDTH);
    }
    printFullBorder(m_output, BATTLE_TABLE_WIDTH, '-');

    const Team* teamA = battle.getTeamA();
    const Team* teamB = battle.getTeamB();
    if (teamA == nullptr || teamB == nullptr) {
        m_output << '\n';
        printCenteredMessage(
            m_output,
            "[THÔNG BÁO] Chưa có trận đấu. Hãy chọn hai đội để bắt đầu.",
            BATTLE_TABLE_WIDTH);
        return;
    }

    m_output << "\n\n";
    printBattleTeam(m_output, "ĐỘI A", *teamA, battle, roster);
    m_output << "\n\n";
    printBattleTeam(m_output, "ĐỘI B", *teamB, battle, roster);
}


void Menu::displayBattleResult(const BattleEngine& battle,
                              const CharacterRoster& roster) const {
    printSectionTitle(m_output,
                      "TRẬN ĐẤU ĐÃ KẾT THÚC!",
                      BATTLE_TABLE_WIDTH);

    const Team* winner = battle.getWinnerTeam();
    if (winner != nullptr) {
        m_output << "\n";
        std::ostringstream winnerTitle;
        winnerTitle << "ĐỘI CHIẾN THẮNG: [" << winner->getId() << "] " << winner->getName();
        printCenteredMessage(m_output, winnerTitle.str(), BATTLE_TABLE_WIDTH);
        m_output << "\n";
        printBattleTeam(m_output, "NHÂN VẬT ĐỘI CHIẾN THẮNG", *winner, battle, roster);
    }

    m_output << "\n\n";
    printSectionTitle(m_output,
                      "BẢNG THỐNG KÊ TRẬN ĐẤU GIỮA 2 TEAM",
                      BATTLE_TABLE_WIDTH);
    printThreeBlankLines(m_output);

    static const std::vector<std::size_t> statsWidths = {
        5U, 20U, 18U, 12U, 10U, 8U
    };

    printTableBorder(m_output, statsWidths);
    printTableRow(m_output,
                  {"ID", "Tên nhân vật", "Đội", "Tổng DMG", "Số lượt", "Kills"},
                  statsWidths);
    printTableBorder(m_output, statsWidths);

    const auto& statsList = battle.getBattleStats();
    for (const auto& stats : statsList) {
        printTableRow(m_output,
                      {numberToString(stats.characterId),
                       stats.characterName,
                       stats.teamName,
                       numberToString(stats.damageDealt),
                       numberToString(stats.turnsTaken),
                       numberToString(stats.kills)},
                      statsWidths);
    }
    printTableBorder(m_output, statsWidths);
    m_output << "\n";
}


void Menu::waitForZeroToReturn() {
    int choice = -1;
    while (true) {
        if (!readInt("Nhấn 0 để quay về menu trận đấu: ", 0, 0, choice)) {
            return;
        }
        if (choice == 0) {
            return;
        }
    }
}


void Menu::showSuccess(const std::string& message) const {
    m_output << "[THÀNH CÔNG] " << message << '\n';
}


void Menu::showError(const std::string& message) const {
    m_output << "[LỖI] " << message << '\n';
}


void Menu::showInfo(const std::string& message) const {
    m_output << "[THÔNG BÁO] " << message << '\n';
}


void Menu::showTeamInfo(const Team& team) const {
    std::ostringstream info;
    info << "ID đội " << team.getId()
         << " | " << team.getName()
         << " | Danh sách nhân vật: ";

    const std::vector<int>& characterIds = team.getCharacterIds();
    if (characterIds.empty()) {
        info << "(rỗng)";
    } else {
        for (std::size_t index = 0; index < characterIds.size(); ++index) {
            if (index > 0) {
                info << ", ";
            }
            info << characterIds[index];
        }
    }
    m_output << "[THÔNG BÁO] " << info.str() << '\n';
}


void Menu::showGoodbye() const {
    m_output << '\n';
    printFullBorder(m_output, MENU_WIDTH, '=');
    printCenteredBoxLine(m_output,
                         "Dữ liệu đã được lưu. Cảm ơn bạn đã chơi!",
                         MENU_WIDTH);
    printFullBorder(m_output, MENU_WIDTH, '=');
}


std::string Menu::trim(const std::string& text) {
    std::size_t first = 0U;
    while (first < text.size()
           && std::isspace(static_cast<unsigned char>(text[first])) != 0) {
        ++first;
    }

    std::size_t last = text.size();
    while (last > first
           && std::isspace(static_cast<unsigned char>(text[last - 1U])) != 0) {
        --last;
    }

    return text.substr(first, last - first);
}


std::string Menu::toLowerAscii(const std::string& text) {
    std::string result = text;
    for (char& character : result) {
        const unsigned char safeCharacter =
            static_cast<unsigned char>(character);
        if (safeCharacter < 128U) {
            character = static_cast<char>(std::tolower(safeCharacter));
        }
    }
    return result;
}

} // namespace TurnBasedGame
