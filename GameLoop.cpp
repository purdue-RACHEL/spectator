#include <chrono>
#include "GameLoop.hpp"
#include <iostream>
#include <string>
#include <thread>
#include "UartDecoder.hpp"

using clock_type = std::chrono::high_resolution_clock;

size_t score_red = 0;
size_t score_blue = 0;

int main()
{
    Bounce bounce = NOBOUNCE;

    Button button = NOPRESS;

    GameStatus gameStatus = ACTIVE;

    // TODO: location variables - multithreaded

    std::string deviceStr = "/dev/ttyUSB0";
	UartDecoder uart = UartDecoder(deviceStr);

    auto start = clock_type::now();
    auto target = start + std::chrono::milliseconds(30);

    while(gameStatus == ACTIVE) {
        std::this_thread::sleep_until(target);
        target += std::chrono::milliseconds(200);

        uart.readSerial();

        bounce = uart.getBounce();
        button = uart.getButton();

        if(handleBounce(bounce)) {
            std::cout << "red score: " << score_red << std::endl;
	        std::cout << "blue score: " << score_blue << std::endl;
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
                    std::cout << "red score: " << score_red << std::endl;
	                std::cout << "blue score: " << score_blue << std::endl;
                    break;
                case FOUR:
                    score_red -= 1;
                    std::cout << "red score: " << score_red << std::endl;
	                std::cout << "blue score: " << score_blue << std::endl;
                    break;
                
                case A:
                    score_blue += 1;
                    std::cout << "red score: " << score_red << std::endl;
	                std::cout << "blue score: " << score_blue << std::endl;
                    break;
                case B:
                    score_blue -= 1;
                    std::cout << "red score: " << score_red << std::endl;
	                std::cout << "blue score: " << score_blue << std::endl;
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

        // TODO: projecting

        // TODO: game finish logic - wins, ... mostly to be handled in projecting
    }

    uart.writeSerial(FORCE_SHUTDOWN);
    uart.closePort();

    return -1;
}

int handleBounce(Bounce bounce) {

    int change = 0;

    static Bounce previous_bounce = NOBOUNCE;
    static auto timeout = clock_type::now();
    static auto invalid_timeout = timeout;

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
                if(bounce == RED) {
                    score_blue += 1;
                    change = 1;
                } else {
                    score_red += 1;
                    change = 1;
                }

                previous_bounce = NOBOUNCE;
                // test:
                timeout = invalid_timeout;
                    
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
            if(previous_bounce == RED) {
                score_blue += 1;
                change = 1;
            } else if(previous_bounce == BLUE) {
                score_red += 1;
                change = 1;
            }
            timeout = invalid_timeout;
        }
    }

    return change;
}
