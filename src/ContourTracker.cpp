#include "ContourTracker.hpp"


ContourTracker::ContourTracker(){
    table_offset = cv::Point2d(0,0);
}

void ContourTracker::findContours(cv::Mat threshold_image){
    ContourTracker &curr = *this;
	std::vector<std::vector<cv::Point>> _contours; 
    std::vector<cv::Vec4i> hierarchy;
    	//offset taken into account on contour data
	//cv::findContours(threshold_image, _contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, curr.table_offset);
	cv::findContours(threshold_image, _contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
    curr.contours = _contours;
}
void ContourTracker::findContours(cv::Mat threshold_image, cv::Point2f top_left, cv::Point2f bottom_right){
    ContourTracker &curr = *this;
    cv::rectangle(threshold_image,cv::Point2f(0,0),cv::Point2f(top_left.x,threshold_image.cols),cv::Scalar(0,0,0),-1);
    cv::rectangle(threshold_image,cv::Point2f(top_left.x,0),cv::Point2f(bottom_right.x,top_left.y),cv::Scalar(0,0,0),-1);
    cv::rectangle(threshold_image,cv::Point2f(top_left.x,bottom_right.y),cv::Point2f(bottom_right.x,threshold_image.rows),cv::Scalar(0,0,0),-1);
    cv::rectangle(threshold_image,cv::Point2f(bottom_right.x,0),cv::Point2f(threshold_image.cols,threshold_image.rows),cv::Scalar(0,0,0),-1);
    cv::imshow("Cropped Filtered",threshold_image);
    std::vector<std::vector<cv::Point>> _contours; 
    std::vector<cv::Vec4i> hierarchy;
    	//offset taken into account on contour data
	//cv::findContours(threshold_image, _contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, curr.table_offset);
	cv::findContours(threshold_image, _contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
    curr.contours = _contours;
}

cv::Mat ContourTracker::drawContours(std::vector<std::vector<cv::Point>> contours,int rows, int cols){
	cv::Mat black(rows, cols, CV_8UC3, cv::Scalar(0, 0, 0));
	cv::drawContours(black, contours, -1, cv::Scalar(255,172,172));
    return black;
}

cv::Mat ContourTracker::drawContours(int rows, int cols){
    ContourTracker &curr = *this;
	cv::Mat black(rows, cols, CV_8UC3, cv::Scalar(0, 0, 0));
	cv::drawContours(black, curr.contours, -1, cv::Scalar(255,172,172));
    return black;
}

std::vector<std::vector<cv::Point>> ContourTracker::getContours(){
    ContourTracker &curr = *this;
    return curr.contours; 
}
cv::Point ContourTracker::findBallCenter(){
	double largestArea = 0;
	int ballx = -1;
	int bally = -1;
#pragma omp for
	for (std::vector<cv::Point> cont : contours){
		// Calculate Area
		double currArea = cv::contourArea(cont);
		if(currArea < largestArea){
			continue;
		}
		largestArea = currArea;

		// Calculate Center if Area is Largest
		cv::Moments currM = cv::moments(cont);
    		if(currM.m00 != 0){
        		ballx = (int)(currM.m10/currM.m00);
        		bally = (int)(currM.m01/currM.m00);
		}
	}
	return cv::Point(ballx,bally);
}
