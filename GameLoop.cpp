#include <chrono>
#include "GameLoop.hpp"
#include <thread>
#include "UartDecoder.hpp"
#include <string>

using clock_type = std::chrono::high_resolution_clock;

int main(int argc, char ** argv){

	// TODO:
    // 1. any initial setup
    // 2. calibrate camera?
    // 3. calibrate projector? might need to be done manually.
    // 4. start threads

    /*
    Thread List:

    1. MainThread - poll MCU every X ms and interpret the packet.
                    cause any changes necessary. updated variables
                    that are needed for other threads will be read
                    automatically in-loop - no need to alert threads.
                    I recommend we set X to be at least 30 FPS.
    
    2. ProjectorThread - read any variables that are needed for display
                         and use them to rewrite to the projector every
                         Y ms. I recommend we set Y to be 30 FPS.

    3. CameraThread - interpret camera data - this is by far the most
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
    /*
    std::thread mainThread(mainLoop);
    std::thread projectorThread(projectorLoop);
    std::thread cameraThread(cameraLoop);

    // TODO: idk what to do here yet
    mainThread.join();
    projectorThread.join();
    cameraThread.join();
    */
	mainLoop();			
    return -1;
}

int mainLoop()
{
    // v maybe accessible to projector thread
    Bounce bounce = NOBOUNCE;
    Bounce previous_bounce = NOBOUNCE;

    // TODO: v must be accessible to projector thread
    uint8_t score_red = 0;
    uint8_t score_blue = 0;

    // TODO: location variables

    Button button = NOPRESS;

    std::string deviceStr = "/dev/ttyUSB0";
	UartDecoder uart = UartDecoder(deviceStr);

    auto start = clock_type::now();
    auto target = start + std::chrono::milliseconds(30);

    for(;;) {
        std::this_thread::sleep_until(target);
        target += std::chrono::milliseconds(30);

        uart.readSerial();

        // TODO: game processing
        // if(!uart.receivedError())
        bounce = uart.getBounce();
        button = uart.getButton();

        if(bounce != NOBOUNCE) {
            // start condition
            if(previous_bounce == NOBOUNCE)
                previous_bounce = bounce;
            else {
                // award points
                if(previous_bounce == bounce) {
                    if(bounce == RED)
                        score_blue += 1;
                    else
                        score_red += 1;

                    previous_bounce = NOBOUNCE;
                        
                } else {
                    previous_bounce = bounce;
                }
            }
        }

        if(button != NOPRESS) {
            // TODO: what should each button do?

            // numbering here is not tied to button numbers, just a counter
            // 1. increase red score
            // 2. decrease red score
            // 3. increase blue score
            // 4. decrease blue score
            // 5. reset screen (for gamemodes or sm)
            // 6. exit game (requires confirmation - second press)
            // sensitivity change buttons?
            // tell which side to serve?
            // ...
        }

        // else - error handling
    }

    return -1;
}

// TODO: move to projector file
int projectorLoop()
{
    return -1;
}

// TODO: move to camera file
int cameraLoop()
{
    return -1;
}
