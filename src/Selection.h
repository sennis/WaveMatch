#ifndef _H_SELECTION_RIO_
#define _H_SELECTION_RIO_

class Selection
{
public:
	Selection();
	static void mouseHandler(int event, int x, int y, int flags, void*param);
	static cv::Rect Selection::getRect(cv::Point p1, cv::Point p2);
	static cv::Mat Selection::computeHistogram(cv::Mat src);


};






#endif // _H_SELECTION_RIO_