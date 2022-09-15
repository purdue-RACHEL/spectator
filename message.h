#ifndef INCLUDE_MESSAGE
#define INCLUDE_MESSAGE

// LINUX TTY ASSOCIATED WITH SERIAL DEVICE
#define DEVICE "/dev/ttyUSB0"

// ENUMS FOR IDENTIFYING MESSAGE SPECIFICS
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

// MESSAGE MASKS
#define BUTTON_PRESS_MASK 0x80
#define BUTTON_MASK 0x78
#define BOUNCE_MASK 0x06
#define NONE -1

int setupUart();
int decode(unsigned char message);

#endif