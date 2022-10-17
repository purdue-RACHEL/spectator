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

    // TODO: location variables - multithreaded

    std::string deviceStr = "/dev/ttyUSB0";
	UartDecoder uart = UartDecoder(deviceStr);

    auto start = clock_type::now();
    auto invalid_timeout = start;
    auto target = start + std::chrono::milliseconds(30);
    auto timeout = invalid_timeout;

    for(;;) {
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

    uart.closePort();

    return -1;
}
