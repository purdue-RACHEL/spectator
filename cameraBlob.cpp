/*
 * James Hubbard
 * fbtest.cpp
 * Reimplementation of oldfbtest.c in cpp to facilitate
 * incremental testing and integration with OpenNI2
 */

// Defines that print stuff
#define DEBUG
#define DEBUG_DRAW

#include "blobDetector.h"
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

struct PixelBGR {
	double b;
	double g;
	double r;
};

struct PixelHSV {
	double h;
	double s;
	double v;
};
	
cv::Mat convert16U2palette(cv::Mat& sixteenbit, std::vector<struct PixelBGR>& palette) {
	cv::Mat outMat(sixteenbit.rows, sixteenbit.cols, CV_8UC3), chans[3];
	chans[0] = cv::Mat::zeros(sixteenbit.rows, sixteenbit.cols, CV_8UC1);
	chans[1] = cv::Mat::zeros(sixteenbit.rows, sixteenbit.cols, CV_8UC1);
	chans[2] = cv::Mat::zeros(sixteenbit.rows, sixteenbit.cols, CV_8UC1);
	for (int y = 0; y < sixteenbit.rows; y++) {
		for (int x = 0; x < sixteenbit.cols; x++) {
			chans[0].at<uint8_t>(y, x) = (uint8_t) palette[sixteenbit.at<uint16_t>(y, x)].r;
			chans[1].at<uint8_t>(y, x) = (uint8_t) palette[sixteenbit.at<uint16_t>(y, x)].g;
			chans[2].at<uint8_t>(y, x) = (uint8_t) palette[sixteenbit.at<uint16_t>(y, x)].b;
		}
	}
	cv::merge(chans, 3, outMat);
	return outMat;
}

struct PixelBGR hsv2rgb(struct PixelHSV in)
{
    double      hh, p, q, t, ff;
    long        i;
    PixelBGR         out;

    if(in.s <= 0.0) {       // < is bogus, just shuts up warnings
        out.r = in.v;
        out.g = in.v;
        out.b = in.v;
        return out;
    }
    hh = in.h;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in.v * (1.0 - in.s);
    q = in.v * (1.0 - (in.s * ff));
    t = in.v * (1.0 - (in.s * (1.0 - ff)));

    switch(i) {
    case 0:
        out.r = in.v;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = in.v;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = in.v;
        out.b = t;
        break;

    case 3:
        out.r = p;
        out.g = q;
        out.b = in.v;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = in.v;
        break;
    case 5:
    default:
        out.r = in.v;
        out.g = p;
        out.b = q;
        break;
    }
    out.r *= 255;
    out.g *= 255;
    out.b *= 255;
    return out;     
}


void generate16BitPalette(std::vector<struct PixelBGR>& palette) {
	struct PixelBGR pix;
	struct PixelHSV hsv;
	hsv.s = 1.0;
	hsv.v = 1.0;
	size_t num_revs = 8;

	for (int i = 0; i < 65536; i++) {
		hsv.h = (double) (( (long) (num_revs * 360.0 * (i / 65536.0)) ) % 360);
		hsv.s = 0.9;
		hsv.v = 0.9;
		pix = hsv2rgb(hsv);
		palette.push_back(pix);
	}
}

class CameraInterface {
	public:
		CameraInterface(openni::Device& device, openni::VideoStream& depth, openni::VideoStream& color);
		~CameraInterface();

		cv::Mat readDepth();
		
		cv::Mat readDepthSlice(uint16_t start);
		cv::Mat readDepth16U();
		cv::Mat readDepthBGR565();
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

cv::Mat CameraInterface::readDepthSlice(uint16_t start) {
	m_depthStream.readFrame(&m_depthFrame);
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
	return outMat;
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

cv::Mat CameraInterface::readDepthBGR565() {
	cv::Mat depthMat = readDepth16U();
	cv::Mat outMat(depthMat.rows, depthMat.cols, CV_8UC3);
	cv::Mat chans[3];
	cv::split(outMat, chans);
	for (int y = 0; y < depthMat.rows; y++) {
		for (int x = 0; x < depthMat.cols; x++) {
			uint16_t inMat = depthMat.at<uint16_t>(y, x);
			chans[0].at<uint8_t>(y, x) = ((inMat >> 8) && 0xFF);
			chans[1].at<uint8_t>(y, x) = ((inMat >> 4) && 0xFF);
			chans[2].at<uint8_t>(y, x) = ((inMat >> 0) && 0xFF);
		}
	}
	cv::merge(chans, 3, outMat);
	return outMat;
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
	
	std::vector<struct PixelBGR> palette;
	generate16BitPalette(palette);

	for (;;) {
		cv::Mat eightbit = cam.readDepth();
		cv::Mat equalized, eq_color, bgr565, sixteenbit, colorized_16bit_palette, slice;
		sixteenbit = cam.readDepth16U();
		//cv::cvtColor(sixteenbit, bgr565, cv::COLOR_BGR5652BGR, 3);
		sixteenbit = cam.readDepth16U();
		colorized_16bit_palette = convert16U2palette(sixteenbit, palette);
		cv::equalizeHist(eightbit, equalized);
		cv::applyColorMap(equalized, eq_color, cv::COLORMAP_AUTUMN);
		//cv::imshow("8bit depth", eightbit);
		//cv::imshow("equalized", equalized);
		//cv::imshow("colorized compressed depth", eq_color);
		//cv::imshow("colorized uncompressed depth", colorized_16bit_palette);
		//detectBlob(colorized_16bit_palette);
		slice = cam.readDepthSlice(1940);
		detectBlob(slice);
		cv::waitKey(25);
	}
	printf("No errors!\n");
	return EXIT_SUCCESS;
}
