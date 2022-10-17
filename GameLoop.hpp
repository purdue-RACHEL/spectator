#ifndef INCLUDE_GAME_LOOP
#define INCLUDE_GAME_LOOP

#define BOUNCE_TIMEOUT_MS 1000

enum GameStatus {
    ACTIVE,
    SHUTDOWN,
};



int gameLoop();

#endif
