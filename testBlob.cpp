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
	Mat im = imread("/home/rachel/Pictures/Random-On-Screen/0.tif", IMREAD_GRAYSCALE);
	
	Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create();
	std::vector<KeyPoint> keypoints;

	detector -> detect(im, keypoints);
	Mat im_with_keypoints;
	drawKeypoints(im, keypoints, im_with_keypoints, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	imshow("keypoints", im_with_keypoints);
	waitKey();
}
