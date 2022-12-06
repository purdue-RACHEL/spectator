#ifndef DISABLEOPENCV

#include "Projector.hpp"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <tiffio.h>
#include <cstring>
#include <stdint.h>

#endif

#ifdef TESTPROJECTOR
#include "UartDecoder.hpp"
int main(int argc, char ** argv){
	Projector proj(1024,768);
	std::string uartDeviceStr = "/dev/ttyUSB0";
	UartDecoder uart(uartDeviceStr);
	for(;;){
		std::string path= "/home/rachel/git/spectator/menus/gameover.tiff";
		proj.renderTiff(path,0,0,.5f);
		if(proj.refresh()){
			break;
		}
	}

}
#endif

#ifdef TESTCOLPROJ
#include "CameraInterface.hpp"
int main(int argc, char ** argv){
	Projector proj(1920,1080);
	CameraInterface cam();
	int pval(FALSE); //Very important to use C-style "FALSE", not C++-style "false"!
	cam.setProperty(true, STREAM_PROPERTY_AUTO_WHITE_BALANCE, &pval);
	cam.setProperty(true, STREAM_PROPERTY_AUTO_EXPOSURE, &pval);
	for(;;){
		proj.refresh();

		if (cv::waitKey(25) == 27) {
			return EXIT_SUCCESS
	}
	}
}
#endif

Projector::Projector(int w, int h){
	Projector &proj = *this;
        proj.h = h;
        proj.w = w;
        cv::Mat display(h, w, CV_8UC3, cv::Scalar(0, 0, 0));
        proj.display = display;
}
void Projector::drawCenterLine(){	
	Projector &proj = *this;
	proj.renderSquare(proj.w/2 - 8,0,16,proj.h,255,255,255);
}
void Projector::putPixel(int x, int y, cv::Vec3b color) {
	Projector &proj = *this;
	//proj.displ
}
void Projector::drawLine(cv::Point2f p1, cv::Point2f p2, int width, cv::Scalar color){
	Projector &proj = *this;
	cv::line(proj.display, p1, p2, color, width);
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

void Projector::renderSquare(int x, int y, int w, int h, cv::Vec3b color){
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
			proj.display.at<cv::Vec3b>(v,u) = color;
		}
	}	
}

//r is the perpendicular distance from (x, y) to each side of the regular polygon
void Projector::renderRegularPolygon(int x, int y, int r, int n, cv::Vec3b color) {
	
	switch(n) {
		case 4: renderSquare(x - r, y - r, 2 * r, 2 * r, color);
			break;
		case 6: break;
		default: ;
	}
}
int Projector::refresh(){
	Projector &proj = *this;
	cv::namedWindow("Projector", cv::WND_PROP_FULLSCREEN);
	cv::setWindowProperty("Projector", cv::WND_PROP_FULLSCREEN, cv::WINDOW_NORMAL);	
	cv::imshow("Projector", proj.display);
	proj.display = cv::Mat(proj.h, proj.w, CV_8UC3, cv::Scalar(0, 0, 0));
	if (cv::waitKey(33) == 27) return 1;
	return 0;

}

void Projector::writeText(std::string& text, float  size, int x, int y, int r, int g, int b){
	Projector &proj = *this;
	cv::putText(proj.display, text, cv::Point(x,y),
			cv::FONT_HERSHEY_PLAIN, size, cv::Scalar(r,g,b), 7, CV_8UC3);
}

void Projector::updateScore(int scoreRed, int scoreBlue) {
	Projector &proj = *this;
	std::string redText = std::to_string(scoreRed);
	std::string blueText = std::to_string(scoreBlue);
	writeRotateText(redText,5,proj.w/2 + 150,proj.h-20,255,255,255,270);
	writeRotateText(blueText,5,proj.w/2 - 150, 20, 255,255,255,90);
}

//Pulled logic out into separate function so that an arbitrary font can be used if needed -JH
void Projector::writeRotateText(const std::string& text, float size, int x, int y, int r, int g, int b, int angle) {
	writeRotateTextFont(text, size, x, y, r, g, b, angle, cv::FONT_HERSHEY_SIMPLEX);
}

void Projector::writeRotateTextFont(const std::string& text, float size, int x, int y, int r, int g, int b, int angle, int cvfont) {
	Projector &proj = *this;
	//Black Backdrop
	//cv::Mat rotImage = cv::Mat::zeros(proj.display.cols, proj.display.rows, proj.display.type());
	//White Backdrop
	cv::Mat rotImage = cv::Mat(proj.display.cols, proj.display.rows, CV_8UC3, cv::Scalar(0, 0, 0));
	if(angle == 270){
		cv::putText(rotImage, text, cv::Point(proj.h - y,x),
			cvfont, size, cv::Scalar(b,g,r), 20, CV_8UC3);
		cv::transpose(rotImage,rotImage);
		cv::flip(rotImage,rotImage,0);
	}
	else if(angle == 90){
		cv::putText(rotImage, text, cv::Point(y,proj.w - x),
			cvfont, size, cv::Scalar(b,g,r), 20, CV_8UC3);
		cv::transpose(rotImage,rotImage);
		cv::flip(rotImage,rotImage,1);
	}
	else {
		std::cout << "Unsupported Rotation angle" << std::endl;
	}
	for(int u = 0; u < proj.w; u++){
		for(int v = 0; v< proj.h; v++){
			cv::Vec3b currCol = rotImage.at<cv::Vec3b>(v,u);
			cv::Vec3b black = cv::Vec3b(0,0,0);
			if(currCol != black){
				proj.display.at<cv::Vec3b>(v,u) = rotImage.at<cv::Vec3b>(v,u);
			}

		}
	}	

}

