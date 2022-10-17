#ifndef INCLUDE_GAME_LOOP
#define INCLUDE_GAME_LOOP
#include "UartDecoder.hpp"

#define BOUNCE_TIMEOUT_MS 3000

enum GameStatus {
    ACTIVE,
    SHUTDOWN,
};



int gameLoop();
int handleBounce(Bounce bounce);

#endif
