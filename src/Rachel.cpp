#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include "Rachel.hpp"
#include "UartDecoder.hpp"
#include "Projector.hpp"
#include "ColorTracker.hpp"

score_t score_red;
score_t score_blue;
std::chrono::milliseconds last_score_timeout;

GameStatus gameStatus;
bool dropShotGrid[GRID_SIZE * 2][GRID_SIZE];

int VanillaShot(Projector& proj, UartDecoder& uart, CameraInterface& cam, ColorTracker& colTrack, ContourTracker& conTracker, int32_t maxScore)
{
    // Init Globals
    gameStatus = ACTIVE;
    score_red = 0;
    score_blue = 0;
    last_score_timeout = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

    Bounce bounce = NOBOUNCE;
    Button button = NOPRESS;
    StatusChange bounceEvent;
    StatusChange buttonEvent;

    auto start = std::chrono::high_resolution_clock::now();
    auto target = start;

    updateDisplay(proj);
    // Disable Handicap Adjustment for Now
    /*
    while(gameStatus == STARTUP) {
        std::this_thread::sleep_until(target);
        target += std::chrono::milliseconds(UART_POLL_MS);

        uart.readSerial();
        handleButton(uart.getButton()); 
    }
    */

    while(gameStatus == ACTIVE || gameStatus == GAMEOVER || gameStatus == PAUSE) {
        std::this_thread::sleep_until(target);
        target += std::chrono::milliseconds(UART_POLL_MS);

        uart.readSerial();
        bounce = uart.getBounce();
        button = uart.getButton();
        bounceEvent = handleBounce(bounce);
        buttonEvent = handleButton(button);

	// DEBUG PRINT
        if(gameStatus == ACTIVE && (bounceEvent == SCORE_CHANGE || buttonEvent == SCORE_CHANGE)) {
            std::cout << "red  score: " << score_red << std::endl;
	    std::cout << "blue score: " << score_blue << std::endl;
        }

        //if(gameStatus != EXITGAME && ((score_red >= maxScore && score_red - score_blue > 1) || (score_blue >= maxScore && score_blue - score_red > 1))) {
        if(gameStatus != EXITGAME && ((score_red >= maxScore || score_blue >= maxScore) && (score_red - score_blue > 1 || score_blue - score_red > 1))) {
            gameStatus = GAMEOVER;
            //std::cout << "Game Over message" << std::endl;
        }
        updateDisplay(proj);
    }
    return gameStatus;
}

