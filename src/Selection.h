#ifndef _H_SELECTION_RIO_
#define _H_SELECTION_RIO_

class Selection
{
public:
	Selection();
	static void mouseHandler(int event, int x, int y, int flags, void*param);
	static cv::Rect Selection::getRect(cv::Point p1, cv::Point p2);
	static cv::Mat Selection::computeHistogram(cv::Mat src);
	static void Selection::findExtrema(cv::Mat src, int* histogram, int scan_region, int* output);
	static void Selection::interpolateWave(cv::Mat src, int* toFill);
	static void Selection::cosInterp(int x1, int x2, int y1, int y2);
	static void Selection::sinInterp(int x1, int x2, int y1, int y2);


};






#endif // _H_SELECTION_RIO_