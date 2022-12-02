#ifndef INCLUDE_GAME_LOOP
#define INCLUDE_GAME_LOOP

// Includes
#include "UartDecoder.hpp"
#include "Projector.hpp"
#include "CameraInterface.hpp"
#include "ColorTracker.hpp"
#include "ContourTracker.hpp"

// Defines
#define SCORE_MAX UINT32_MAX
#define BOUNCE_TIMEOUT_MS   1500
#define SCORE_TIMEOUT_MS    3000
#define UART_POLL_MS        30

// Type Declarations
typedef uint32_t score_t;

// Enum Declarations
enum GameStatus {
    STARTUP  = 0,   //INIT
    ACTIVE   = 1,   //MID-GAME
    GAMEOVER = 2,   //POST-GAME
    PAUSE = 3,      //PAUSE
    EXITGAME = 4,   //EXIT TO MAIN MENU
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
    EXIT_ALL_CHANGE,
    NSTATUSCHANGE
};

// Functions
int VanillaShot(Projector&,UartDecoder&,CameraInterface&,ColorTracker&,ContourTracker&,int32_t);
StatusChange handleBounce(Bounce bounce);
StatusChange handleButton(Button button);
void updateDisplay(Projector& proj);


#endif
