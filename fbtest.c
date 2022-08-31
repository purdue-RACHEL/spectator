#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>

int main (int argc, char **argv) {
	int fbfd = 0; //framebuffer file descriptor
	struct fb_var_screeninfo var_info;

	//Open the fb device file for r/w
	fbfd = open("/dev/fb0", O_RDWR);
	if (fbfd == -1) {
		printf("Can't open framebuffer device.\n");
		return EXIT_FAILURE;
	}
	printf("The framebuffer device is open.\n");

	//Get variable screen info
	if (ioctl(fbfd, FBIOGET_VSCREENINFO, &var_info)) {
		printf("Can't read variable screen information.\n");
	}
	printf("Display info: %dx%d pix., %d bits/pixel\n", var_info.xres, var_info.yres, var_info.bits_per_pixel);

	//close framebuffer device file
	close(fbfd);

	return EXIT_SUCCESS;
}
