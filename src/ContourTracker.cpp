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
#ifndef TESTPROJECTOR
void ContourTracker::setTableGUI(CameraInterface & cam){

    	ContourTracker &curr = *this;
	for(;;){
		cv::Mat in = cam.readColor();
		cv::circle(in, curr.table_offset, 3, cv::Scalar(0,0,225), -1);
		cv::imshow("Adjust Table:(WASD), Set Point:(q)", in);
		switch(cv::waitKey(33)){
			case 'w':
				curr.table_offset.y = curr.table_offset.y - 2;
				break;
			case 'a':
				curr.table_offset.x = curr.table_offset.x - 2;
				break;
			case 's':
				curr.table_offset.y = curr.table_offset.y + 2;
				break;
			case 'd':
				curr.table_offset.x = curr.table_offset.x + 2;
				break;
			case 'q':
				cv::destroyWindow("Adjust Table:(WASD), Set Point:(q)");
				return;
		}
	}

}
#endif
