#ifndef INCLUDE_UART_DECODER
#define INCLUDE_UART_DECODER
// MESSAGE MASKS
#define BUTTON_PRESS_MASK 0x10
#define BUTTON_SHIFT 0
#define BUTTON_MASK 0x0F
#define BOUNCE_MASK 0x60
#define BOUNCE_SHIFT 5
#define NONE -1

// LINUX TTY ASSOCIATED WITH SERIAL DEVICE
#define DEVICE "/dev/ttyUSB0"
#include <string.h>
class UartDecoder{
	public:
		int curr_bounce;
		int curr_press;
		int serial_port;
enum Button{
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
    E,
    F,
};

enum Bounce{
    LEFT=1,
    RIGHT,
};

UartDecoder(std::string&);
int decode(unsigned char message);
void closePort();
};
#endif
