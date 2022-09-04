//fbviewer.h
#ifndef _FBVIEWER_H_
#define _FBVIEWER_H_

#include <linux/fb.h>
#include <OpenNI.h>

#define MAX_DEPTH 10000

enum DisplayModes
{
	DISPLAY_MODE_OVERLAY,
	DISPLAY_MODE_DEPTH,
	DISPLAY_MODE_IMAGE
};

class fbviewer
{
public:
	fbviewer(const char* strSampleName, openni::Device& device, openni::VideoStream& depth, openni::VideoStream& color);
	virtual ~fbviewer();

	virtual openni::Status init(int argc, char **argv);
	virtual openni::Status run();	//Does not return

protected:
	openni::VideoFrameRef		m_depthFrame;
	openni::VideoFrameRef		m_colorFrame;

	openni::Device&			m_device;
	openni::VideoStream&		m_depthStream;
	openni::VideoStream&		m_colorStream;
	openni::VideoStream**		m_streams;

	struct fb_var_screeninfo	var_info;
	struct fb_fix_screeninfo	fix_info;
	char*				fbp; //framebuffer pointer
	int 				fbfd = 0; //framebuffer file descriptor
	struct fb_var_screeninfo 	var_info_orig;

private:
	fbviewer(const fbviewer&);
	fbviewer& operator=(fbviewer&);

	static fbviewer* ms_self;

	char			m_strSampleName[ONI_MAX_STR];
	DisplayModes		m_eViewState;
	int			m_width;
	int			m_height;
};


#endif // _ONI_SAMPLE_VIEWER_H_
