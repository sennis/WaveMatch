#ifndef _H_SELECTION_RIO_
#define _H_SELECTION_RIO_

class Selection
{
public:
	Selection();
	static void mouseHandler(int event, int x, int y, int flags, void*param);
	static cv::Rect Selection::getRect(cv::Point p1, cv::Point p2);
	static cv::Mat Selection::computeHistogram(cv::Mat src);
	static void Selection::findExtrema(cv::Mat src, int* histogram, int scan_region, float* output);
	static void Selection::interpolateWave(cv::Mat src, float* toFill);
	static void Selection::calcInterp(int x1, int x2, float y1, float y2, int direction, float* toFill);


};






#endif // _H_SELECTION_RIO_