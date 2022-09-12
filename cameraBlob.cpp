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
#include <OpenNI.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

class CameraInterface {
	public:
		CameraInterface(openni::Device& device, openni::VideoStream& depth, openni::VideoStream& color);
		~CameraInterface();

		void readImage();

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

void CameraInterface::readImage() {
#ifdef DEBUG_DRAW
	printf("In detect():\n");
#endif /* DEBUG_DRAW */
	m_depthStream.readFrame(&m_depthFrame);
#ifdef DEBUG_DRAW
	printf("Read depth frame\n");
#endif /* DEBUG_DRAW */
	//m_colorStream.readFrame(&m_colorFrame);
	size_t depthByteSize = m_depthFrame.getWidth() * m_depthFrame.getHeight();
	//size_t colorByteSize = 3 * m_colorFrame.getWidth() * m_colorFrame.getHeight();


 
	cv::Mat depthMat = cv::Mat(m_depthFrame.getHeight(), m_depthFrame.getWidth(), CV_8UC1);
	std::memcpy(depthMat.data, (uint8_t *) m_depthFrame.getData(), depthByteSize);
	//cv::Mat colorMat = cv::Mat(m_colorFrame.getHeight(), m_colorFrame.getWidth(), CV_8UC3);
	//std::memcpy(colorMat.data, (uint8_t *) m_colorFrame.getData(), colorByteSize);
	
	//cv::namedWindow("color");
	//cv::imshow("color", colorMat);
	//cv::waitKey(0);
	cv::namedWindow("depth");
	cv::imshow("depth", depthMat);
	cv::waitKey(0);
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

	while (1) cam.readImage();
	printf("No errors!\n");
	return EXIT_SUCCESS;
}