StatusChange handleBounce(Bounce bounce) {

    StatusChange statusChange = NO_CHANGE;

    static Bounce previous_bounce = NOBOUNCE;
    static auto timeout = std::chrono::high_resolution_clock::now();
    static auto invalid_timeout = timeout;
    std::chrono::milliseconds curr_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

    if(gameStatus != ACTIVE) { return statusChange; }

    if(bounce != NOBOUNCE && (curr_time > last_score_timeout)) {

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
		    last_score_timeout = curr_time + std::chrono::milliseconds(SCORE_TIMEOUT_MS);
                } else {
                    if(score_red != SCORE_MAX) {
                        score_red += 1;
                        statusChange = SCORE_CHANGE;
                    } else {
                        statusChange = FAILED_SCORE_CHANGE;
                    }
		    last_score_timeout = curr_time + std::chrono::milliseconds(SCORE_TIMEOUT_MS);
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
    if(gameStatus == STARTUP || gameStatus == ACTIVE || gameStatus == PAUSE) {
        switch(button) {
            case A:
		if(gameStatus == PAUSE){
                	if(score_red != SCORE_MAX)  { score_red += 1; statusChange = SCORE_CHANGE; }
                	else                        { statusChange = FAILED_SCORE_CHANGE; }
		} break;
            case ONE:
		if(gameStatus == PAUSE) { 
                	if(score_blue != SCORE_MAX) { score_blue += 1; statusChange = SCORE_CHANGE; }
                	else                        { statusChange = FAILED_SCORE_CHANGE; }
		} break;
            case B:
		if(gameStatus == PAUSE) { 
			if(score_red != 0)          { score_red -= 1; statusChange = SCORE_CHANGE; }
			else                        { statusChange = FAILED_SCORE_CHANGE; }
		} break;
            case FOUR:
		if(gameStatus == PAUSE) { 
			if(score_blue != 0)         { score_blue -= 1; statusChange = SCORE_CHANGE; }
			else                        { statusChange = FAILED_SCORE_CHANGE; }
		}
		break;
            case STAR:
                if (gameStatus == STARTUP)  { gameStatus = ACTIVE; score_red = 0; score_blue = 0; }
		else if (gameStatus == PAUSE) { gameStatus = ACTIVE; }
	        else if (gameStatus == ACTIVE) { gameStatus = PAUSE; }
                statusChange =  MENU_CHANGE; break;
            case POUND: 
		if (gameStatus == PAUSE){
		    gameStatus = EXITGAME;
		    statusChange = EXIT2MAIN_CHANGE; 
		} break;
            case ZERO: 
                if (gameStatus == PAUSE) {
                    score_red = score_blue = 0;
                    for(int i = 0; i < GRID_SIZE*2; i++)
                        for(int j = 0; j < GRID_SIZE; j++)
                            dropShotGrid[i][j] = true;
		    gameStatus = ACTIVE;
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
            case B:
		if (score_red != 0) { score_red -= 1; }
                statusChange = SCORE_CHANGE;
                gameStatus = PAUSE; break;
            case FOUR:
		if (score_blue != 0) { score_blue -= 1; }
                statusChange = SCORE_CHANGE;
                gameStatus = PAUSE; break;
            case ZERO:
                score_red = score_blue = 0;
                gameStatus = ACTIVE;
                statusChange = RESTART_CHANGE; break;
            case POUND:
		gameStatus = EXITGAME; statusChange = EXIT2MAIN_CHANGE; break;
        }
    }

    return statusChange;
}

StatusChange handleDropBounce(Bounce bounce, cv::Point2f bounceLoc) {

    StatusChange statusChange = NO_CHANGE;

    static Bounce previous_bounce = NOBOUNCE;
    static auto timeout = std::chrono::high_resolution_clock::now();
    static auto invalid_timeout = timeout;
    std::chrono::milliseconds curr_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

    if(gameStatus != ACTIVE) { return statusChange; }

    if(bounce != NOBOUNCE && (curr_time > last_score_timeout)) {
	// exit if ball was not seen
	/*
	if (bounceLoc.x == -1){
		return statusChange;
	}
	*/

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
		    last_score_timeout = curr_time + std::chrono::milliseconds(SCORE_TIMEOUT_MS);
                } else {
                    if(score_red != SCORE_MAX) {
                        score_red += 1;
                        statusChange = SCORE_CHANGE;
                    } else {
                        statusChange = FAILED_SCORE_CHANGE;
                    }
		    last_score_timeout = curr_time + std::chrono::milliseconds(SCORE_TIMEOUT_MS);
                }

	    for(int i = 0; i < GRID_SIZE * 2; i++)
		for(int j = 0; j < GRID_SIZE; j++)
		    dropShotGrid[i][j] = true;
                previous_bounce = NOBOUNCE;
                timeout = invalid_timeout;
                    
            // game continuation
            } else {
                previous_bounce = bounce;

                // NEW
                int x = bounceLoc.x * GRID_SIZE *2;
                int y = bounceLoc.y * GRID_SIZE;

                // this grid already hit
                if(dropShotGrid[x][y] == false) {

                    if(bounce == RED)
                        score_red++;
                    else if(bounce == BLUE)
                        score_blue++;

                    // TODO: reset whole grid here?
                    for(int i = 0; i < GRID_SIZE*2; i++)
                        for(int j = 0; j < GRID_SIZE; j++)
                            dropShotGrid[i][j] = true;
                } else {
                    dropShotGrid[x][y] = false;
                }

                // END NEW

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
	    for(int i = 0; i < GRID_SIZE*2; i++)
		for(int j = 0; j < GRID_SIZE; j++)
		    dropShotGrid[i][j] = true;
            timeout = invalid_timeout;
            previous_bounce = NOBOUNCE;
        }
    }
    
    return statusChange;
}

