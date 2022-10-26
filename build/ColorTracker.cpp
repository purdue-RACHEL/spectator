/* James Hubbard
 * ColorTracker.cpp
 * Takes a range of HSV colors and returns object locations
 */

#include "ColorTracker.hpp"

ColorTracker::ColorTracker() {}

ColorTracker::~ColorTracker() {}

cv::Mat ColorTracker::filterImage(cv::Mat in, int hl, int sl, int vl, int hu, int su, int vu) {
	cv::Mat im = in;//loading actual image to matrix from video stream//
	cv::Mat imHSV;//declaring a matrix to store converted image//
	cv::cvtColor(im, imHSV, cv::COLOR_BGR2HSV);//converting BGR image to HSV and storing it in convert_to_HSV matrix//
	cv::Mat imBIN;//declaring matrix for window where object will be detected//
	if (hl > hu) {
		cv::Mat temp1, temp2;
		cv::inRange(
		 	imHSV,
			cv::Scalar(hl,sl, vl), 
		 	cv::Scalar(179,su, vu),
		 	temp1);//applying track-bar modified value of track-bar//
		cv::inRange(
		 	imHSV,
		 	cv::Scalar(0,sl, vl), 
		 	cv::Scalar(hu,su, vu),
			temp2);//applying track-bar modified value of track-bar//
		cv::bitwise_or(temp1, temp2, imBIN);
	} else {
		cv::inRange(
			imHSV,
			cv::Scalar(hl,sl, vl),
			cv::Scalar(hu,su, vu), 
			imBIN);//applying track-bar modified value of track-bar//
	}
	erode(imBIN, imBIN, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3)));//morphological opening for removing small objects from foreground//
	dilate(imBIN, imBIN, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3)));//morphological opening for removing small object from foreground//
	dilate(imBIN, imBIN, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(1, 1)));//morphological closing for filling up small holes in foreground//
	erode(imBIN, imBIN, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(1, 1)));//morphological closing for filling up small holes in foreground//
	return imBIN;
}


