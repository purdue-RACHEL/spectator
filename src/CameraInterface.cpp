/* James Hubbard
 * CameraInterface.cpp
 * Implementation of CameraInterface.hpp
 */

#include "CameraInterface.hpp"

CameraInterface::~CameraInterface() {
}

/*
CameraInterface::CameraInterface(openni::Device& device, openni::VideoStream& depth, openni::VideoStream& color) : 
	m_device(device),
	m_depthStream(depth),
	m_colorStream(color),
	m_depthFrame(openni::VideoFrameRef()),
	m_colorFrame(openni::VideoFrameRef()),
	m_depthVideoMode(depth.getVideoMode()),
	m_colorVideoMode(color.getVideoMode()) {
}
*/

CameraInterface::CameraInterface() : 
	m_depthFrame(openni::VideoFrameRef()),
	m_colorFrame(openni::VideoFrameRef()) {

	m_device = new openni::Device;
	m_colorStream = new openni::VideoStream;
	m_depthStream = new openni::VideoStream;

	openni::Status rc = openni::STATUS_OK;
	rc = openni::OpenNI::initialize();
#ifdef DEBUG
	printf("After initialization:\n");
#endif /* DEBUG */
	if (rc != openni::STATUS_OK) {
		printf("Couldn't initialize.\n%s\n", openni::OpenNI::getExtendedError());
	}

	rc = m_device -> open(openni::ANY_DEVICE);
	if (rc != openni::STATUS_OK) {
		printf("failed to open device\n");
	}
#ifdef DEBUG
	printf("After device opened:\n");
#endif /* DEBUG */

	openni::VideoStream depthStream, colorStream;
	rc = m_depthStream -> create(*m_device, openni::SENSOR_DEPTH);
	if (rc != openni::STATUS_OK) {
		printf("Couldn't find depth stream\n");
	} else {
		rc = m_depthStream -> start();
		if (rc != openni::STATUS_OK) {
			printf("Couldn't start depth stream\n");
			m_depthStream -> destroy();
		}
	}
#ifdef DEBUG
	printf("Depth stream started\n");
#endif /* DEBUG */

	rc = m_colorStream -> create(*m_device, openni::SENSOR_COLOR);
	if (rc != openni::STATUS_OK) {
		printf("Couldn't find color stream\n");
	} else {
		rc = m_colorStream -> start();
		if (rc != openni::STATUS_OK) {
			printf("Couldn't start depth stream\n");
			m_colorStream -> destroy();
		}
	}
#ifdef DEBUG
	printf("Color stream started\n");
#endif /* DEBUG */
	int data = 0;
	this -> setProperty(STREAM_PROPERTY_AUTO_WHITE_BALANCE, (void *) &data, true);
	this -> setProperty(STREAM_PROPERTY_AUTO_EXPOSURE, (void *) &data, true);
}

cv::Mat CameraInterface::readDepth16U() {
	m_depthStream -> readFrame(&m_depthFrame);
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
	m_colorStream -> readFrame(&m_colorFrame);
	size_t colorByteSize = 3 * m_colorFrame.getWidth() * m_colorFrame.getHeight();
	cv::Mat colorRaw = cv::Mat(m_colorFrame.getHeight(), m_colorFrame.getWidth(), CV_8UC3, (void *) m_colorFrame.getData());
	cv::Mat colorMat;
	cv::cvtColor(colorRaw, colorMat, cv::COLOR_BGR2RGB);
	return colorMat;
}

cv::Mat CameraInterface::readDepthSlice(uint16_t start) {
	m_depthStream->readFrame(&m_depthFrame);
	size_t width = m_depthFrame.getWidth();
	size_t height = m_depthFrame.getHeight();
	size_t depthSize = width * height;
	cv::Mat depthMat = cv::Mat(m_depthFrame.getHeight(), m_depthFrame.getWidth(), CV_16UC1, (void *) m_depthFrame.getData());
	depthMat = depthMat.clone();
	cv::Mat outMat(height, width, CV_8UC1);
	uint16_t stop = start +120;
	uint16_t curr;
	//printf("%d %d", start, stop);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			curr = depthMat.at<uint16_t>(y, x);
			//printf("%d\n", curr);
			if ((curr > stop) || (curr < start)) {
				outMat.at<uint8_t>(y, width - x) = 0;
			} else {
				outMat.at<uint8_t>(y, width - x) = 255 - (depthMat.at<uint16_t>(y, x) - start) & 0xff;
			}
		}
	}

}

void CameraInterface::getProperty(CameraInterface::StreamProperty p, void *data, bool color ) {
	int dataSize;
	switch (color) {
		case true:
			m_colorStream -> getProperty(p, data, &dataSize);
			break;
		case false:
			m_depthStream -> getProperty(p, data, &dataSize);
			break;
	}
}

void CameraInterface::setProperty(CameraInterface::StreamProperty p, void *data, bool color) {
	int dataSize = 0;
	switch (p) {
		case STREAM_PROPERTY_CROPPING: // OniCropping*  
			dataSize = sizeof(OniCropping*);
			break;
		case STREAM_PROPERTY_HORIZONTAL_FOV: // float: radians
			dataSize = sizeof(float);
			break;
		case STREAM_PROPERTY_VERTICAL_FOV: // float: radians
			dataSize = sizeof(float);
			break;
		case STREAM_PROPERTY_VIDEO_MODE: // OniVideoMode*
			dataSize = sizeof(OniVideoMode*);
			break;
		default: //only a few of the types aren't ints, OniBool is just an int in its definition
			dataSize = sizeof(int);
			break;
	}

	switch (color) {
		case true:
			m_colorStream -> setProperty(p, data, dataSize);
			break;
		case false:
			m_depthStream -> setProperty(p, data, dataSize);
			break;
	}
}
