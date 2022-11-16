#ifndef INCLUDE_GAME_LOOP
#define INCLUDE_GAME_LOOP

// Includes
#include "UartDecoder.hpp"

// Defines
#define SCORE_MAX UINT32_MAX
#define BOUNCE_TIMEOUT_MS   3000
#define UART_POLL_MS        30

// Type Declarations
typedef uint32_t score_t;
//typedef std::chrono::steady_clock::time_point time_t;

// Struct Declarations
typedef struct Game_Preferences_t {
    score_t max_score = 21;
    // TODO: continue adding these as needed
} Game_Preferences_t;

// Enum Declarations
enum GameStatus {
    STARTUP  = 0,   //INIT
    ACTIVE   = 1,   //MID-GAME
    GAMEOVER = 2,   //POST-GAME
    EXITGAME = 3,   //EXIT TO MAIN MENU
    SHUTDOWN = -1   //TURN OFF
};

enum StatusChange {
    NO_CHANGE,
    SCORE_CHANGE,
    FAILED_SCORE_CHANGE,
    RESTART_CHANGE,
    SHUTDOWN_CHANGE,
    ACTIVE_CHANGE,
    MENU_CHANGE,
    EXIT2MAIN_CHANGE,
    EXIT_ALL_CHANGE
};

// Functions
int gameLoop();
StatusChange handleBounce(Bounce bounce);
StatusChange handleButton(Button button);

#endif
