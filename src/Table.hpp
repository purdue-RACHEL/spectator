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
#include <thread>
#include <atomic>
#include <list>

#define SINGLE_SAMPLE 1

// COLOR FILTER VALUES

// ORANGE
#define HUE_LOWER 38
#define HUE_UPPER 82
#define VAL_LOWER 50
#define VAL_UPPER 255
#define SAT_LOWER 45
#define SAT_UPPER 255

//GREEN
/*
#define HUE_LOWER
#define HUE_UPPER
#define VAL_LOWER
#define VAL_UPPER
#define SAT_LOWER
#define SAT_UPPER
*/

//YELLOW
/*
#define HUE_LOWER
#define HUE_UPPER
#define VAL_LOWER
#define VAL_UPPER
#define SAT_LOWER
#define SAT_UPPER
*/

class BounceStore {
    public:
	BounceStore(cv::Point2f,int);
        cv::Point2f loc;
        unsigned int time;
};

class Table{
    public:
        CameraInterface cam;
        ColorTracker colTrack;
        ContourTracker conTrack;
        cv::Point2f top_left;
        cv::Point2f bottom_right; 
	cv::Point2f lastBallPos;
        std::thread samplerThread;
	int sampleFreq;
	bool stopSample;
	std::list<BounceStore> bounceList;
	int bounceListI;
	
	// Color Picker Params
        int Hue_Lower_Value;//initial hue value(lower)//
        int Hue_Upper_Value;//initial hue value(upper)//
        int Saturation_Lower_Value;//initial saturation(lower)// 
        int Saturation_Upper_Value;//initial saturation(upper)//
        int Value_Lower;//initial value (lower)//    
        int Value_Upper;//initial saturation(upper)//


	//Initializes Table.
	//CameraInterface: Current Camera Interface
	//ColorTracker: Current Color Tracker
	//ContourTracker: Current Contour Tracker
	//int: Max Sampling Frequency
        Table(CameraInterface&, ColorTracker&, ContourTracker&, int);
	//Creates a Popup window to move around a point
	//const char *: Name of window
        cv::Point2f setPointGUI(const char *, cv::Point2f&);
	//Calls setPointGUI to set the edges of the table
        void setTableBorder();
	//Returns the normalized coords of the ball on the table
	cv::Point2f getNormalizedCoords();
	//Returns the non-normalized ball coords
	cv::Point2f getBallCoords();	
	//The function started as a thread
    	void detectionThread();
	//Starts detectionThread
	//Must be paired with stopDetection
	void startDetection();
	//Stops detectionThread and frees the list of bounces
	//Must be called after startDetection
	void stopDetection();
	//Gets the average postion of a ball within the specified time frame
	//int: start time in time since epoch in milliseconds
	//int: end time in time since epoch in milliseconds
	cv::Point2f getAveragedPos(int startTime, int stopTime);
};
