#include "mainLoop.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <iostream>


using namespace std;

// GLOBAL VARIABLES
int curr_press;
int curr_bounce;

// TEST MAIN FOR DEBUGGING
int main(int argc, char ** argv){
    int serial_port = setupUart();
    if(serial_port == 0){
        cout << "Problem Setting Up Serial Port" << endl;
        return 1;
    }
    if(decode(0xFC)){
        cerr << "Decode Message Failed" << endl;
        return 1;
    }
    cout << "BUTTON PRESSED: " << curr_press << endl;
    cout << "BALL BOUNCED: " << curr_bounce << endl;
    close(serial_port);
    
}

// SETUP UART CONNECTION WITH CORRECT PARAMETERS
int setupUart(){
    struct termios tty;
    tty.c_cflag &= ~PARENB; // Parity Bit
    tty.c_cflag &= ~CSTOPB; // Stop Bit
    tty.c_cflag &= ~CSIZE; // Clear Size
    tty.c_cflag |= CS8; // Set Size
    tty.c_cflag &= ~CRTSCTS; // Disable Hardware Flow Control
    tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)
    tty.c_lflag &= ~ICANON; // Don't wait for \n
    tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
    // Change VTIME and VMIN to adjust main behavior
    tty.c_cc[VTIME] = 0; // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
    tty.c_cc[VMIN] = 1;
    cfsetispeed(&tty, B115200);
    cfsetospeed(&tty, B115200);

    // Catch errors
    int serial_port = open(DEVICE, O_RDWR);
    if (serial_port < 0) {
        printf("Error %i from open: %s\n", errno, strerror(errno));
        return 0;
    }

    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        return 0;
    }
    if(tcgetattr(serial_port, &tty) != 0) {
        printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
        return 0;
    }
    return serial_port;  
}

// DECODE MESSAGE AND SET GLOBAL VARIABLES
int decode(unsigned char message){
    if(!(message & BUTTON_PRESS_MASK)){
        curr_press = NONE;
    }else{
        curr_press = (message & BUTTON_MASK) >> 3;
    }
    unsigned char bounce_message = (message & BOUNCE_MASK);
    if((bounce_message == 0)||(bounce_message == 3)){
        curr_bounce = NONE;
    }else{
        curr_bounce = (message & BOUNCE_MASK) >> 1;
    }
    return 0;
}
