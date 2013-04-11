// Standard C++ includes
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <algorithm>

// OpenCV includes
#include <cv.h>
#include <highgui.h>

#include "Selection.h"



  
  int drag = 0;
	
	static cv::Point roiP1;
	static cv::Point roiP2;

	static cv::Point notRoi;


	


Selection::Selection(){
	roiP1.x = -1;
	roiP1.y = -1;
	roiP2.x = -1;
	roiP2.y = -1;

	notRoi.x = -1;
	notRoi.y = -1;
}

void Selection::mouseHandler(int event, int x, int y, int flags, void*param){


	//user presses left button
	if (event == CV_EVENT_LBUTTONDOWN && !drag)
	{
		roiP1.x = x;
		roiP1.y = y;
		roiP2.x = x;
		roiP2.y = y;

		drag = 1;
	}

	//user drags the mouse
	else if (event == CV_EVENT_MOUSEMOVE && drag)
	{
		roiP2.x = x;
		roiP2.y = y;

	}

	//user releases the left button
	else if (event == CV_EVENT_LBUTTONUP && drag)
	{
		drag = 0;
	}

	//user click right button, reset drawing
	else if (event == CV_EVENT_RBUTTONUP){

	roiP1.x = -1;
	roiP1.y = -1;
	roiP2.x = -1;
	roiP2.y = -1;
	
	drag = 0;

	}


}
	
cv::Rect Selection::getRect(cv::Point p1, cv::Point p2){

  cv::Rect ret;
  ret.x = std::min(p1.x, p2.x);
  ret.y = std::min(p1.y, p2.y);
  ret.width = std::max(p1.x, p2.x) - ret.x;
  ret.height = std::max(p1.y, p2.y) - ret.y;

  return ret;

}




	
int main(int argc, char *argv[])
{
	cv::VideoCapture capture(1);
	
	cv::Mat frame;
	
	cv::namedWindow("View");

		cvSetMouseCallback("View", Selection::mouseHandler, NULL);//(void*) frame);
	int key = 0;
		while(key != 'q'){

			capture.read(frame);

			if(roiP1 != notRoi && roiP2 != notRoi){
			  
			  cv::Rect roi = Selection::getRect(roiP1,roiP2);

			  cv::Mat image_roi = frame(roi);

			  //computeHistogram()

			  cv::rectangle(frame, roi, CV_RGB(0, 255, 0), 1, 8, 0);
			  
		
			}

			cv::imshow("View", frame);

			key = cv::waitKey(1);

		}
		cv::destroyWindow("View");

		return 0;


}