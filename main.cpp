/**
 * @file main.cpp
 * @module Application
 * @brief Program entrypoint creating GameApp and starting run loop.
 * @features Minimal main that constructs and runs the GameApp.
 * @input none
 * @output application lifecycle
 */

#include "GameApp.h"

#ifdef _WIN32
#include <windows.h>
#endif

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif
    TurnBasedGame::GameApp app;
    return app.run();
}

