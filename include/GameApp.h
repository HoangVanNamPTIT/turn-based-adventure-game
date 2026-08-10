/**
 * @file GameApp.h
 * @module Application
 * @brief High-level orchestrator that wires modules together and runs the main loop.
 * @features Initializes roster, team manager, battle engine, menu, and persistence.
 * @input user menu commands
 * @output invokes service layer and displays results
 */

#ifndef TURN_BASED_ADVENTURE_GAME_GAME_APP_H
#define TURN_BASED_ADVENTURE_GAME_GAME_APP_H

#include <iostream>
#include <istream>
#include <ostream>
#include <string>

#include "BattleEngine.h"
#include "CharacterRoster.h"
#include "Menu.h"
#include "TeamManager.h"

namespace TurnBasedGame {

class GameApp {
public:
    explicit GameApp(std::istream& input = std::cin,
                     std::ostream& output = std::cout);

    int run();

private:
    bool initialize();
    bool checkBattleLock();
    bool loadCharacters();
    void loadTeams();
    bool saveCharacters() const;
    bool saveTeams() const;
    bool saveData();

    void runCharacterMenu();
    void runTeamMenu();
    void runBattleMenu();

    void handleListCharacters();
    void handleAddCharacterMenu();
    void handleAddWarrior();
    void handleAddMage();
    void handleUpdateCharacter();
    void handleDeleteCharacter();
    void handleSearchCharacterById();
    void handleSearchCharacterByName();

    void handleListTeams();
    void handleViewTeam();
    void handleCreateTeam();
    void handleRenameTeam();
    void handleDeleteTeam();
    void handleAddCharacterToTeam();
    void handleRemoveCharacterFromTeam();

    void handleStartBattle();
    void handleShowBattleStatus();
    void handleContinueBattle();
    void handleResetBattle();

    void handleShowTeamStatistics();

    CharacterRoster m_roster;
    TeamManager m_teamManager;
    BattleEngine m_battleEngine;
    Menu m_menu;

    const std::string m_charactersFilePath{"data/characters.txt"};
    const std::string m_teamsFilePath{"data/teams.txt"};
    const std::string m_teamStatsFilePath{"data/team_win_loss_stats.txt"};
};

} // namespace TurnBasedGame

#endif // TURN_BASED_ADVENTURE_GAME_GAME_APP_H
