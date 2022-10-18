#include "Projector.hpp"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

#define TEST_PROJECTOR
#ifdef TEST_PROJECTOR
#include "UartDecoder.hpp"
#include <string>
int main(int argc, char ** argv){
	Projector proj(1080,1920);
	std::string uartDeviceStr = "/dev/ttyUSB0";
	UartDecoder uart(uartDeviceStr);
	proj.renderSquare(40,40,100,100,255,0,0);
	std::string text = "Yo Mama";
	proj.writeText(text,1,10,10,100,100,100);
	proj.redraw();
	cv::waitKey();

}
#endif

Projector::Projector(int h, int w){
	Projector &proj = *this;
        proj.h = h;
        proj.w = w;
        cv::Mat display(h, w, CV_8UC3, cv::Scalar(0, 0, 0));
        proj.display = display;
}
void Projector::renderSquare(int x, int y, int w, int h, int r, int g, int b){
	Projector &proj = *this;
	if((y<0) || (x<0)){
		return;
	}
        if((x > proj.w) || (y > proj.w)){
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
			proj.display.at<cv::Vec3b>(u,v) = cv::Vec3b(r,g,b);
		}
	}	
}
void Projector::redraw(){
	Projector &proj = *this;
	cv::namedWindow("Projector", cv::WND_PROP_FULLSCREEN);
	cv::setWindowProperty("Projector", cv::WND_PROP_FULLSCREEN, cv::WINDOW_NORMAL);	
	cv::imshow("Projector", proj.display);
}
void Projector::writeText(std::string& text, float  size, int x, int y, int r, int g, int b){
	Projector &proj = *this;
	cv::putText(proj.display, text, cv::Point(x,y),
			cv::FONT_HERSHEY_COMPLEX_SMALL, size, cv::Scalar(r,g,b), 1, CV_8UC3);
}
void Projector::updateScore(int scoreRed, int scoreBlue){
	Projector &proj = *this;
	cv::Mat rotImage = Mat::zeros(proj.display.rows, proj.display.cols, proj.display.type());
    putText(rotImage, "RotateText", cv::Point(0, proj.display.cols/2), cv::FONT_HERSHEY_COMPLEX_SMALL, 5.0,cv::Scalar(255,255,255),1);
    putRotateText(rotImage, 90, rotImage);	
	proj.display+= rotImage;
}
void Projector::putRotateText(cv::Mat& src, double angle, cv::Mat& dst)
{
    int _len = std::max(src.cols, src.rows);
    cv::Point2f pt(_len/2., _len/2.);
    cv::Mat r = cv::getRotationMatrix2D(pt, angle, 1.0);
    cv::warpAffine(src, dst, r, cv::Size(_len, _len));
}