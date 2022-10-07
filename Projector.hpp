#ifndef INCLUDE_PROJECTOR
#define INCLUDE_PROJECTOR

#include <opencv2/opencv.hpp>

class Projector{
    public:
        Projector(int h, int w);
        void renderSquare(int x, int y, int w, int h, int r, int g, int b);
	void redraw();
	void writeText(std::string &text, float size, int x, int y, int r, int g, int b);
    private:
        int h,w;
        cv::Mat display;
};

#endif
