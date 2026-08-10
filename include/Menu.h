/**
 * @file Menu.h
 * @module Application
 * @brief Console menu handling for main application actions.
 * @features Display menus, read user input, dispatch commands to GameApp.
 * @input user selections from console
 * @output selections to GameApp
 */

#ifndef TURN_BASED_ADVENTURE_GAME_MENU_H
#define TURN_BASED_ADVENTURE_GAME_MENU_H

#include <iosfwd>
#include <string>
#include <vector>
#include "BattleState.h"

namespace TurnBasedGame {

class BattleEngine;
enum class BattleState;
class Character;
class CharacterRoster;
class Team;
class TeamManager;

class Menu {
public:
    Menu();
    Menu(std::istream& input, std::ostream& output);

    int showMainMenu();
    int showCharacterMenu();
    int showAddCharacterMenu();
    int showTeamMenu();
    int showBattleMenu(BattleState state = BattleState::READY);

    bool readInt(const std::string& prompt,
                 int minValue,
                 int maxValue,
                 int& value);
    bool readPositiveInt(const std::string& prompt, int& value);
    bool readRequiredText(const std::string& prompt, std::string& value);
    bool readOptionalLine(const std::string& prompt, std::string& value);
    bool readName(const std::string& prompt, std::string& value);
    bool confirm(const std::string& prompt);
    void waitForEnter();

    void displayRoster(const CharacterRoster& roster) const;
    void displayCharacters(
        const std::vector<const Character*>& characters) const;
    void displayTeams(const TeamManager& teamManager) const;
    void displayTeamSummaries(const TeamManager& teamManager) const;
    void displayTeam(const Team& team,
                     const CharacterRoster& roster) const;
    void displayBattle(const BattleEngine& battle,
                       const CharacterRoster& roster) const;
    void displayBattleResult(const BattleEngine& battle,
                            const CharacterRoster& roster) const;
    void waitForZeroToReturn();

    void showSuccess(const std::string& message) const;
    void showError(const std::string& message) const;
    void showInfo(const std::string& message) const;
    void showTeamInfo(const Team& team) const;
    void showGoodbye() const;

private:
    bool readLine(const std::string& prompt, std::string& value);
    static std::string trim(const std::string& text);
    static std::string toLowerAscii(const std::string& text);

    std::istream& m_input;
    std::ostream& m_output;
};

} // namespace TurnBasedGame

#endif // TURN_BASED_ADVENTURE_GAME_MENU_H
