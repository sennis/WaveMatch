// Standard C++ includes
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <algorithm>

// OpenCV includes
#include <cv.h>
#include <highgui.h>

#include "lens\ICamera.h"
#include "lens\OpenCVCamera.h"
#include "lens\PointGreyCamera.h"

#include "Selection.h"



  
  int drag = 0;

  bool computeHist = false;
	
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
		computeHist = false;

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
		computeHist = true;
		drag = 0;
	}

	//user click right button, reset drawing
	else if (event == CV_EVENT_RBUTTONUP){


	roiP1.x = -1;
	roiP1.y = -1;
	roiP2.x = -1;
	roiP2.y = -1;
	
	computeHist = false;
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

cv::Mat Selection::computeHistogram(cv::Mat src){
	cv::Mat src_gray;
	cv::cvtColor(src, src_gray, CV_RGB2GRAY);

	int* histogram = new int[src.cols];


	for (int i = 0; i < src.cols; i++){
		
		//Create Mat of the selected column
		cv::Mat thisColumn = src_gray.col(i);

		int sum = 0;//, count = 0;
		for (int j = 0; j < src.rows; j++){
			sum += (int) thisColumn.at<uchar>(j, 0);
			//count++;
		}

		//put the average intensity of the column into the histogram array
		histogram[i] = cvRound( sum / (src_gray.rows * 1.0));
		//printf("Element at %d is %d", i, histogram[i]); 
	}

	 //for(int i = 0; i < src_gray.cols; i++)
       // std::cout<<histogram[i]<<" ";

	 //draw histogram
	 int hist_width = src_gray.cols;
	 int hist_height = src_gray.rows;
 
	 //NORMALIZE histogram between 0 and hist_height
	for(int i = 0; i < hist_width; i++){
		histogram[i] = cvRound(((double)histogram[i]/255*hist_height));
	}
	//for(int i = 0; i < src_gray.cols; i++)
      //  std::cout<<histogram[i]<<" ";

	//Draw line for histogram
	for (int i = 1; i < hist_width; i++){
		cv::line(src, cv::Point((i-1), hist_height - histogram[i-1] ),
			cv::Point(i, hist_height - histogram[i]),
			cv::Scalar( 255, 100, 100), 2, 8, 0);

		
	}


	//RELEASE HISTOGRAM ARRAY????
	return src;

}


	
int main(int argc, char *argv[])
{
	//cv::VideoCapture capture(1);
	
	auto camera = shared_ptr<lens::ICamera>(new lens::PointGreyCamera());
	camera->open();


	cv::Mat frame;
	
	cv::namedWindow("View");
		
	cvSetMouseCallback("View", Selection::mouseHandler, NULL);
	
	int key = 0;
		
	while(key != 'q'){

			//capture.read(frame);

			frame = cv::Mat(camera->getFrame());

			cv::Mat image_roi;

			if(roiP1 != notRoi && roiP2 != notRoi){
			  
			  cv::Rect roi = Selection::getRect(roiP1,roiP2);

			  image_roi = frame(roi);

			  if(computeHist){
						
				Selection::computeHistogram(image_roi);

			  }

			  cv::rectangle(frame, roi, CV_RGB(0, 255, 0), 2, 8, 0);
			  
			}
			
			cv::imshow("View", frame);

			key = cv::waitKey(1);

		}
		cv::destroyWindow("View");

		return 0;


}