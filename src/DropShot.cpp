#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include "DropShot.hpp"
#include "UartDecoder.hpp"
#include "Projector.hpp"
#include "ColorTracker.hpp"
#include "Table.hpp"

score_t score_red = 0;
score_t score_blue = 0;

GameStatus gameStatus = STARTUP;
Game_Preferences_t game_preferences;
bool menuIsVisible = true;

int DropShot(Projector proj, UartDecoder uart, CameraInterface cam, ColorTracker colTrack, ContourTracker conTracker, int32_t maxScore)
{
    Bounce bounce = NOBOUNCE;
    Button button = NOPRESS;

    auto start = std::chrono::high_resolution_clock::now();
    auto target = start;

    while(gameStatus == STARTUP) {
        std::this_thread::sleep_until(target);
        target += std::chrono::milliseconds(UART_POLL_MS);

        uart.readSerial();
        handleButton(uart.getButton()); 
    }

    while(gameStatus == ACTIVE || gameStatus == GAMEOVER) {
        std::this_thread::sleep_until(target);
        target += std::chrono::milliseconds(UART_POLL_MS);

        uart.readSerial();
        bounce = uart.getBounce();
        button = uart.getButton();

        if(bounceEvent == SCORE_CHANGE || buttonEvent == SCORE_CHANGE) {
            std::cout << "red  score: " << score_red << std::endl;
	        std::cout << "blue score: " << score_blue << std::endl;

            proj.drawCenterLine();
            proj.updateScore(score_red, score_blue);
            proj.refresh();

            // TODO: clear sprites
        }

        if((score_red >= maxScore && score_red - score_blue > 1) || (score_blue >= maxScore && score_blue - score_red > 1)) {
            gameStatus = GAMEOVER;
            menuIsVisible = true;
            std::cout << "Game Over message" << std::endl;
        }
    }
    return gameStatus;
}

StatusChange handleBounce(Bounce bounce) {

    StatusChange statusChange = NO_CHANGE;

    static Bounce previous_bounce = NOBOUNCE;
    static auto timeout = std::chrono::high_resolution_clock::now();
    static auto invalid_timeout = timeout;

    if(bounce != NOBOUNCE) {

        // serve condition
        if(previous_bounce == NOBOUNCE) {
            // TODO: any necessary serve logic
            previous_bounce = bounce;
            timeout = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(BOUNCE_TIMEOUT_MS);
        } else {

            // award points
            if(previous_bounce == bounce) {
                if(bounce == RED) {
                    if(score_blue != SCORE_MAX) {
                        score_blue += 1;
                        statusChange = SCORE_CHANGE;
                    } else {
                        statusChange = FAILED_SCORE_CHANGE;
                    }
                } else {
                    if(score_red != SCORE_MAX) {
                        score_red += 1;
                        statusChange = SCORE_CHANGE;
                    } else {
                        statusChange = FAILED_SCORE_CHANGE;
                    }
                }

                previous_bounce = NOBOUNCE;
                timeout = invalid_timeout;
                    
            // game continuation
            } else {
                previous_bounce = bounce;
                timeout = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(BOUNCE_TIMEOUT_MS);
            }
        }
    }

    if(timeout != invalid_timeout) {
        // bounce timed out
        if(std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(0) > timeout) {
            if(previous_bounce == RED) {
                if(score_blue != SCORE_MAX) {
                    score_blue += 1;
                    statusChange = SCORE_CHANGE;
                } else {
                    statusChange = FAILED_SCORE_CHANGE;
                }
            } else if(previous_bounce == BLUE) {
                if(score_red != SCORE_MAX) {
                    score_red += 1;
                    statusChange = SCORE_CHANGE;
                } else {
                    statusChange = FAILED_SCORE_CHANGE;
                }
            }
            timeout = invalid_timeout;
            previous_bounce = NOBOUNCE;
        }
    }
    
    return statusChange;
}

StatusChange handleButton(Button button) {

    StatusChange statusChange = NO_CHANGE;

    if(button == NOPRESS) { return statusChange; }
    
    if(menuIsVisible == false) {
        menuIsVisible = true;
        return MENU_CHANGE;
    }
    /*
    START-GAME MENU (GAME):
        (HANDICAPS)
        1 == SCORE BLUE ++
        A == SCORE RED ++
        4 == SCORE BLUE --
        B == SCORE RED --

        * == START GAME 
        # == RETURN TO MAIN MENU
        D == EXIT
    */
    /*
    MID-GAME MENU (PAUSE):
        1 == SCORE BLUE ++
        A == SCORE RED ++
        4 == SCORE BLUE --
        B == SCORE RED --

        * == CONTINUE (GETS RID OF MENU)
        0 == RESTART
        # == RETURN TO MAIN MENU
        D == EXIT
    */
    if(gameStatus == STARTUP || gameStatus == ACTIVE) {
        switch(button) {
            case ONE:
                if(score_red != SCORE_MAX)  { score_red += 1; statusChange = SCORE_CHANGE; }
                else                        { statusChange = FAILED_SCORE_CHANGE; } break;
            case A:
                if(score_blue != SCORE_MAX) { score_blue += 1; statusChange = SCORE_CHANGE; }
                else                        { statusChange = FAILED_SCORE_CHANGE; } break;
            case FOUR:
                if(score_red != 0)          { score_red -= 1; statusChange = SCORE_CHANGE; }
                else                        { statusChange = FAILED_SCORE_CHANGE; } break;
            case B:
                if(score_blue != 0)         { score_blue -= 1; statusChange = SCORE_CHANGE; }
                else                        { statusChange = FAILED_SCORE_CHANGE; } break;
            case STAR:
                menuIsVisible = true;
                if (gameStatus == STARTUP)  { gameStatus = ACTIVE; score_red = 0; score_blue = 0; }
                statusChange =  MENU_CHANGE; break;
            case POUND: gameStatus = EXITGAME; statusChange = EXIT2MAIN_CHANGE; break;
            case D:     gameStatus = SHUTDOWN; statusChange = EXIT_ALL_CHANGE; break;
            case ZERO: 
                if (gameStatus == ACTIVE) {
                    score_red = score_blue = 0;
                    menuIsVisible = false;
                    statusChange = RESTART_CHANGE;
                } break;
        }
    /*
        END-MENU (GAMEOVER):
            4 == SCORE BLUE --
            B == SCORE RED --

            0 == PLAY AGAIN (RESTART) 
            # == RETURN TO MAIN MENU
            D == EXIT
    */
    } else if(gameStatus == GAMEOVER) {
        switch(button) {
            case FOUR:
                score_red -= 1; 
                statusChange = SCORE_CHANGE;
                gameStatus = ACTIVE; break;
            case B:
                score_blue -= 1; 
                statusChange = SCORE_CHANGE;
                gameStatus = ACTIVE; break;
            case ZERO:
                score_red = score_blue = 0;
                gameStatus = ACTIVE;
                statusChange = RESTART_CHANGE; break;
            case POUND: gameStatus = EXITGAME; statusChange = EXIT2MAIN_CHANGE; break;
            case D:     gameStatus = SHUTDOWN; statusChange = EXIT_ALL_CHANGE; break;
        }
        menuIsVisible = false;
    }

    return statusChange;
}

void DisplayMenu() {

}
