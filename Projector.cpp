#include "Projector.hpp"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

int main(int argc, char ** argv){
	Projector proj(100,100);
	proj.renderSquare(40,40,100,100,255,255,255);
	proj.redraw();
	cv::waitKey();
}
Projector::Projector(int h, int w){
	Projector &proj = *this;
        proj.h = h;
        proj.w = w;
        cv::Mat display(h, w, CV_8UC3, cv::Scalar(0, 0, 0));
        proj.display = display;
}
void Projector::renderSquare(int x, int y, int w, int h, float r, float g, float b){
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
	cv::imshow("Projector", proj.display);
}
