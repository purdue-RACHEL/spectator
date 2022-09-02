//fbviewer.cpp

#include <linux/fb.h>
#include "fbviewer.h"


#define DEFAULT_DISPLAY_MODE	DISPLAY_MODE_DEPTH

fbviewer* fbviewer::ms_self = NULL;

fbviewer::fbviewer(const char* strSampleName, openni::Device& device, openni::VideoStream& depth, openni::VideoStream& color) :
	m_device(device), m_depthStream(depth), m_colorStream(color), m_streams(NULL), m_eViewState(DEFAULT_DISPLAY_MODE), m_pTexMap(NULL)
{
	ms_self = this;
	strncpy(m_strSampleName, strSampleName, ONI_MAX_STR - 1);
}

fbviewer::~fbviewer()
{
	delete[] m_pTexMap;

	ms_self = NULL;

	if (m_streams != NULL)
	{
		delete []m_streams;
	}
}

openni::Status fbviewer::init(int argc, char **argv)
{
	openni::VideoMode depthVideoMode;
	openni::VideoMode colorVideoMode;

	if (m_depthStream.isValid() && m_colorStream.isValid())
	{
		depthVideoMode = m_depthStream.getVideoMode();
		colorVideoMode = m_colorStream.getVideoMode();

		int depthWidth = depthVideoMode.getResolutionX();
		int depthHeight = depthVideoMode.getResolutionY();
		int colorWidth = colorVideoMode.getResolutionX();
		int colorHeight = colorVideoMode.getResolutionY();

		m_width = depthWidth;
		m_height = depthHeight;
	}
	else if (m_depthStream.isValid())
	{
		depthVideoMode = m_depthStream.getVideoMode();
		m_width = depthVideoMode.getResolutionX();
		m_height = depthVideoMode.getResolutionY();
	}
	else if (m_colorStream.isValid())
	{
		colorVideoMode = m_colorStream.getVideoMode();
		m_width = colorVideoMode.getResolutionX();
		m_height = colorVideoMode.getResolutionY();
	}
	else
	{
		printf("Error - expects at least one of the streams to be valid...\n");
		return openni::STATUS_ERROR;
	}

	m_streams = new openni::VideoStream*[2];
	m_streams[0] = &m_depthStream;
	m_streams[1] = &m_colorStream;

	/* RAW INITIALIZATION AND CLEANUP CODE FROM fbtest.c !!!
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
	var_info.bits_per_pixel = 8;
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
		draw();
		sleep(5);
	}

	//cleanup
	munmap(fbp, screensize);
	if(ioctl(fbfd, FBIOPUT_VSCREENINFO, &var_info_orig)) printf("Error re-setting variable screeninfo.\n");
	close(fbfd);
	*/
	
	return openni::STATUS_OK;
}

openni::Status fbviewer::draw()
{
	
	
	return openni::STATUS_OK;
}
