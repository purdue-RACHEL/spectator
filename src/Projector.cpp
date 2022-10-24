#ifndef DISABLEOPENCV

#include "Projector.hpp"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

#ifdef TESTPROJECTOR
#include "UartDecoder.hpp"
#include <string>
int main(int argc, char ** argv){
	Projector proj(1920,1080);
	std::string uartDeviceStr = "/dev/ttyUSB0";
	UartDecoder uart(uartDeviceStr);
	proj.updateScore(20,20);
	for(;;){
		proj.redraw();
	}

}
#endif

Projector::Projector(int w, int h){
	Projector &proj = *this;
        proj.h = h;
        proj.w = w;
        cv::Mat display(h, w, CV_8UC3, cv::Scalar(255, 255, 255));
        proj.display = display;
}
void Projector::drawCenterLine(){	
	Projector &proj = *this;
	proj.renderSquare(1920/2 - 8,0,16,1080,0,0,0);
}
void Projector::renderSquare(int x, int y, int w, int h, int r, int g, int b){
	Projector &proj = *this;
	if((y<0) || (x<0)){
		return;
	}
        if((x > proj.w) || (y > proj.h)){
            	return;
	}
	if(x < 0){
		w = w + x;
		x = 0;
	}
	if(y < 0){
		h = h + y;
		y = 0;
	}
	if(x + w >=  proj.w){
		w = proj.w - x - 1;	
	}
	if(y + h >=  proj.h){
		h = proj.h - y - 1;	
	}
	for(int u = x;u<x+w;u++){
		for(int v = y; v< y + h;v++){
			proj.display.at<cv::Vec3b>(v,u) = cv::Vec3b(b,g,r);
		}
	}	
}
void Projector::refresh(){
	Projector &proj = *this;
	cv::namedWindow("Projector", cv::WND_PROP_FULLSCREEN);
	cv::setWindowProperty("Projector", cv::WND_PROP_FULLSCREEN, cv::WINDOW_NORMAL);	
	cv::imshow("Projector", proj.display);
	cv::pollKey();
	proj.display = cv::Mat(proj.h, proj.w, CV_8UC3, cv::Scalar(255, 255, 255));
	
}
void Projector::writeText(std::string& text, float  size, int x, int y, int r, int g, int b){
	Projector &proj = *this;
	cv::putText(proj.display, text, cv::Point(x,y),
			cv::FONT_HERSHEY_SIMPLEX, size, cv::Scalar(r,g,b), 20, CV_8UC3);
}
void Projector::updateScore(int scoreRed, int scoreBlue){
	Projector &proj = *this;
	std::string redText = std::to_string(scoreRed);
	std::string blueText = std::to_string(scoreBlue);
	writeRotateText(blueText,5,proj.w/2 + 150,proj.h-20,0,0,255,270);
	writeRotateText(redText,5,proj.w/2 - 150, 20, 255,0,0,90);
}
void Projector::writeRotateText(std::string& text, float size, int x, int y, int r, int g, int b, int angle)
{
	Projector &proj = *this;
	//Black Backdrop
	//cv::Mat rotImage = cv::Mat::zeros(proj.display.cols, proj.display.rows, proj.display.type());
	//White Backdrop
	cv::Mat rotImage = cv::Mat(proj.display.cols, proj.display.rows, CV_8UC3, cv::Scalar(255, 255, 255));
	if(angle == 270){
		cv::putText(rotImage, text, cv::Point(proj.h - y,x),
			cv::FONT_HERSHEY_SIMPLEX, size, cv::Scalar(b,g,r), 20, CV_8UC3);
		cv::transpose(rotImage,rotImage);
		cv::flip(rotImage,rotImage,0);
	}
	else if(angle == 90){
		cv::putText(rotImage, text, cv::Point(y,proj.w - x),
			cv::FONT_HERSHEY_SIMPLEX, size, cv::Scalar(b,g,r), 20, CV_8UC3);
		cv::transpose(rotImage,rotImage);
		cv::flip(rotImage,rotImage,1);
	}
	else {
		std::cout << "Unsupported Rotation angle" << std::endl;
	}
	for(int u = 0; u < proj.w; u++){
		for(int v = 0; v< proj.h; v++){
			cv::Vec3b currCol = rotImage.at<cv::Vec3b>(v,u);
			cv::Vec3b white = cv::Vec3b(255,255,255);
			if(currCol != white){
				proj.display.at<cv::Vec3b>(v,u) = rotImage.at<cv::Vec3b>(v,u);
			}

		}
	}	

}
#endif
