#include <opencv2/features2d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/optflow.hpp>
#include <opencv2/video.hpp>
#include <vector>

class ContourTracker{
    public:
        cv::Point table_offset;
        std::vector<std::vector<cv::Point>> contours;

        ContourTracker();
        void findContours(cv::Mat);
        cv::Point ballPos(std::vector<std::vector<cv::Point>>);
        cv::Mat drawContours(std::vector<std::vector<cv::Point>>, int, int);
        cv::Mat drawContours(int, int);
        std::vector<std::vector<cv::Point>> getContours();
	cv::Point findBallCenter();

};
