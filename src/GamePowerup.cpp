#include "Projector.hpp"
#include "DropShot.hpp"
#include "ColorTracker.hpp" 
#include "UartDecoder.hpp"
#include "ContourTracker.hpp"
#include "CameraInterface.hpp"
#include "Table.hpp"

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
    
    Projector proj = Projector(1536,768);
    CameraInterface cam = CameraInterface();
    ColorTracker colTrack = ColorTracker();
    ContourTracker conTrack = ContourTracker();
    std::string deviceStr = "/dev/ttyUSB0";
    UartDecoder uart = UartDecoder(deviceStr);
    Table table = Table(cam, colTack, conTrack, 10);
    if(uart.serial_port == 0){
        std::cout << "Problem Setting Up Serial Port" << std::endl;
        return 1;
    }
	std::string path= "/home/rachel/git/spectator/menus/main.tiff";
    int32_t maxScore = 11;
    int returnVal = STARTUP;


    for(;;){
        uart.readSerial();
        switch(uart.getButton()){
            case STAR:
                table.startDetection();
                returnVal = DropShot(proj, uart, cam, colTrack, conTrack, table, maxScore);
                table.stopDetection();
                break;
            case TWO:
                maxScore += 1;
                break;
            case FIVE:
                maxScore -= 1;
                break;
            default:
                break; 
        }
        if(returnVal == SHUTDOWN){
            break;
        }
        std::string scoreStr = std::to_string(maxScore);
        proj.writeText(scoreStr, 10, 0, 0, 1, 1, 1);
        proj.renderTiff(path,0,0,1);
        proj.refresh();
    }
    return 0;
}
