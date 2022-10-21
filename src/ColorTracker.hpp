/* James Hubbard
 * ColorTracker.hpp
 * Takes a range of HSV colors and returns object locations
 */

#ifndef COLOR_TRACKER_HPP
#define COLOR_TRACKER_HPP

#include<iostream>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

class ColorTracker {
public:
	ColorTracker();
	~ColorTracker();

	cv::Mat filterImage(cv::Mat in, int hu, int su, int vu, int hl, int sl, int vl);
private:
};

#endif //COLOR_TRACKER_HPP