void updateDisplay(Projector& proj) {
    std::string path;
    std::string redScoreLabel = "RED:";
    std::string blueScoreLabel = "BLUE:";
    std::string redScoreStr = std::to_string(score_red);
    std::string blueScoreStr = std::to_string(score_blue);
    switch(gameStatus){
        case PAUSE:    
		path= "/home/rachel/git/spectator/menus/Active.tiff";
    		proj.renderTiff(path,80,150,.25); //need to adjust scale and location
                proj.drawCenterLine();
		proj.writeText(redScoreLabel, 5, 860, 100, 0,0,255);
		proj.writeText(redScoreStr, 5, 860, 200, 0, 0, 255);
		proj.writeText(blueScoreLabel, 5, 860, 300, 255,0,0);
        	proj.writeText(blueScoreStr, 5, 860, 400, 255, 0, 0);
		break;
        case GAMEOVER:  
		path= "/home/rachel/git/spectator/menus/GameOver.tiff"; 
    		proj.renderTiff(path,80,150,.25); //need to adjust scale and location
                proj.drawCenterLine();
		proj.writeText(redScoreLabel, 5, 860, 100, 0,0,255);
		proj.writeText(redScoreStr, 5, 860, 200, 0, 0, 255);
		proj.writeText(blueScoreLabel, 5, 860, 300, 255,0,0);
        	proj.writeText(blueScoreStr, 5, 860, 400, 255, 0, 0);
		break;
        case STARTUP:   
		path= "/home/rachel/git/spectator/menus/StartUp.tiff"; 
    		proj.renderTiff(path,80,150,.25); //need to adjust scale and location
                proj.drawCenterLine();
		proj.writeText(redScoreLabel, 5, 860, 100, 0,0,255);
		proj.writeText(redScoreStr, 5, 860, 200, 0, 0, 255);
		proj.writeText(blueScoreLabel, 5, 860, 300, 255,0,0);
        	proj.writeText(blueScoreStr, 5, 860, 400, 255, 0, 0);
		break;
	case ACTIVE:
                proj.drawCenterLine();
                proj.updateScore(score_red, score_blue);
		break;
    }
    proj.refresh();
}

int DropShot(Projector& proj, UartDecoder& uart, CameraInterface& cam, Table& table, ColorTracker& colTrack, ContourTracker& conTracker, int32_t maxScore)
{
    // Init Globals
    for (int i=0; i<GRID_SIZE * 2; i++){
	for (int j=0; j<GRID_SIZE; j++){
		dropShotGrid[i][j] = true;
	}
    }
    gameStatus = ACTIVE;
    score_red = 0;
    score_blue = 0;
    last_score_timeout = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

    Bounce bounce = NOBOUNCE;
    Button button = NOPRESS;
    StatusChange bounceEvent;
    StatusChange buttonEvent;

    auto start = std::chrono::high_resolution_clock::now();
    auto target = start;

    updateDisplay(proj);
    // Disable Handicap Adjustment for Now
    /*
    while(gameStatus == STARTUP) {
        std::this_thread::sleep_until(target);
        target += std::chrono::milliseconds(UART_POLL_MS);

        uart.readSerial();
        handleButton(uart.getButton()); 
    }
    */

    while(gameStatus == ACTIVE || gameStatus == GAMEOVER || gameStatus == PAUSE) {
        std::this_thread::sleep_until(target);
        target += std::chrono::milliseconds(UART_POLL_MS);

        uart.readSerial();
        bounce = uart.getBounce();
        button = uart.getButton();

        // TODO: fix this if necessary
        if(1) {
            cv::Point2f bounceLoc = cv::Point2f(0,0);
            bounceLoc = table.getAveragedPos(uart.last_time, uart.curr_time);
            bounceEvent = handleDropBounce(bounce, bounceLoc);
        }

        // bounceEvent = handleDropBounce(bounce);
        buttonEvent = handleButton(button);

	// DEBUG PRINT
        if(gameStatus == ACTIVE && (bounceEvent == SCORE_CHANGE || buttonEvent == SCORE_CHANGE)) {
            std::cout << "red  score: " << score_red << std::endl;
	    std::cout << "blue score: " << score_blue << std::endl;
        }

        if(gameStatus != EXITGAME && ((score_red >= maxScore && score_red - score_blue > 1) || (score_blue >= maxScore && score_blue - score_red > 1))) {
            gameStatus = GAMEOVER;
            //std::cout << "Game Over message" << std::endl;
        }
        updateDisplayDropShot(proj);
    }
    return gameStatus;
}

