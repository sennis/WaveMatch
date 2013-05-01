// Standard C++ includes
#include <stdio.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <string.h>
#include <algorithm>

// OpenCV includes
#include <cv.h>
#include <highgui.h>

//#include "lens\ICamera.h"
//#include "lens\OpenCVCamera.h"
//#include "lens\PointGreyCamera.h"

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
	  //capture values from cross section halfway down screen
	  histogram[i] = (int) src_gray.at<uchar>(src.rows/2, i);
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

	
	float* ideal_wave = new float[hist_width];

	Selection::findExtrema(src, histogram, 5, ideal_wave);
	
	//Draw line for histogram
	for (int i = 1; i < hist_width; i++){
			cv::line(src, cv::Point((i-1), hist_height - histogram[i-1] ),
			cv::Point(i, hist_height - histogram[i]),
			cv::Scalar( 255, 100, 100), 1, 8, 0);

		
	}
	
	//draw mins and maxes
	for (int i = 0;	i < hist_width; i++){	
		if (ideal_wave[i] != -1)
			cv::circle(src, cv::Point(i, hist_height - cvRound(ideal_wave[i])), 3, cv::Scalar(0, 0, 255), -1, 8, 0);
	}

	Selection::interpolateWave(src, ideal_wave);

	//draw ideal wave
	for (int i = 1; i < hist_width; i++){
			cv::line(src, cv::Point((i-1), hist_height - cvRound(ideal_wave[i-1]) ),
			cv::Point(i, hist_height - cvRound(ideal_wave[i])),
			cv::Scalar( 200, 255, 0), 2, 8, 0);

		
	}


	//RELEASE HISTOGRAM ARRAY????
	return src;

}


void Selection::findExtrema(cv::Mat src, int* histogram, int scan_region, float* output){
	
	
	//initialize all values of output to -1
	for (int i = 0; i < src.cols; i++){
		output[i] = -1.0;
	}



	int hist_width = src.cols;
	int hist_height = src.rows;

	int half_window = (scan_region - 1 ) / 2;

	for (int i = 0; i < hist_width; i++){
	std::cout<<histogram[i]<<" ";
	}


	for (int i = half_window; i < hist_width - half_window; i++){
				
			float thisValue = histogram[i];
			
			bool possMax = true, possMin = true;
			for (int j = i - half_window; j < i + half_window && (possMax || possMin); j++){

				//look at values to the left of the value
				if (j < i){
					if (thisValue > histogram[j]){
						possMin = false;
						continue;
					}
					else if (thisValue < histogram[j]){
						possMax = false;
						continue;
					}
					else{
						possMin = false;
						possMax = false;
						continue;
					}
				}
				//look at values to the right of the value
				else if (j > i){
					if (thisValue > histogram[j]){
						possMin = false;
						continue;
					}
					else if (thisValue < histogram[j]){
						possMax = false;
						continue;
					}
					else{
						possMin = false;
						possMax = false;
						continue;
					}
				}
				//continue because we are at the value
				else{
					continue;
				}
			


			}
			//if it has passed the test, store it in the output array
			if (possMin || possMax){
					output[i] = thisValue;
					//std::cout<<output[i]<<" ";
				}
	}


}

void Selection::interpolateWave(cv::Mat src, float* toFill){
	int iExtrema1 = -1;
	int iExtrema2 = -1;

	int hist_width = src.cols;
	int hist_height = src.rows;
	
	for (int i = 0; i < hist_width; i++){
	std::cout<<toFill[i]<<" ";
	}

	for (int i = 0; i < hist_width; i++){
		if (toFill[i] != -1.0){
			if (iExtrema1 == -1)
				iExtrema1 == i;
			else if (iExtrema2 == -1)
				iExtrema2 == i;
		}

		//if we have found extrema value, fill array
		if (iExtrema1 != -1 && iExtrema2 != -1){
			float valExtrema1 = toFill[iExtrema1];
			float valExtrema2 = toFill[iExtrema2];
		
			//if extrema 1 is a max and extrema 2 is a min, do a cosine inerpelation (direction 1)
			if (valExtrema1 > valExtrema2){
			calcInterp(iExtrema1, iExtrema2, valExtrema1, valExtrema2, 1, toFill);
			}
			//otherwise, extrema 1 is a min and extrema 2 is a max, so do a negative sin interpelation (direction -1
			else{
			calcInterp(iExtrema1, iExtrema2, valExtrema1, valExtrema2, -1, toFill);
			}

			iExtrema1 = iExtrema2;
			iExtrema2 = -1;
		}


	}






}
	

void Selection::calcInterp(int x1, int x2, float y1, float y2, int direction, float* toFill){
  int xRange = x2 - x1;
  int yRange = y2 - y1;

  for (int i = x1 + 1; i < x2; i++){
	toFill[i] = (yRange/2) * direction * ((float) cos((2*M_PI*i)/(2 * xRange))) + (yRange/2);
  }

}



int main(int argc, char *argv[])
{
	cv::VideoCapture capture(1);
	
	//auto camera = shared_ptr<lens::ICamera>(new lens::OpenCVCamera());
	//camera->open();


	cv::Mat frame;
	
	cv::namedWindow("View");
		
	cvSetMouseCallback("View", Selection::mouseHandler, NULL);
	
	int key = 0;
		
	while(key != 'q'){

			capture.read(frame);

			//frame = cv::Mat(camera->getFrame());

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