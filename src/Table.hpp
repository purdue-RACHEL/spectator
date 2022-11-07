#include "CameraInterface.hpp"
#include "ColorTracker.hpp"
#include "ContourTracker.hpp"
#include <opencv2/features2d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/optflow.hpp>
#include <opencv2/video.hpp>
#include <vector>

class Table{
    public:
        CameraInterface * cam;
        ColorTracker * colTrack;
        ContourTracker * conTrack;
        cv::Point2f top_left;
        cv::Point2f bottom_right; 

        Table(CameraInterface&, ColorTracker&, ContourTracker&);
        cv::Point setPointGUI(String &);
        void setTableBorder();
        cv::Point2f getNormalizedCoords();


}