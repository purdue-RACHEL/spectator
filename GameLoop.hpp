#ifndef INCLUDE_GAME_LOOP
#define INCLUDE_GAME_LOOP

// Includes
#include "UartDecoder.hpp"

// Defines
#define BOUNCE_TIMEOUT_MS   3000

// Type Declarations

// Struct Declarations

// Enum Declarations
enum GameStatus {
    ACTIVE,
    SHUTDOWN,
};


// Functions
int gameLoop();
int handleBounce(Bounce bounce);

#endif
