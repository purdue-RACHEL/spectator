#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include "DropShot.hpp"
#include "UartDecoder.hpp"
#ifndef DISABLEOPENCV
#include "Projector.hpp"
#endif

score_t score_red = 0;
score_t score_blue = 0;

GameStatus gameStatus = STARTUP;
Game_Preferences_t game_preferences;
bool menuIsHidden = true;         //<----------------------- HAVE SOMETHING LIKE THIS

// GIVE PARAM: UARTDECODER WHEN CALLED FROM MAIN MENU
//int main(UartDecoder uart)
int main()
{
    Bounce bounce = NOBOUNCE;
    Button button = NOPRESS;

    std::string deviceStr = "/dev/ttyUSB0";
	UartDecoder uart = UartDecoder(deviceStr); // REMOVE AFTER NEW MAIN PARAM GET ADDED
#ifndef DISABLEOPENCV
	Projector proj(1920,1080);
#endif

    auto start = std::chrono::high_resolution_clock::now();
    auto target = start;

    while(gameStatus == STARTUP) { 
        std::this_thread::sleep_until(target);
        target += std::chrono::milliseconds(UART_POLL_MS);
        handleButton(uart.getButton()); 
    }

    while(gameStatus == ACTIVE || gameStatus == GAMEOVER) {

        // TODO: protection logic.
        // if we happen to take 35 ms to process, there is no point for the sleep
        // perhaps we skip the sleep
        std::this_thread::sleep_until(target);
        target += std::chrono::milliseconds(UART_POLL_MS);
        uart.readSerial();

        bounce = uart.getBounce();
        button = uart.getButton();

        // TODO: keep in mind, we may have short circuiting here......
        if((bounceEvent == SCORE_CHANGE) || (buttonEvent == SCORE_CHANGE)) {
            // debug
            std::cout << "red score: " << score_red << std::endl;
	        std::cout << "blue score: " << score_blue << std::endl;
            // EVENTUALLY UPDATE SCORE DISPLAY

            // TODO: project updates
            #ifndef DISABLEOPENCV
                    proj.drawCenterLine();
                    proj.updateScore(score_red,score_blue);
                    proj.refresh();
            #endif

            // TODO: clear sprites
        }

            // TODO: game finish logic - wins, ... mostly to be handled in projecting
        if(score_red >= game_preferences.max_score || score_blue >= game_preferences.max_score) {
            gameStatus = GAMEOVER;
            menuIsHidden = false;
            std::cout << "Game Over message" << std::endl;
        }
    }

    // WITH BIG MAIN FUNCTION - DO NOT CALL THIS PLZ
    uart.writeSerial(FORCE_SHUTDOWN);
    uart.closePort();
    // REMOVE AFTER BIG MAIN FX

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

    //static Button previous_button = NOPRESS;

    if(button == NOPRESS) { return NO_CHANGE; }
    
    if(menuIsHidden == true) {
        menuIsHidden == false;
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
                if(score_red != SCORE_MAX) { score_red += 1; statusChange = SCORE_CHANGE; }
                else { statusChange = FAILED_SCORE_CHANGE; } break;
            case A:
                if(score_blue != SCORE_MAX) { score_blue += 1; statusChange = SCORE_CHANGE; }
                else { statusChange = FAILED_SCORE_CHANGE; } break;
            case FOUR:
                if(score_red != 0) { score_red -= 1; statusChange = SCORE_CHANGE; }
                else { statusChange = FAILED_SCORE_CHANGE; } break;
            case B:
                if(score_blue != 0) { score_blue -= 1; statusChange = SCORE_CHANGE; }
                else { statusChange = FAILED_SCORE_CHANGE; } break;
            case STAR:
                menuIsHidden = false;
                if (gameStatus == STARTUP) { gameStatus = GameStatus.ACTIVE; }
                statusChange =  MENU_CHANGE; break;
            case POUND: menuIsHidden = true; gameStatus = EXITGAME; statusChange = EXIT2MAIN_CHANGE; break;
            case D:     menuIsHidden = true; gameStatus = SHUTDOWN; statusChange = EXIT_ALL_CHANGE; break;
            case ZERO: 
                if (gameStatus == ACTIVE) {
                    score_red = score_blue = 0;
                    menuIsHidden = true;
                    statusChange = RESTART_CHANGE;
                } break;
        }
    /*
        END-MENU (GAMEOVER): *MENU ALWAYS UP* <-----
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
            case POUND: menuIsHidden = true; gameStatus = EXITGAME; statusChange = EXIT2MAIN_CHANGE; break;
            case D:     menuIsHidden = true; gameStatus = SHUTDOWN; statusChange = EXIT_ALL_CHANGE; break;
        }
    }

    if (menuIsHidden == false) {
        // HIDE DA MENU  <---------------------------------------------------------------
    }

    return statusChange;
}
