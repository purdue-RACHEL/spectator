#include "Table.hpp"
#include "Projector.hpp"
#include "UartDecoder.hpp"
#include <string>
#include <chrono>

#ifdef TESTTABLE
int main(int argc, char ** argv){
    Projector proj = Projector(1536,768);
    CameraInterface cam = CameraInterface();
    ColorTracker colTrack = ColorTracker();
    ContourTracker conTrack = ContourTracker();
    Table table = Table(cam, colTrack, conTrack, 10);
    std::string deviceStr = "/dev/ttyUSB0";
    UartDecoder uart = UartDecoder(deviceStr);
    if(uart.serial_port == 0){
        std::cout << "Problem Setting Up Serial Port" << std::endl;
        return 1;
    }
    table.setTableBorder();
    table.startDetection();
    cv::Point2f currBounce = cv::Point2f(0,0);
    cv::Point2f projPos;
    cv::Point2f prevProjPos(-1, -1);
    for(;;) {
        uart.readSerial();
        if (cv::waitKey(33) == 27){
	    table.stopDetection();
	    break;
	}

	if(uart.getBounce() == RED || uart.getBounce() == BLUE){
	    //std::cout << "Bounce" << std::endl;
	    currBounce = table.getAveragedPos(uart.last_time, uart.curr_time);
	    std::cout << currBounce.x << " " << currBounce.y << std::endl;
	}
	std::cout << "(" << table.bounceList.front().loc.x << ", " << table.bounceList.front().loc.y << ") time: " << table.bounceList.front().time << std::endl;
	//std::cout << table.bounceListI << std::endl;
	//projPos.x = currBounce.x * proj.w;
	//projPos.y = currBounce.y * proj.h;
	cv::circle(proj.display,projPos, 20, cv::Scalar(225,225,225), 4);
	//proj.drawLine(prevProjPos, projPos, 5, cv::Scalar(255, 127, 255));
	prevProjPos = projPos;
	if(proj.refresh()) break;
        }
    return EXIT_SUCCESS;
}
#endif



Table::Table(CameraInterface& cam, ColorTracker& colTrack, ContourTracker& conTrack, int sampleFreq) : 
  cam(cam),
  colTrack(colTrack),
  conTrack(conTrack),
  sampleFreq(sampleFreq) {
    lastBallPos = cv::Point2f(-1,-1);
    stopSample = FALSE;
    bounceListI = 0;
}
void Table::detectionThread(){
    Table &table = *this;
    std::chrono::milliseconds time_offset = std::chrono::milliseconds(1000/table.sampleFreq);
    std::chrono::milliseconds lastSample;
    lastSample = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
    std::chrono::milliseconds currSample;
    for(;;){
        currSample = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	if(stopSample)
	    return;
        if(currSample - lastSample >= time_offset){
            lastSample = currSample;
            BounceStore currBallPos = BounceStore(table.getNormalizedCoords(),currSample.count());
	    if(currBallPos.loc.x < 0){
		continue;	
	    }
	    table.bounceListI++;
	    table.bounceList.push_front(currBallPos);
        }
    }
}
cv::Point2f Table::getAveragedPos(int startTime, int stopTime){
    Table &table = *this;
    float xSum = 0;
    float ySum = 0;
    int nSamples = 0;
    std::list<BounceStore>::iterator it;
    std::cout << startTime << " to " << stopTime << std::endl;
    for (it = table.bounceList.begin(); it != table.bounceList.end(); it++){
	//std::cout << it->time << std::endl;
	if(it->time > stopTime)
	    continue;
	if(it->time < startTime)
	    break;
	//std::cout << it->loc.x << std::endl;
	xSum += it->loc.x;
	ySum += it->loc.y;
	nSamples++;
    }
    if(nSamples == 0){
	return cv::Point2f(-1,-1);
    }
    return cv::Point2f(xSum/nSamples, ySum/nSamples);
}
void Table::startDetection(){
  samplerThread = std::thread(&Table::detectionThread,this);  
}
void Table::stopDetection(){
  this->stopSample = TRUE;
}
void Table::setTableBorder(){
    Table &table = *this;
    table.top_left = setPointGUI("SET TOP LEFT CORNER");
    table.bottom_right = setPointGUI("SET BOTTOM RIGHT CORNER");
    /*
    table.top_left = cv::Point2f(70, 120);
    table.bottom_right = cv::Point2f(240, 212);
    */
    std::cout << table.top_left << table.bottom_right << std::endl;
}
cv::Point2f Table::setPointGUI(const char * windowName){
    Table &table = *this;
	cv::Point2f curr_point = cv::Point2f(70,120);
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
    /*
    #ifdef TESTTABLE
        cv::Mat in = cam.readColor();
        cv::circle(in, table.top_left, 3, cv::Scalar(0,0,225), -1); 
        cv::circle(in, table.bottom_right, 3, cv::Scalar(0,0,225), -1); 
        cv::circle(in, currPos, 3, cv::Scalar(0,225,225), -1); 
        cv::imshow("Table and Ball", in);
    #endif
    */
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

BounceStore::BounceStore(cv::Point2f loc, int time):
loc(loc),
time(time){
}
