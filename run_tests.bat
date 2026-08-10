@echo off
chcp 65001 >nul
echo ======================================================================
echo           COMPILING AND EXECUTING ALL SYSTEM UNIT TESTS               
echo ======================================================================

echo [1/8] Compiling TestUtils.exe...
g++ -std=c++11 -Iinclude src/Utils.cpp tests/TestUtils.cpp -o test_utils.exe
if %ERRORLEVEL% NEQ 0 (
    echo Compilation failed for TestUtils.exe!
    exit /b %ERRORLEVEL%
)

echo [2/8] Compiling TestCharacterAndSubclasses.exe...
g++ -std=c++11 -Iinclude src/Character.cpp src/Warrior.cpp src/Archer.cpp src/Mage.cpp src/Healer.cpp tests/TestCharacterAndSubclasses.cpp -o test_character.exe
if %ERRORLEVEL% NEQ 0 (
    echo Compilation failed for TestCharacterAndSubclasses.exe!
    exit /b %ERRORLEVEL%
)

echo [3/8] Compiling TestTeam.exe...
g++ -std=c++11 -Iinclude src/Team.cpp tests/TestTeam.cpp -o test_team.exe
if %ERRORLEVEL% NEQ 0 (
    echo Compilation failed for TestTeam.exe!
    exit /b %ERRORLEVEL%
)

echo [4/8] Compiling TestCharacterRoster.exe...
g++ -std=c++11 -Iinclude src/Character.cpp src/Warrior.cpp src/Archer.cpp src/Mage.cpp src/Healer.cpp src/CharacterRoster.cpp tests/TestCharacterRoster.cpp -o test_roster.exe
if %ERRORLEVEL% NEQ 0 (
    echo Compilation failed for TestCharacterRoster.exe!
    exit /b %ERRORLEVEL%
)

echo [5/8] Compiling TestTeamManager.exe...
g++ -std=c++11 -Iinclude src/Character.cpp src/Warrior.cpp src/Archer.cpp src/Mage.cpp src/Healer.cpp src/CharacterRoster.cpp src/Team.cpp src/TeamManager.cpp src/DataFileManager.cpp src/Utils.cpp tests/TestTeamManager.cpp -o test_team_manager.exe
if %ERRORLEVEL% NEQ 0 (
    echo Compilation failed for TestTeamManager.exe!
    exit /b %ERRORLEVEL%
)

echo [6/8] Compiling TestBattleEngine.exe...
g++ -std=c++11 -Iinclude src/Character.cpp src/Warrior.cpp src/Archer.cpp src/Mage.cpp src/Healer.cpp src/CharacterRoster.cpp src/Team.cpp src/TeamManager.cpp src/DataFileManager.cpp src/Utils.cpp src/BattleState.cpp src/BattleEngine.cpp tests/TestBattleEngine.cpp -o test_battle_engine.exe
if %ERRORLEVEL% NEQ 0 (
    echo Compilation failed for TestBattleEngine.exe!
    exit /b %ERRORLEVEL%
)

echo [7/8] Compiling TestDataFileManager.exe...
g++ -std=c++11 -Iinclude src/DataFileManager.cpp src/CharacterRoster.cpp src/Warrior.cpp src/Archer.cpp src/Mage.cpp src/Healer.cpp src/Character.cpp src/Team.cpp src/Utils.cpp tests/TestDataFileManager.cpp -o test_data_file_manager.exe
if %ERRORLEVEL% NEQ 0 (
    echo Compilation failed for TestDataFileManager.exe!
    exit /b %ERRORLEVEL%
)

echo [8/8] Compiling RunAllTests.exe (Master Suite)...
g++ -std=c++11 -Iinclude src/*.cpp tests/RunAllTests.cpp -o run_all_tests.exe
if %ERRORLEVEL% NEQ 0 (
    echo Compilation failed for RunAllTests.exe!
    exit /b %ERRORLEVEL%
)

echo.
echo ======================================================================
echo                   RUNNING MASTER UNIT TEST SUITE                      
echo ======================================================================
.\run_all_tests.exe
