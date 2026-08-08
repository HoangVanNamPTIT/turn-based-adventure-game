/**
 * @file main.cpp
 * @module Application
 * @brief Program entrypoint creating GameApp and starting run loop.
 * @features Minimal main that constructs and runs the GameApp.
 * @input none
 * @output application lifecycle
 */

#include "GameApp.h"

int main() {
    TurnBasedGame::GameApp app;
    return app.run();
}
