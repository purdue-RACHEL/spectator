#include <thread>

#include "CameraLoop.hpp"
#include "GameLoop.hpp"

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
    

    // 4. Start Threads
    std::thread mainThread(gameLoop);
    std::thread cameraThread(cameraLoop);

    // 5. Handle Shutdown
    // TODO:
    mainThread.join();
    cameraThread.join();
		
    return 0;
}
