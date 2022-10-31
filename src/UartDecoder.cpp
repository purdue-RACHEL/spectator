#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <termios.h>
#include <unistd.h>

#include "UartDecoder.hpp"

using namespace std;
#ifdef TESTUART
// TEST MAIN FOR DEBUGGING
int main(int argc, char ** argv){
	string deviceStr = "/dev/ttyUSB0";
	UartDecoder uart = UartDecoder(deviceStr);
    if(uart.serial_port == 0){
        cout << "Problem Setting Up Serial Port" << endl;
        return 1;
    }
    int messagei = 0;
    for(;;){
        uart.readSerial();
	printf("New Message %d\n",messagei);
        printf("Current Button: %d\n", uart.getButton());
        printf("Current Bounce: %d\n", uart.getBounce());
	messagei++;
    }


    return 0;
}
#endif


// SETUP UART CONNECTION WITH CORRECT PARAMETERS
UartDecoder::UartDecoder(string& deviceName){
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
    char arr[deviceName.length() + 1];
    strcpy(arr, deviceName.c_str());
    serial_port = open(arr, O_RDWR);
    if (serial_port < 0) {
        printf("Error %i from open: %s\n", errno, strerror(errno));
	    return;
    }

    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        return;
    }
    if(tcgetattr(serial_port, &tty) != 0) {
        printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
        return;
    }
}

// DECODE MESSAGE AND SET GLOBAL VARIABLES
int UartDecoder::decode(unsigned char message){
    UartDecoder& decoder= *this;
    int temp_press;
    int temp_bounce;
    if(!(message & BUTTON_PRESS_MASK)){
	    temp_press = NOPRESS;
    }else{
	    temp_press = (message & BUTTON_MASK) >> BUTTON_SHIFT;
    }
    unsigned char bounce_message = (message & BOUNCE_MASK);
    if((bounce_message == 0)||(bounce_message == 3)){
	    temp_bounce = NOBOUNCE;
    }else{
	    temp_bounce = (message & BOUNCE_MASK) >> BOUNCE_SHIFT;
    }
    if(message & ERROR_MASK){
	    if(temp_press != NOPRESS){
		    decoder.curr_press = temp_press;
	    }
        return 1;
    }
    decoder.curr_press = temp_press;
    decoder.curr_bounce = temp_bounce;
    return 0;
}

enum Bounce UartDecoder::getBounce(){
    UartDecoder& decoder= *this;
    return static_cast<Bounce>(decoder.curr_bounce);
}

enum Button UartDecoder::getButton(){
    UartDecoder& decoder = *this;
    return static_cast<Button>(decoder.curr_press);
}

int UartDecoder::readSerial(){
    UartDecoder& decoder = *this;
    char send_buf = REQUEST_DATA;
    write(decoder.serial_port, &send_buf, sizeof(send_buf));
    char read_buf;
    int n = read(decoder.serial_port, &read_buf, sizeof(read_buf));
    if(n ==0 ){
        return 1;
    }
    if(decoder.decode(read_buf)){
        return 1;
    }
    return 0;
}

void UartDecoder::writeSerial(char send_buf){
    UartDecoder& decoder = *this;
    write(decoder.serial_port, &send_buf, sizeof(send_buf));
}

void UartDecoder::closePort(){
	close(serial_port);
}
