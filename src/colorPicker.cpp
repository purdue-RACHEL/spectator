#include "CameraInterface.hpp"
#include<iostream>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
using namespace std;
using namespace cv;
int main(int argc, char** argv) {
   CameraInterface cam = CameraInterface();//capturing video from default camera//
   namedWindow("Adjust");//declaring window to show the image//
   int Hue_Lower_Value = 155;//initial hue value(lower)//
   int Hue_Upper_Value = 22;//initial hue value(upper)//
   int Saturation_Lower_Value = 10;//initial saturation(lower)//
   int Saturation_Upper_Value = 255;//initial saturation(upper)//
   int Value_Lower = 188;//initial value (lower)//
   int Value_Upper = 255;//initial saturation(upper)//
   int exposure = 0;//exposure
   int gain = 0;//gain
   OniBool truthy = 1;
   OniBool falsey = 0;
   cam.setProperty(CameraInterface::STREAM_PROPERTY_AUTO_EXPOSURE, &truthy);
   cam.setProperty(CameraInterface::STREAM_PROPERTY_AUTO_WHITE_BALANCE, &truthy);
   cam.getProperty(CameraInterface::STREAM_PROPERTY_EXPOSURE, &exposure);
   cam.getProperty(CameraInterface::STREAM_PROPERTY_GAIN, &gain);
   cout << exposure << " " << gain << endl;
   //cam.setProperty(CameraInterface::STREAM_PROPERTY_AUTO_EXPOSURE, &falsey);
   //cam.setProperty(CameraInterface::STREAM_PROPERTY_AUTO_WHITE_BALANCE, &falsey);

   createTrackbar("Hue_Lower", "Adjust", &Hue_Lower_Value, 179);//track-bar for lower hue//
   createTrackbar("Hue_Upper", "Adjust", &Hue_Upper_Value, 179);//track-bar for lower-upper hue//
   createTrackbar("Sat_Lower", "Adjust", &Saturation_Lower_Value, 255);//track-bar for lower saturation//
   createTrackbar("Sat_Upper", "Adjust", &Saturation_Upper_Value, 255);//track-bar for higher saturation//
   createTrackbar("Val_Lower", "Adjust", &Value_Lower, 255);//track-bar for lower value//
   createTrackbar("Val_Upper", "Adjust", &Value_Upper, 255);//track-bar for upper value//
   createTrackbar("exposure", "Adjust", &exposure, numeric_limits<int>::max());//track-bar for upper value//
   createTrackbar("gain", "Adjust", &gain, numeric_limits<int>::max());//track-bar for upper value//

   while (1) {
      Mat actual_Image = cam.readColor();//loading actual image to matrix from video stream//
      Mat convert_to_HSV;//declaring a matrix to store converted image//
      cvtColor(actual_Image, convert_to_HSV, COLOR_BGR2HSV);//converting BGR image to HSV and storing it in convert_to_HSV matrix//
      Mat detection_screen;//declaring matrix for window where object will be detected//
      if (Hue_Lower_Value > Hue_Upper_Value) {
		Mat temp1, temp2;
		inRange(
			 convert_to_HSV,
			 Scalar(Hue_Lower_Value,Saturation_Lower_Value, Value_Lower), 
			 Scalar(179,Saturation_Upper_Value, Value_Upper),
			 temp1);//applying track-bar modified value of track-bar//
		inRange(
			 convert_to_HSV,
			 Scalar(0,Saturation_Lower_Value, Value_Lower), 
			 Scalar(Hue_Upper_Value,Saturation_Upper_Value, Value_Upper),
			 temp2);//applying track-bar modified value of track-bar//

		bitwise_or(temp1, temp2, detection_screen);
      } else {
		inRange(
			 convert_to_HSV,
			 Scalar(Hue_Lower_Value,Saturation_Lower_Value, Value_Lower),
			 Scalar(Hue_Upper_Value,Saturation_Upper_Value, Value_Upper), 
			 detection_screen);//applying track-bar modified value of track-bar//
      }
      erode(detection_screen, detection_screen, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)));//morphological opening for removing small objects from foreground//
      dilate(detection_screen, detection_screen, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)));//morphological opening for removing small object from foreground//
      dilate(detection_screen, detection_screen, getStructuringElement(MORPH_ELLIPSE, Size(1, 1)));//morphological closing for filling up small holes in foreground//
      erode(detection_screen, detection_screen, getStructuringElement(MORPH_ELLIPSE, Size(1, 1)));//morphological closing for filling up small holes in foreground//
      imshow("Threesholded Image", detection_screen);//showing detected object//
      imshow("Original", actual_Image);//showing actual image//
      int key =waitKey(33);
      if (key == 27){ //if esc is press break the loop//
          break;
      } else if (key == 'q') {
	  std::cout << exposure << " " << gain << std::endl;
          cam.setProperty(CameraInterface::STREAM_PROPERTY_EXPOSURE, &exposure);
          cam.setProperty(CameraInterface::STREAM_PROPERTY_GAIN, &gain);
      }
   }
   return 0;
}
