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
#include "CameraInterface.hpp"
#include "ColorTracker.hpp"
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

int main(void) {
	CameraInterface cam = CameraInterface();
	ColorTracker colTrack = ColorTracker();
	cv::Mat in, bin;
	cv::Scalar lower = cv::Scalar(164, 89, 175);
	cv::Scalar upper = cv::Scalar(22, 255, 255);
	
	for (;;) {
		in = cam.readColor();
		bin = colTrack.filterImage(in, 164, 89, 175, 22, 255, 255);
		cv::imshow("orig", in);
		cv::imshow("filtered", bin);
		if (cv::waitKey(33) == 27) break;
	}
	printf("No errors!\n");
	return EXIT_SUCCESS;
}
