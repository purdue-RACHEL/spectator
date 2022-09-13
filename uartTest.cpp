#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

int main(int argc, char ** argv){
    struct termios tty;
    tty.c_cflag &= ~PARENB; //Parity Bit
    tty.c_cflag &= ~CSTOPB; //Stop Bit
    tty.c_cflag &= ~CSIZE; //Clear Size
    tty.c_cflag |= CS8; //Set Size
    tty.c_cflag &= ~CRTSCTS; //Disable Hardware Flow Control
    tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)
    tty.c_lflag &= ~ICANON; //Don't wait for \n
    tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
    tty.c_cc[VTIME] = 0;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
    tty.c_cc[VMIN] = 1;
    cfsetispeed(&tty, B115200);
    cfsetospeed(&tty, B115200);

    int serial_port = open("/dev/ttyUSB0", O_RDWR);
    if (serial_port < 0) {
        printf("Error %i from open: %s\n", errno, strerror(errno));
    }

    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
    }
    if(tcgetattr(serial_port, &tty) != 0) {
        printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
    }
    //char read_buf [256];
    //int n = read(serial_port, &read_buf, sizeof(read_buf));
    //printf("%c\n",read_buf[0]);
    //char sendByte;
    //sendByte = 'a'; 
    //write(serial_port, &sendByte, 1);
    close(serial_port);
}