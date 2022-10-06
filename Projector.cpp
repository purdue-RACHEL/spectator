#include "Projector.cpp"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

int main(int argc, char ** argv){
    Projector::Projector(int h, int w){
        Projector &proj = *this;
        proj.h = h;
        proj.w = w;
        cv::Mat display;
        cv::Mat display(h, w, CV_8UC3, cv::Scalar(0, 0, 0));
        proj.display = display;
    }

    void Projector::renderSquare(int x, int y, int w, int h, float r, float g, float b){
        Projector &proj = *this;
        if((x > proj.w) || (x < 0) || (y > proj.w) || (y < 0)){
            return;
        } 
        
    }
}