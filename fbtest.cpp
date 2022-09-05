/*
 * James Hubbard
 * fbtest.cpp
 * Reimplementation of oldfbtest.c in cpp to facilitate
 * incremental testing and integration with OpenNI2
 */

#define DEBUG

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <linux/fb.h>
#include <OpenNI.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

enum DisplayMode {
	DISPLAY_MODE_COLOR,
	DISPLAY_MODE_DEPTH
};

class testviewer {
	public:
		testviewer(openni::Device& device, openni::VideoStream& depth, openni::VideoStream& color);
		~testviewer();

		struct fb_var_screeninfo	var_info;
		struct fb_fix_screeninfo	fix_info;
		char*				fbp; //framebuffer pointer
		struct fb_var_screeninfo	orig_var_info;
		int				fbfd; //framebuffer file descriptor
		int				m_width;
		int				m_height;

		openni::VideoFrameRef		m_depthFrame;
		openni::VideoFrameRef		m_colorFrame;
		openni::VideoMode		m_depthVideoMode;
		openni::VideoMode		m_colorVideoMode;
		
		openni::Device&			m_device;
		openni::VideoStream&		m_depthStream;
		openni::VideoStream&		m_colorStream;
		openni::VideoStream**		m_streams;

		DisplayMode			m_displayMode;
	private:
};

testviewer::testviewer(openni::Device& device, openni::VideoStream& depth, openni::VideoStream& color) : 
	fbfd(0),
	m_device(device),
	m_depthStream(depth),
	m_colorStream(color)
{
}

testviewer::~testviewer() {
	munmap(fbp, fix_info.smem_len);
	if (ioctl(fbfd, FBIOPUT_VSCREENINFO, &orig_var_info)) {
		printf("Couldn't revert variable screen info.");
	}
	close(fbfd);
}

int main(int argc, char** argv) {
#ifdef DEBUG
	printf("Entry point.\n");
#endif /* DEBUG */

	openni::Status rc = openni::STATUS_OK;
	rc = openni::OpenNI::initialize();
#ifdef DEBUG
	printf("After initialization:\n");
#endif /* DEBUG */
	if (rc != openni::STATUS_OK) {
		printf("Couldn't initialize.\n%s\n", openni::OpenNI::getExtendedError());
		return EXIT_FAILURE;
	}
	
	openni::Device device;
	
	rc = device.open(openni::ANY_DEVICE);
	if (rc != openni::STATUS_OK) {
		printf("failed to open device\n");
		return rc;
	}
#ifdef DEBUG
	printf("After device opened:\n");
#endif /* DEBUG */

	openni::VideoStream depth, color;
	rc = depth.create(device, openni::SENSOR_DEPTH);
	if (rc != openni::STATUS_OK) {
		printf("Couldn't find depth stream\n");
		return rc;
	} else {
		rc = depth.start();
		if (rc != openni::STATUS_OK) {
			printf("Couldn't start depth stream\n");
			depth.destroy();
			return rc;
		}
	}
#ifdef DEBUG
	printf("Depth stream started\n");
#endif /* DEBUG */

	testviewer viewer = testviewer(device, depth, color);
#ifdef DEBUG
	printf("viewer object created\n");
#endif /* DEBUG */
	viewer.fbfd = open("/dev/fb0", O_RDWR);
	if (!viewer.fbfd) {
		printf("Couldn't open fb.\n");
		return EXIT_FAILURE;
	}
#ifdef DEBUG
	printf("fb opened successfully.\n");
#endif /* DEBUG */

	if (ioctl(viewer.fbfd, FBIOGET_VSCREENINFO, &(viewer.var_info))) {
		printf("Couldn't get variable screen info.\n");
		return EXIT_FAILURE;
	}
#ifdef DEBUG
	printf("Retrieved variable screen info.\n");
	printf("Resolution: X: %d Y: %d\n", viewer.var_info.xres, viewer.var_info.yres);
	printf("BPP: %d\n", viewer.var_info.bits_per_pixel);
#endif /* DEBUG */

	memcpy(&(viewer.orig_var_info), &(viewer.var_info), sizeof(struct fb_var_screeninfo));

	viewer.var_info.bits_per_pixel = 16;
	if(ioctl(viewer.fbfd, FBIOPUT_VSCREENINFO, &(viewer.var_info))) {
		printf("Couldn't set variable screen info.\n");
		return EXIT_FAILURE;
	}
#ifdef DEBUG
	printf("Successfully set variable screen info.\n");
#endif /* DEBUG */

	if (ioctl(viewer.fbfd, FBIOGET_FSCREENINFO, &(viewer.fix_info))) {
		printf("Couldn't get fixed screen info.\n");
		return EXIT_FAILURE;
	}
#ifdef DEBUG
	printf("Successfully retrieved fixed screen info.\n");
	printf("smem_length: %llib\n", viewer.fix_info.smem_len);
#endif /* DEBUG */

	viewer.fbp = (char*) mmap(0, viewer.fix_info.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, viewer.fbfd, 0);
	if ((int) viewer.fbp == -1) {
		printf("Couldn't map fb memory to userspace.");
		return EXIT_FAILURE;
	}
#ifdef DEBUG
	printf("Mapped fb to user memory.\n");
#endif /* DEBUG */

	printf("No errors!\n");
	return EXIT_SUCCESS;
}
