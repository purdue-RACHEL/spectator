#include "Table.hpp"
#include "Projector.hpp"
#include "UartDecoder.hpp"
#include <string>

#ifdef TESTTABLE
int main(int argc, char ** argv){
    Projector proj = Projector(1536,768);
	CameraInterface cam = CameraInterface();
	ColorTracker colTrack = ColorTracker();
    ContourTracker conTrack = ContourTracker();
    Table table = Table(cam, colTrack, conTrack);
    table.setTableBorder();
	//Uart Setup
    std::string deviceStr = "/dev/ttyUSB0";
        UartDecoder uart = UartDecoder(deviceStr);
    	if(uart.serial_port == 0){
		std::cout << "Problem Setting Up Serial Port" << std::endl;
        	return 1;
    	}

    for(;;){
        uart.readSerial();
	if(uart.getBounce() == RED || uart.getBounce() == BLUE){
        	cv::Point2f curPos = table.getNormalizedCoords();
        	std::cout << curPos << std::endl; 
		cv::Point2f projPos;
		projPos.x = curPos.x * proj.w;
		projPos.y = curPos.y * proj.h;
		cv::circle(proj.display,projPos, 20, cv::Scalar(225,225,225), 4);
		if(proj.refresh()) break;
	}
        if (cv::waitKey(33) == 27) break;
    }
    return EXIT_SUCCESS;
}
#endif

Table::Table(CameraInterface & cam, ColorTracker & colTrack, ContourTracker & conTrack){
    cam = cam;
    colTrack = colTrack;
    conTrack = conTrack;
}
void Table::setTableBorder(){
    Table &table = *this;
    table.top_left = setPointGUI("SET TOP LEFT CORNER");
    table.bottom_right = setPointGUI("SET BOTTOM RIGHT CORNER");

}
cv::Point2f Table::setPointGUI(const char * windowName){
    Table &table = *this;
	cv::Point2f curr_point = cv::Point2f(0,0);
	for(;;){
		cv::Mat in = table.cam.readColor();
		cv::circle(in, curr_point, 3, cv::Scalar(0,0,225), -1);
		cv::imshow(windowName, in);
		switch(cv::waitKey(33)){
			case 'w':
				curr_point.y = curr_point.y - 2;
				break;
			case 'a':
				curr_point.x = curr_point.x - 2;
				break;
			case 's':
				curr_point.y = curr_point.y + 2;
				break;
			case 'd':
				curr_point.x = curr_point.x + 2;
				break;
			case 'q':
				cv::destroyWindow(windowName);
				return curr_point;
		}
	}
}

cv::Point2f Table::getNormalizedCoords(){
    Table &table = *this;
    cv::Point2f currPos = table.getBallCoords();
    if((currPos.x < table.top_left.x) || (currPos.x > table.bottom_right.x)){
       return cv::Point2f(-1,-1); 
    }
    if((currPos.y < table.top_left.y) || (currPos.y > table.bottom_right.y)){
       return cv::Point2f(-1,-1); 
    }
    cv::Point2f retPos;
    retPos.x = (currPos.x - table.top_left.x) / (table.bottom_right.x - table.top_left.x);
    retPos.y = 1- (currPos.y - table.top_left.y) / (table.bottom_right.y - table.top_left.y);
    #ifdef TESTTABLE
        cv::Mat in = cam.readColor();
        cv::circle(in, table.top_left, 3, cv::Scalar(0,0,225), -1); 
        cv::circle(in, table.bottom_right, 3, cv::Scalar(0,0,225), -1); 
        cv::circle(in, currPos, 3, cv::Scalar(0,225,225), -1); 
        cv::imshow("Table and Ball", in);
    #endif
    return retPos;
}

cv::Point2f Table::getBallCoords(){
    Table &table = *this;
    cv::Mat in, bin;
    in = cam.readColor();
	bin = table.colTrack.filterImage(in, 164, 89, 175, 22, 255, 255);
        table.conTrack.findContours(bin);
    cv::Point ballCenter = table.conTrack.findBallCenter();
    return ballCenter;
}
