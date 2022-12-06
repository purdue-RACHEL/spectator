#include <chrono>

#include "Projector.hpp"
#include "Rachel.hpp"
#include "ColorTracker.hpp" 
#include "UartDecoder.hpp"
#include "ContourTracker.hpp"
#include "CameraInterface.hpp"

void updateMainMenu(Projector& proj, std::string& path, int32_t maxScore, int32_t gameMode);

int main(int argc, char ** argv){

	// TODO:
    // 1. any initial setup
    // 2. calibrate camera?
    // 3. calibrate projector? might need to be done manually.

    /*
    Thread List:

    1. MainThread - poll MCU every X ms and interpret the packet.
                    cause any changes necessary. updated variables
                    that are needed for other threads will be read
                    automatically in-loop - no need to alert threads.
                    I recommend we set X to be at least 30 FPS.
                    Also read any variables that are needed for display
                    and use them to rewrite to the projector after every
                    necessary update

    2. CameraThread - interpret camera data - this is by far the most
                      complex thread - we may need more than 1. data
                      from this thread does not yet have a laid out
                      representation.
    */
    // I think multiple threads could be a bit overkill here. Specifically because all
    // these events, (conduct game logic, display to projector, read from camera, etc.)
    // should happen in the same sequence each loop of the game, and we would probably
    // just end up locking every thread while one is running. Also managing shared 
    // variables between threads can be a headache. I commented it out for now
    // just to make testing simpler.

    
    Projector proj = Projector(1600,860);
    CameraInterface cam = CameraInterface();
    ColorTracker colTrack = ColorTracker();
    ContourTracker conTrack = ContourTracker();
    std::string deviceStr = "/dev/ttyUSB0";
    UartDecoder uart = UartDecoder(deviceStr);
    Table table = Table(cam, colTrack, conTrack, 30);
    table.setTableBorder();
    if(uart.serial_port == 0){
        std::cout << "Problem Setting Up Serial Port" << std::endl;
        return 1;
    }
    int returnVal = STARTUP;

    std::string path= "/home/rachel/git/spectator/menus/Main.tiff";
    std::string exitStr = "PRESS AGAIN TO EXIT";
    int32_t maxScore = 11;
    int32_t gameMode = 0;

    Button lastPress = NOPRESS;
    Button currPress = NOPRESS;


//auto start = std::chrono::high_resolution_clock::now();
    //auto target = start;

#ifdef TESTBUILDINESS
    return EXIT_SUCCESS;
#endif

    updateMainMenu(proj, path, maxScore, gameMode);
    for(;;){
        uart.readSerial();
        //std::this_thread::sleep_until(target);
        //target += std::chrono::milliseconds(UART_POLL_MS);
	currPress = uart.getButton();
	if(currPress == NOPRESS)
		continue;
        switch(currPress){
            case ZERO:
                break;
            case STAR:
		switch(gameMode){
		    case 0:
                    	VanillaShot(proj, uart, cam, colTrack, conTrack, maxScore);
			break;
		    case 1:
			table.startDetection();
			DropShot(proj, uart, cam, table, colTrack, conTrack, maxScore);
			table.stopDetection();
			break;
		   default:
			std::cout << "ERROR WHEN CHOOSING GAME MODE" << std::endl;
			break;
		}
                break;
            case TWO:
        	maxScore += 1;
                break;
            case FIVE:
		if(maxScore > 6){
        	    maxScore -= 1;
		}	
                break;
	    case ONE:
		gameMode = (gameMode + 1) % 2;
		break;
	    case FOUR:
		gameMode = (gameMode + 1) % 2;
		break;
	    case D:
		proj.writeText(exitStr, 4, 860, 700, 0, 0, 255);
		if(lastPress == D){
			returnVal = SHUTDOWN;
		}
		break;
            default:
                break; 
        }
	lastPress = currPress;
        if(returnVal == SHUTDOWN){
	    std::cout << "GAME STATE SHUTDOWN, BYE BYE." << std::endl;
            break;
        }
        updateMainMenu(proj, path, maxScore, gameMode);
    }
    return 0;
}

void updateMainMenu(Projector& proj, std::string& path, int32_t maxScore, int32_t gameMode){
	std::string maxScoreLabel = "MAX SCORE:";
        std::string maxScoreStr = std::to_string(maxScore);
	std::string gameModeLabel = "GAMEMODE:";
	std::string gameModeStr;
	switch(gameMode){
	    case 0: gameModeStr = "Vanilla"; break;
	    case 1: gameModeStr = "Drop Shot"; break;
	    default: std::cout << "IMPROPER GAMEMODE" << std::endl; break;
	}
	proj.writeText(gameModeLabel, 5, 860, 300, 255, 255, 255);
	proj.writeText(gameModeStr, 5, 860, 400, 255, 255, 255);
	proj.drawCenterLine();
        proj.writeText(maxScoreLabel, 5, 860, 100, 255, 255, 255);
        proj.writeText(maxScoreStr, 5, 860, 200, 255, 255, 255);
        proj.renderTiff(path,80,150,.25);
        proj.refresh();
}