void updateDisplayDropShot(Projector& proj) {
    std::string path;
    std::string redScoreLabel = "RED:";
    std::string blueScoreLabel = "BLUE:";
    std::string redScoreStr = std::to_string(score_red);
    std::string blueScoreStr = std::to_string(score_blue);
    switch(gameStatus){
        case PAUSE:    
		path= "/home/rachel/git/spectator/menus/Active.tiff";
    		proj.renderTiff(path,80,150,.25); //need to adjust scale and location
                proj.drawCenterLine();
		proj.writeText(redScoreLabel, 5, 860, 100, 0,0,255);
		proj.writeText(redScoreStr, 5, 860, 200, 0, 0, 255);
		proj.writeText(blueScoreLabel, 5, 860, 300, 255,0,0);
        	proj.writeText(blueScoreStr, 5, 860, 400, 255, 0, 0);
		break;
        case GAMEOVER:  
		path= "/home/rachel/git/spectator/menus/GameOver.tiff"; 
    		proj.renderTiff(path,80,150,.25); //need to adjust scale and location
                proj.drawCenterLine();
		proj.writeText(redScoreLabel, 5, 860, 100, 0,0,255);
		proj.writeText(redScoreStr, 5, 860, 200, 0, 0, 255);
		proj.writeText(blueScoreLabel, 5, 860, 300, 255,0,0);
        	proj.writeText(blueScoreStr, 5, 860, 400, 255, 0, 0);
		break;
        case STARTUP:   
		path= "/home/rachel/git/spectator/menus/StartUp.tiff"; 
    		proj.renderTiff(path,80,150,.25); //need to adjust scale and location
                proj.drawCenterLine();
		proj.writeText(redScoreLabel, 5, 860, 100, 0,0,255);
		proj.writeText(redScoreStr, 5, 860, 200, 0, 0, 255);
		proj.writeText(blueScoreLabel, 5, 860, 300, 255,0,0);
        	proj.writeText(blueScoreStr, 5, 860, 400, 255, 0, 0);
		break;
	case ACTIVE:
		double squareH = proj.h/GRID_SIZE;
		double currX = 0;
		double currY = 0;
		for(int i=0; i<GRID_SIZE*2; i++){
		    currY = 0;
		    for(int j=0; j<GRID_SIZE; j++){
			//std::cout << i<< " " << j << ": " << dropShotGrid[i][j] << std::endl;
			if(dropShotGrid[i][j]) {
			    proj.renderSquare(currX,currY,squareH,squareH,75,75,75);
			    //proj.renderSquare(currX+8,currY+8,squareH-16,squareH-16,0,0,0);
			}
			else {
			    proj.renderSquare(currX,currY,squareH,squareH,0,0,0);
			    //proj.renderSquare(currX+8,currY+8,squareH-16,squareH-16,0,0,0);
			}
			currY += squareH;
		    } 
		    currX += squareH;
		}
		proj.updateScore(score_red, score_blue);
		break;
    }
    proj.refresh();
}
