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

#include "blobDetector.h"

using namespace std;
using namespace cv;

#define DEBUG_BLOB_DETECTOR
Ptr<SimpleBlobDetector> detector;
int main(int argc, char** argv) {
	Mat im = imread("/home/rachel/Pictures/Random-On-Screen/1.tif", IMREAD_COLOR);
	initBlobDetection();
	detectBlob(im);
	return EXIT_SUCCESS;
}
int initBlobDetection(){
	SimpleBlobDetector::Params params;
	
	/* Default params.
	params.thresholdStep = 10;
	params.minThreshold = 10;
	params.maxThreshold = 220;
	params.minRepeatability = 2;
	params.minDistBetweenBlobs = 10;
	params.filterByColor = false;
	params.blobColor = 0;
	params.filterByArea = false;
	params.minArea = 25;
	params.maxArea = 5000;
	params.filterByCircularity = false;
	params.minCircularity = 0.9f;
	params.maxCircularity = (float) 1e37;
	params.filterByInertia = false;
	params.minInertiaRatio = 0.1f;
	params.maxInertiaRatio = (float) 1e37;
	params.filterByConvexity = false;
	params.minConvexity = 0.95f;
	params.maxConvexity = (float) 1e37;
	*/
	
	params.thresholdStep = 10;
	params.minThreshold = 10;
	params.maxThreshold = 220;
	params.minRepeatability = 1;
	params.minDistBetweenBlobs = 10;
	params.filterByColor = true;
	params.blobColor = 0xd4;
	params.filterByArea = false;
	params.minArea = 25;
	params.maxArea = 500;
	params.filterByCircularity = false;
	params.minCircularity = 0.9f;
	params.maxCircularity = (float) 1e37;
	params.filterByInertia = false;
	params.minInertiaRatio = 0.1f;
	params.maxInertiaRatio = (float) 1e37;
	params.filterByConvexity = false;
	params.minConvexity = 0.95f;
	params.maxConvexity = (float) 1e37;
	
	detector = SimpleBlobDetector::create(params);
	return EXIT_SUCCESS;
}

int detectBlob(Mat im){

	//im: matrix representing photo
	Mat imgray, im_gauss, im_thresh, im_hierarchy;
	vector<vector<Point>> contours; 
	cvtColor(im, imgray, COLOR_BGR2GRAY);
	GaussianBlur(imgray, im_gauss, Size(5,5), 0);
	threshold(im_gauss, im_thresh, 212, 255, THRESH_BINARY);
	imshow("Original", im);
	findContours(im_thresh, contours, im_hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
	drawContours(im, contours, -1, Scalar(255,172,172));
	imshow("Contours", im);
	#ifdef DEBUG_BLOB_DETECTOR
	//cerr << "Key Points Found: " <<keypoints.size() << endl;
	#endif
	//imshow("keypoints", im_gauss);
	waitKey();
	return EXIT_SUCCESS;
}
