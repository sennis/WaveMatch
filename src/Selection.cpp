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
	
	std::printf("Depth of src: %d", src.channels());
	cv::Mat src_gray;
	cv::cvtColor(src, src_gray, CV_RGB2GRAY);
	std::printf("Depth of src_gray: %d", src_gray.channels());

	//cv::imshow("hist attempt", src_gray);

	//cv::waitKey(0);

	//number of bins
	int histSize = src_gray.cols;

	float intensity_range[] = { 0 , 256 };
	const float* histRange = {intensity_range};
	bool uniform = true; bool accumulate = false;

	cv::Mat hist;

	calcHist( &src_gray, 1, 0, cv::Mat() , hist, 1, &histSize, &histRange, uniform, accumulate);

	int hist_w = src_gray.cols; int hist_h = 256;
	//bin width should be one pixel
	
	int bin_w = 1;
	//int bin_w = cvRound( (double) hist_w/histSize );

	//double maxVal = 0;
	//minMaxLoc( hist, 0, &maxVal, 0 ,0);

	/*
	int scale = 5;
	cv::Mat histImage = cv::Mat::zeros(src_gray.cols * scale, 256 * scale, CV_8UC3); 
	*/

	/*
	
	for (int i = 1; i < histSize; i++){
		//float binVal = hist.at<float>(i);
		//int intensity = cvRound(binVal*255/maxVal);
		cv::line(histImage, cv::Point((i-1)*scale, 256 - cvRound(hist.at<float>(i-1)) ),
			cv::Point(i*scale, 256 - cvRound(hist.at<float>(i) ) ),
			cv::Scalar( 255, 0 , 0), 2, 8, 0);

		cv::imshow("hist attempt", histImage);
		cv::waitKey(0);

	}
	*/



	cv::Mat histImage ( hist_h, hist_w, CV_8UC3, cv::Scalar(0 , 0, 0));

	cv::normalize(hist, hist, 0, 256, cv::NORM_MINMAX, -1, cv::Mat());

	for (int i = 1; i < histSize; i++)
	{
		std::printf("Line from (%d, %d) to (%d,%d)", i - 1, cvRound(hist.at<float>(i-1)), i, cvRound(hist.at<float>(i)) );
		line ( histImage, cv::Point((i-1), cvRound(hist.at<float>(i-1)) ),
			cv::Point((i), cvRound(hist.at<float>(i)) ),
			cv::Scalar( 255, 0, 0), 1, 8, 0 );

		cv::imshow("hist attempt", histImage);
		cv::waitKey(0);

	}


	
	return histImage;


}


	
int main(int argc, char *argv[])
{
	cv::VideoCapture capture(1);
	
	cv::Mat frame;
	
	cv::namedWindow("View");
	cv::namedWindow("hist attempt", CV_WINDOW_AUTOSIZE);
		
	cvSetMouseCallback("View", Selection::mouseHandler, NULL);//(void*) frame);
	
	int key = 0;
		
	while(key != 'q'){

			capture.read(frame);

			if(roiP1 != notRoi && roiP2 != notRoi){
			  
			  cv::Rect roi = Selection::getRect(roiP1,roiP2);

			  cv::Mat image_roi = frame(roi);

			  if(computeHist){
						
					cv::Mat histImage = Selection::computeHistogram(image_roi);

						cv::imshow("hist attempt", histImage);
				
						cv::waitKey(0);

			  }

			  cv::rectangle(frame, roi, CV_RGB(0, 255, 0), 1, 8, 0);
			  
		
			}

			cv::imshow("View", frame);

			key = cv::waitKey(1);

		}
		cv::destroyWindow("View");

		return 0;


}