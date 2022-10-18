/* James Hubbard
 * CameraInterface.cpp
 * Implementation of CameraInterface.hpp
 */

#include "CameraInterface.hpp"

CameraInterface::~CameraInterface() {}

CameraInterface::CameraInterface(openni::Device& device, openni::VideoStream& depth, openni::VideoStream& color) : 
	m_device(device),
	m_depthStream(depth),
	m_colorStream(color),
	m_depthFrame(openni::VideoFrameRef()),
	m_colorFrame(openni::VideoFrameRef()),
	m_depthVideoMode(depth.getVideoMode()),
	m_colorVideoMode(color.getVideoMode()) {
}

CameraInterface CameraInterface::autoCameraInterface() {
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
	}
	
	openni::Device device;
	
	rc = device.open(openni::ANY_DEVICE);
	if (rc != openni::STATUS_OK) {
		printf("failed to open device\n");
	}
#ifdef DEBUG
	printf("After device opened:\n");
#endif /* DEBUG */

	openni::VideoStream depth, color;
	rc = depth.create(device, openni::SENSOR_DEPTH);
	if (rc != openni::STATUS_OK) {
		printf("Couldn't find depth stream\n");
	} else {
		rc = depth.start();
		if (rc != openni::STATUS_OK) {
			printf("Couldn't start depth stream\n");
			depth.destroy();
		}
	}
#ifdef DEBUG
	printf("Depth stream started\n");
#endif /* DEBUG */

	rc = color.create(device, openni::SENSOR_COLOR);
	if (rc != openni::STATUS_OK) {
		printf("Couldn't find color stream\n");
	} else {
		rc = color.start();
		if (rc != openni::STATUS_OK) {
			printf("Couldn't start depth stream\n");
			color.destroy();
		}
	}
#ifdef DEBUG
	printf("Color stream started\n");
#endif /* DEBUG */

	return CameraInterface(device, depth, color)

}

cv::Mat CameraInterface::readDepth16U() {
	m_depthStream.readFrame(&m_depthFrame);
	size_t width = m_depthFrame.getWidth();
	size_t height = m_depthFrame.getHeight();
	size_t depthSize = width * height;
	cv::Mat depthMat = cv::Mat(m_depthFrame.getHeight(), m_depthFrame.getWidth(), CV_16UC1, (void *) m_depthFrame.getData());
	depthMat = depthMat.clone();
	cv::Mat outMat(height, width, CV_16UC1);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			outMat.at<uint16_t>(y, width - x) = (depthMat.at<uint16_t>(y, x));
		}
	}
	return outMat;
}

cv::Mat CameraInterface::readColor() {
	m_colorStream.readFrame(&m_colorFrame);
	size_t colorByteSize = 3 * m_colorFrame.getWidth() * m_colorFrame.getHeight();
	cv::Mat colorMat = cv::Mat(m_colorFrame.getHeight(), m_colorFrame.getWidth(), CV_8UC3, (void *) m_colorFrame.getData());
	colorMat = colorMat.clone();
	return colorMat;
}

