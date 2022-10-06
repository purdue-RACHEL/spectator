#ifndef INCLUDE_PROJECTOR
#define INCLUDE_PROJECTOR

#include <opencv2/opencv.hpp>

class Projector{
    public:
        Projector(int h, int w);
        void renderSquare(int x, int y, int w, int h, float r, float g, float b);
    private:
        int h,w;
        cv::Mat display;
};

#endif