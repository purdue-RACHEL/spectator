//fbtest.c
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <stdint.h>

//Global screen info
struct fb_var_screeninfo var_info;
struct fb_fix_screeninfo fix_info;
char *fbp = 0; //framebuffer pointer

//help function
void put_pixel(int x, int y, int c) {
	//pixel offset in fb 
	unsigned int pix_off = x + (y * fix_info.line_length);
	fbp[pix_off] = c;
}

void put_pixel_RGB565(int x, int y, char r, char g, char b) {
	//every pixel is two cosnecutive bytes with 5 bits dedicated to R, 6 to G, and 5 to B
	unsigned int pix_off = (x * 2) + y * fix_info.line_length;
	unsigned short c = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
	*((unsigned short *) (fbp + pix_off)) = c;
}

void put_pixel_RGB24(int x, int y, char r, char g, char b) {
	unsigned int pix_off = (x * 3) + y * fix_info.line_length;
	*((uint8_t*) (fbp + pix_off + 0)) = r;
	*((uint8_t*) (fbp + pix_off + 1)) = g;
	*((uint8_t*) (fbp + pix_off + 2)) = b;
}

void draw() {
	int x, y;
	static size_t count = 0;
	char r, g, b;
	for (y = 0; y < var_info.yres; y++) {
		for (x = 0; x < var_info.xres; x++) {
				r = x % 256;
				g = y % 256;
				b = count;
				put_pixel_RGB565(x, y, r, g, b);
		}
	}
	count = ++count % 256;
}

int main (int argc, char **argv) {
	int fbfd = 0; //framebuffer file descriptor
	struct fb_var_screeninfo var_info_orig;
	long int screensize = 0;

	//Open the fb device file for r/w
	fbfd = open("/dev/fb0", O_RDWR);
	if (!fbfd) {
		printf("Can't open framebuffer device.\n");
		return EXIT_FAILURE;
	}
	printf("The framebuffer device is open.\n");

	//Get variable screen info
	if (ioctl(fbfd, FBIOGET_VSCREENINFO, &var_info)) {
		printf("Can't read variable screen information.\n");
	}
	printf("Display info: %dx%d pix., %d bits/pixel\n", var_info.xres, var_info.yres, var_info.bits_per_pixel);

	//store original screeninfo for later
	memcpy(&var_info_orig, &var_info, sizeof(struct fb_var_screeninfo));

	//This is where you would change the variable screen info if you wanted!!
	var_info.bits_per_pixel = 16;
	if(ioctl(fbfd, FBIOPUT_VSCREENINFO, &var_info)) {
		printf("Error setting variable screeninfo.\n");
	}

	//Get fixed screen info
	if (ioctl(fbfd, FBIOGET_FSCREENINFO, &fix_info)) {
		printf("Can't read fixed screen information.\n");
	}

	//map fb to user memory
	screensize = fix_info.smem_len;
	fbp = (char*) mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
	if ((int) fbp == -1) printf("Failed to map framebuffer to user memory.\n");
	else {
		while (1) draw();
		sleep(5);
	}

	//cleanup
	munmap(fbp, screensize);
	if(ioctl(fbfd, FBIOPUT_VSCREENINFO, &var_info_orig)) printf("Error re-setting variable screeninfo.\n");
	close(fbfd);

	return EXIT_SUCCESS;
}


