/*
 * James Hubbard
 * fbtest.cpp
 * Reimplementation of oldfbtest.c in cpp to facilitate
 * incremental testing and integration with OpenNI2
 */

// Defines that print stuff
#define DEBUG
#define DEBUG_DRAW

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <linux/fb.h>
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
		CameraInterface(openni::Device& device, openni::VideoStream& depth, openni::VideoStream& color);
		~CameraInterface();

		cv::Mat readDepth();
		cv::Mat readDepth16U();
		cv::Mat readColor();
		void drawRLOF();

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

CameraInterface::CameraInterface(openni::Device& device, openni::VideoStream& depth, openni::VideoStream& color) : 
	m_device(device),
	m_depthStream(depth),
	m_colorStream(color),
	m_depthFrame(openni::VideoFrameRef()),
	m_colorFrame(openni::VideoFrameRef()),
	m_depthVideoMode(depth.getVideoMode()),
	m_colorVideoMode(color.getVideoMode())
{
}

CameraInterface::~CameraInterface() {
}

cv::Mat CameraInterface::readDepth() {
	m_depthStream.readFrame(&m_depthFrame);
	size_t width = m_depthFrame.getWidth();
	size_t height = m_depthFrame.getHeight();
	size_t depthSize = width * height;
	cv::Mat depthMat = cv::Mat(m_depthFrame.getHeight(), m_depthFrame.getWidth(), CV_16UC1, (void *) m_depthFrame.getData());
	depthMat = depthMat.clone();
	cv::Mat outMat(height, width, CV_8UC1);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			outMat.at<uint8_t>(y, width - x) = (depthMat.at<uint16_t>(y, x) & 0xff00) >> 8;
		}
	}
	return outMat;
}

cv::Mat CameraInterface::readDepth16U() {
	m_depthStream.readFrame(&m_depthFrame);
	size_t width = m_depthFrame.getWidth();
	size_t height = m_depthFrame.getHeight();
	size_t depthSize = width * height;
	cv::Mat depthMat = cv::Mat(m_depthFrame.getHeight(), m_depthFrame.getWidth(), CV_16UC1, (void *) m_depthFrame.getData());
	depthMat = depthMat.clone();
	return depthMat;
}


cv::Mat CameraInterface::readColor() {
	m_colorStream.readFrame(&m_colorFrame);
	size_t colorByteSize = 3 * m_colorFrame.getWidth() * m_colorFrame.getHeight();
	cv::Mat colorMat = cv::Mat(m_colorFrame.getHeight(), m_colorFrame.getWidth(), CV_8UC3, (void *) m_colorFrame.getData());
	colorMat = colorMat.clone();
	return colorMat;
}

/*
void CameraInterface::drawRLOF() {
	cv::Mat frame_now, frame_past, outImage, flowMap;
	std::vector<cv::Point> feat_now, feat_past;

	frame_past = cv::cvtColor(readDepth(), 
	frame_now = readDepth();

	cv::goodFeaturesToTrack ();

	while (1) {
		
	}
}
*/

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

	rc = color.create(device, openni::SENSOR_COLOR);
	if (rc != openni::STATUS_OK) {
		printf("Couldn't find color stream\n");
		return rc;
	} else {
		rc = color.start();
		if (rc != openni::STATUS_OK) {
			printf("Couldn't start depth stream\n");
			color.destroy();
			return rc;
		}
	}
#ifdef DEBUG
	printf("Color stream started\n");
#endif /* DEBUG */

	CameraInterface cam = CameraInterface(device, depth, color);
#ifdef DEBUG
	printf("viewer object created\n");
#endif /* DEBUG */

	//cam.drawRLOF(thisFrame, lastFrame); //doesn't return

	for (;;) {
		cv::Mat eightbit = cam.readDepth();
		cv::Mat equalized, eq_color;
		cv::equalizeHist(eightbit, equalized);
		cv::applyColorMap(equalized, eq_color, cv::COLORMAP_AUTUMN);
		cv::imshow("8bit depth", eightbit);
		cv::imshow("equalized", equalized);
		cv::imshow("colorized", eq_color);
		cv::waitKey(0);
	}
	printf("No errors!\n");
	return EXIT_SUCCESS;
}