void Projector::renderTiff(std::string& fname, int xOff, int yOff,float scale){
	// Disable TIFF Warnings
	TIFFSetWarningHandler(NULL);
	// Get and easy reference to the projector
	Projector &proj = *this;
	// Convert to Cstring for TIFF reading
	char * cstr = new char[fname.length() + 1];
	strcpy(cstr, fname.c_str());
	// Attempt to open TIFF
	TIFF* in = TIFFOpen(cstr, "r");
	// If TIFF cannot be opened
	if (in == NULL) {
	std::cout << fname << " could not be opened" << std::endl;
		return;
	}
	// TIFF opened
	if (in) {
		// Find out TIFF dimensions
		uint32_t imageW, imageH;
		TIFFGetField(in, TIFFTAG_IMAGEWIDTH, &imageW);
		TIFFGetField(in, TIFFTAG_IMAGELENGTH, &imageH);
		//std::cerr << "TIFF WIDTH: " << imageW << std::endl;
		//std::cerr << "TIFF HEIGHT: " << imageH << std::endl;
	
		// Read the TIFF image
		size_t nPixels = 0;
		uint32_t* raster; // Image buffer
		nPixels = imageW * imageH;
		raster = (uint32_t*)_TIFFmalloc(nPixels * sizeof(uint32));
		if (raster != NULL) {
			if (TIFFReadRGBAImage(in, imageW, imageH, raster, 0)) { // Seg Fault Here
				for (int j = 0; j < (int) (scale * imageH); j++) {
					for (int i = 0; i < (int) (scale * imageW); i++) {
						//std::cout << i << " " << j << std::endl;
						// Check the bounds of the projector
						if(xOff + i >= proj.w)
							break;
						if(yOff + j >= proj.h)
							break;
						if(xOff + i < 0)
							continue;	
						if(yOff + j < 0)
							continue;
						// Get RGB values from unsigned int
						
						int i_scaled = (int) (i/scale);
						int j_scaled = (int) (j/scale);
						uint32_t currCol = raster[(imageH - j_scaled-1)*imageW + i_scaled];
						// This could be wrong, need to test
						uint8_t a = (currCol >> 24) & 0xFF;
						uint8_t b = (currCol >> 16) & 0xFF;
						uint8_t g = (currCol >> 8) & 0xFF;
						uint8_t r = (currCol) & 0xFF;
						//std::cout << (unsigned int)r << " " << (unsigned int)g << " " <<  (unsigned int)b << " " << (unsigned int) a << std::endl;
						// Set the current pixel from the raster
						if(a != 0)
							proj.display.at<cv::Vec3b>(j + yOff, i + xOff) = cv::Vec3b(b,g,r);

					}
				}
			}
			_TIFFfree(raster);
		}
		TIFFClose(in);
	}
}

// void Projector::renderTiff(const std::string fname, int xOff, int yOff,float scale){
// 	// Get and easy reference to the projector
// 	Projector &proj = *this;
// 	// Convert to Cstring for TIFF reading
// 	char * cstr = new char[fname.length() + 1];
// 	strcpy(cstr, fname.c_str());
// 	// Attempt to open TIFF
// 	TIFF* in = TIFFOpen(cstr, "r");
// 	// If TIFF cannot be opened
// 	if (in == NULL) {
// 	std::cout << fname << " could not be opened" << std::endl;
// 		return;
// 	}
// 	// TIFF opened
// 	if (in) {
// 		// Find out TIFF dimensions
// 		uint32_t imageW, imageH;
// 		TIFFGetField(in, TIFFTAG_IMAGEWIDTH, &imageW);
// 		TIFFGetField(in, TIFFTAG_IMAGELENGTH, &imageH);
// 		//std::cerr << "TIFF WIDTH: " << imageW << std::endl;
// 		//std::cerr << "TIFF HEIGHT: " << imageH << std::endl;
	
// 		// Read the TIFF image
// 		size_t nPixels = 0;
// 		uint32_t* raster; // Image buffer
// 		nPixels = imageW * imageH;
// 		raster = (uint32_t*)_TIFFmalloc(nPixels * sizeof(uint32));
// 		if (raster != NULL) {
// 			if (TIFFReadRGBAImage(in, imageW, imageH, raster, 0)) { // Seg Fault Here
// 				for (int j = 0; j < (int) (scale * imageH); j++) {
// 					for (int i = 0; i < (int) (scale * imageW); i++) {
// 						//std::cout << i << " " << j << std::endl;
// 						// Check the bounds of the projector
// 						if(xOff + i >= proj.w)
// 							break;
// 						if(yOff + j >= proj.h)
// 							break;
// 						if(xOff + i < 0)
// 							continue;	
// 						if(yOff + j < 0)
// 							continue;
// 						// Get RGB values from unsigned int
						
// 						int i_scaled = (int) (i/scale);
// 						int j_scaled = (int) (j/scale);
// 						uint32_t currCol = raster[(imageH - j_scaled-1)*imageW + i_scaled];
// 						// This could be wrong, need to test
// 						uint8_t a = (currCol >> 24) & 0xFF;
// 						uint8_t b = (currCol >> 16) & 0xFF;
// 						uint8_t g = (currCol >> 8) & 0xFF;
// 						uint8_t r = (currCol) & 0xFF;
// 						//std::cout << (unsigned int)r << " " << (unsigned int)g << " " <<  (unsigned int)b << " " << (unsigned int) a << std::endl;
// 						// Set the current pixel from the raster
// 						if(a != 0)
// 							proj.display.at<cv::Vec3b>(j + yOff, i + xOff) = cv::Vec3b(b,g,r);

// 					}
// 				}
// 			}
// 			_TIFFfree(raster);
// 		}
// 		TIFFClose(in);
// 	}
// }
