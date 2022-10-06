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
