/*
 * James Hubbard
 * fbtest.cpp
 * Reimplementation of oldfbtest.c in cpp to facilitate
 * incremental testing and integration with OpenNI2
 */

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
		testviewer();
		~testviewer();

		openni::Status getVideoFeeds(const char* deviceURI);

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

testviewer::testviewer() : 
	fbfd(0) 
{
}

testviewer::~testviewer() {
	munmap(fbp, fix_info.smem_len);
	if (ioctl(fbfd, FBIOPUT_VSCREENINFO, &orig_var_info)) {
		printf("Couldn't revert variable screen info.");
	}
	close(fbfd);
}

openni::Status testviewer::getVideoFeeds(const char* deviceURI) {
	openni::Status rc = openni::STATUS_OK;

	rc = m_device.open(deviceURI);
	if (rc != openni::STATUS_OK) {
		printf("failed to open device\n");
		return rc;
	}

	rc = m_depthStream.create(m_device, openni::SENSOR_DEPTH);
	if (rc != openni::STATUS_OK) {
		printf("Couldn't find depth stream\n");
		return rc;
	} else {
		rc = m_depthStream.start();
		if (rc != openni::STATUS_OK) {
			printf("Couldn't start depth stream\n");
			m_depthStream.destroy();
			return rc;
		}
	}

	return rc;

}

int main(int argc, char** argv) {
	testviewer viewer = testviewer();
	viewer.fbfd = open("/dev/fb0", O_RDWR);
	if (!viewer.fbfd) {
		printf("Couldn't open fb.");
		return EXIT_FAILURE;
	}

	if (ioctl(viewer.fbfd, FBIOGET_VSCREENINFO, &(viewer.var_info))) {
		printf("COuldn't get variable screen info.");
		return EXIT_FAILURE;
	}

	if (ioctl(viewer.fbfd, FBIOGET_FSCREENINFO, &(viewer.fix_info))) {
		printf("Couldn't get fixed screen info.");
		return EXIT_FAILURE;
	}

	memcpy(&(viewer.orig_var_info), &(viewer.var_info), sizeof(struct fb_var_screeninfo));

	viewer.var_info.bits_per_pixel = 16;
	if(ioctl(viewer.fbfd, FBIOPUT_VSCREENINFO, &(viewer.var_info))) {
		printf("Couldn't set variable screen info.");
		return EXIT_FAILURE;
	}

	viewer.fbp = (char*) mmap(0, viewer.fix_info.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, viewer.fbfd, 0);
	if ((int) viewer.fbp == -1) {
		printf("Couldn't map fb memory to userspace.");
		return EXIT_FAILURE;
	}

	openni::Status rc = openni::STATUS_OK;
	rc = openni::OpenNI::initialize();
	if (rc != openni::STATUS_OK) {
		printf("Couldn't initialize.\n%s\n", openni::OpenNI::getExtendedError());
		return EXIT_FAILURE;
	}
	rc = viewer.getVideoFeeds(openni::ANY_DEVICE);
	if (rc != openni::STATUS_OK) {
		printf("Couldn't get video feeds.\n%s\n", openni::OpenNI::getExtendedError());
		return EXIT_FAILURE;
	}

	printf("No errors!\n");
	return EXIT_SUCCESS;
}
