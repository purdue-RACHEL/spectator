#pragma once
#ifndef INCLUDE_PROJECTOR
#define INCLUDE_PROJECTOR

#include <opencv2/opencv.hpp>
#include <string.h>

class Projector{
    public:
        Projector(int h, int w);
        void renderSquare(int x, int y, int w, int h, int r, int g, int b);
        void renderSquare(int x, int y, int w, int h, cv::Vec3b color);
	void renderRegularPolygon(int x, int y, int r, int n, cv::Vec3b color);
	void putPixel(int x, int y, cv::Vec3b color);
	int refresh();
	void writeText(std::string &text, float size, int x, int y, int r, int g, int b);
        void updateScore(int, int);
	void writeRotateText(const std::string &text, float size, int x, int y, int r, int g, int b, int angle);
	void writeRotateTextFont(const std::string &text, float size, int x, int y, int r, int g, int b, int angle, int cvfont);
	void drawLine(cv::Point2f p1, cv::Point2f p2, int width, cv::Scalar color);
	void drawCenterLine();
        void renderTiff(std::string& fname, int xOff, int yOff, float scale);
        // void renderTiff(const std::string fname, int xOff, int yOff, float scale);
        int h,w;
        cv::Mat display;
};

#endif
