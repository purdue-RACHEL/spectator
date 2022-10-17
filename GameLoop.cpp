#include <chrono>
#include "GameLoop.hpp"
#include <string>
#include <thread>
#include "UartDecoder.hpp"

using clock_type = std::chrono::high_resolution_clock;

int gameLoop()
{
    Bounce bounce = NOBOUNCE;
    Bounce previous_bounce = NOBOUNCE;

    Button button = NOPRESS;

    uint8_t score_red = 0;
    uint8_t score_blue = 0;

    GameStatus gameStatus = ACTIVE;

    // TODO: location variables - multithreaded

    std::string deviceStr = "/dev/ttyUSB0";
	UartDecoder uart = UartDecoder(deviceStr);

    auto start = clock_type::now();
    auto invalid_timeout = start;
    auto target = start + std::chrono::milliseconds(30);
    auto timeout = invalid_timeout;

    while(gameStatus == ACTIVE) {
        std::this_thread::sleep_until(target);
        target += std::chrono::milliseconds(30);

        uart.readSerial();

        bounce = uart.getBounce();
        button = uart.getButton();

        /********************
         * Bounce Logic
         *******************/

        if(bounce != NOBOUNCE) {

            // serve condition
            if(previous_bounce == NOBOUNCE) {
                // TODO: any necessary serve logic
                previous_bounce = bounce;
                // test:
                timeout = clock_type::now() + std::chrono::milliseconds(BOUNCE_TIMEOUT_MS);
            } else {

                // award points
                if(previous_bounce == bounce) {
                    if(bounce == RED)
                        score_blue += 1;
                    else
                        score_red += 1;

                    previous_bounce = NOBOUNCE;
                    // test:
                    timeout = start;
                        
                // game continuation
                } else {
                    previous_bounce = bounce;
                    // test:
                    timeout = clock_type::now() + std::chrono::milliseconds(BOUNCE_TIMEOUT_MS);
                }
            }
        }

        if(timeout != invalid_timeout) {
            // bounce timed out
            if(clock_type::now() + std::chrono::milliseconds(0) > timeout) {
                if(previous_bounce == RED)
                    score_blue += 1;
                else
                    score_red += 1;
                timeout = invalid_timeout;
            }
        }

        /********************
         * Button Logic
         *******************/

        if(button != NOPRESS) {
            // TODO: what should each button do?
            // We should refactor the Button enum to refer to the specific actions

            switch(button) {
                case ONE:
                    score_red += 1;
                    break;
                case FOUR:
                    score_red -= 1;
                    break;
                
                case A:
                    score_blue += 1;
                    break;
                case B:
                    score_blue -= 1;
                    break;

                case D:
                    gameStatus = SHUTDOWN;
                    break;

            }

            // 1. increase red score
            // 4. decrease red score
            // a. increase blue score
            // b. decrease blue score

            // d. exit game (require second press - confirmation TODO: mem logic)

            // numbering here is not tied to button numbers, just a counter
            // 5. reset screen (for gamemodes or sm)
            // sensitivity change buttons?
            // tell which side to serve?
            // ...
        }

        // else - error handling
    }

    uart.closePort();

    return -1;
}
