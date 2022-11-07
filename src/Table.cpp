#include "Table.hpp"
#include "Projector.hpp"
#ifdef TESTTABLE
int main(void){
    Projector proj = Projector(1024,768);
	CameraInterface cam = CameraInterface();
	ColorTracker colTrack = ColorTracker();
    ContourTracker conTrack = ContourTracker();
    Table table = Table(cam, colTrack, conTrack);
    table.setTableBorder()
    for(;;){
        cv::Point2f curPos = table.getNormalizedCoords();
        std::cout << curPos << std::endl; 
        proj.renderSquare(curPos.x,curPos.y,3,3,0,0,0);
		if(proj.refresh()) break;
        if (cv::waitKey(33) == 27) break;
    }
    return EXIT_SUCCESS;
}
#endif
void Table::Table(CameraInterface & cam, ColorTracker & colTrack, ContourTracker & conTrack){
    cam = cam;
    colTrack = colTrack;
    conTrack = conTrack;
}
void Table::setTableBorder(){
    Table &table = *this;
    table.top_left = setPointGUI("SET TOP LEFT CORNER");
    table.bottom_right = setPointGUI("SET BOTTOM RIGHT CORNER");

}
cv::Point Table::setPointGUI(String & windowName){
    Table &table = *this;
	for(;;){
        cv::Point2f curr_point;
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

Point2f Table::getNormalizedCoords(cv::Point2f ballPos){
    Table &table = *this;
    currPos = table.getBallCoords();
    if((currPos.x < table.top_left.x) || (currPos.x > table.bottom_right.x)){
       return cv::Point2f(-1,-1); 
    }
    if((currPos.y < table.top_left.y) || (currPos.y > table.bottom_right.y)){
       return cv::Point2f(-1,-1); 
    }
    cv::Point2f retPos;
    retPos.x = (2 * currPos.x) / (table.bottom_right.x - table.top_left.x);
    retPos.y = currPos.y / (table.bottom_right.y - table.top_left.y);
    #ifdef TESTTABLE
        cv::Mat in = cam.readColor();
        cv::circle(in, table.top_left, 3, cv::Scalar(0,0,225), -1); 
        cv::circle(in, table.bottom_right, 3, cv::Scalar(0,0,225), -1); 
        cv::circle(in, retPos, 3, cv::Scalar(0,225,225), -1); 
        cv::imshow("Table and Ball", in)
    #endif
    return currPos;
}

Point2f Table::getBallCoords(){
    Table &table = *this;
    cv::Mat in, bin;
    in = cam.readColor();
	bin = table.colTrack.filterImage(in, 164, 89, 175, 22, 255, 255);
        table.conTrack.findContours(bin);
    cv::Point ballCenter = table.conTrack.findBallCenter();
    #ifdef TESTTABLE
        cv::Mat cons = table.conTrack.drawContours(in.rows, in.cols);
        cv::imshow("orig", in);
        cv::imshow("filtered", bin);
        cv::circle(cons, table.top_left, 3, cv::Scalar(0,0,225), -1);
        cv::imshow("contours", cons);
    #endif
    return ballCenter;
}