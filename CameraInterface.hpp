/*
 * James Hubbard
 * CameraInterface.hpp
 * Header file defining class CameraInterface
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <opencv2/features2d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/optflow.hpp>
#include <opencv2/video.hpp>
#include <OpenNI.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

class CameraInterface {
	public:
		CameraInterface();
		CameraInterface(openni::Device& device, openni::VideoStream& depth, openni::VideoStream& color);
		~CameraInterface();

		cv::Mat readDepth16U();
		cv::Mat readColor();

		openni::VideoFrameRef		m_depthFrame;
		openni::VideoFrameRef		m_colorFrame;
		openni::VideoMode		m_depthVideoMode;
		openni::VideoMode		m_colorVideoMode;
		
		openni::Device&			m_device;
		openni::VideoStream&		m_depthStream;
		openni::VideoStream&		m_colorStream;
		openni::VideoStream**		m_streams;
	private:
};


