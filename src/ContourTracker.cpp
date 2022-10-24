#include "ContourTracker.hpp"

ContourTracker::ContourTracker(){
    table_offset = cv::Point2f(0.0f,0.0f);
}

void ContourTracker::findContours(cv:Mat threshold_image){
    ContourTracker &curr = *this;
	std::vector<std::vector<std::Point>> _contours; 
    std::vector<cv::Vec4i> hierarchy;
	cv::findContours(threshold_image, _contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, curr.table_offset);
    curr.contours = _contours;
}

cv::Mat ContourTracker::drawContours(std::vector<std::vector<cv::Point>> contours){
	cv::Mat black(im_thresh.rows, im_thresh.cols, CV_8UC3, cv::Scalar(0, 0, 0));
	cv::drawContours(black, contours, -1, Scalar(255,172,172));
    return black;
}

cv::Mat ContourTracker::drawContours(){
    ContourTracker &curr = *this;
	cv::Mat black(im_thresh.rows, im_thresh.cols, CV_8UC3, cv::Scalar(0, 0, 0));
	cv::drawContours(black, curr.contours, -1, Scalar(255,172,172));
    return black;
}

std::vector<std::vector<cv:Point>> ContourTracker::getContours(){
    ContourTracker &curr = *this;
    return curr.contours; 
}
