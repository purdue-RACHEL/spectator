#ifndef INCLUDE_UART_DECODER
#define INCLUDE_UART_DECODER
// MESSAGE MASKS
#define BUTTON_PRESS_MASK 0x10
#define BUTTON_SHIFT 0
#define BUTTON_MASK 0x0F
#define BOUNCE_MASK 0x60
#define BOUNCE_SHIFT 5
#define NONE -1
#define ERROR_MASK 0x80
#define ERROR_SHIT 7

// LINUX TTY ASSOCIATED WITH SERIAL DEVICE
#define DEVICE "/dev/ttyUSB0"
#include <string.h>
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
class UartDecoder{
	public:
		enum Bounce curr_bounce;
		enum Button curr_press;
		int serial_port;

UartDecoder(std::string&);
int decode(unsigned char message);
void closePort();
};
#endif
