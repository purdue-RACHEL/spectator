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

using namespace std;
using namespace cv;

#define DEBUG_BLOB_DETECTOR

int main(int argc, char** argv) {
	Mat im = imread("/home/rachel/Pictures/Random-On-Screen/1.tif", IMREAD_GRAYSCALE);

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
	
	Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);
	std::vector<KeyPoint> keypoints;

	detector -> detect(im, keypoints);
	Mat im_with_keypoints;
	drawKeypoints(im, keypoints, im_with_keypoints, Scalar(255,0,0), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	imshow("keypoints", im_with_keypoints);
	waitKey();
}
