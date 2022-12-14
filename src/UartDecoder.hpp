#ifndef INCLUDE_UART_DECODER
#define INCLUDE_UART_DECODER

// Includes
#include <string>

// Defines
#define BUTTON_PRESS_MASK 0x10
#define BUTTON_SHIFT 0
#define BUTTON_MASK 0x0F
#define BOUNCE_MASK 0x60
#define BOUNCE_SHIFT 5
#define ERROR_MASK 0x80
#define ERROR_SHIFT 7

// Type Declarations

// Struct Declarations

// Enum Declarations
enum Button {
    NOPRESS = -1,
    ZERO,
    ONE,
    TWO,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT,
    NINE,
    A,
    B,
    C,
    D,
    POUND,
    STAR,
};

enum Command {
    REQUEST_DATA = 1,
    FORCE_RESET,
    FORCE_SHUTDOWN,
};

enum Bounce {
    NOBOUNCE = -1,
    BLUE=1,
    RED,
};

// Class Declarations
class UartDecoder {
	public:
		int curr_bounce;
		int curr_press;
		int serial_port;
		//Time of last uart read
		//Miliseconds past the epoch
		unsigned int last_time;
		//Time of current uart read
		//Miliseconds past the epoch
		unsigned int curr_time;

// Functions
UartDecoder(std::string&);
int decode(unsigned char message);
enum Button getButton();
enum Bounce getBounce();
int readSerial();
void writeSerial(char send_buf);
void closePort();
};

#endif
