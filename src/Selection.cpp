// Standard C++ includes
#include <stdio.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <string.h>
#include <algorithm>

#include <memory>

// OpenCV includes
#include <cv.h>
#include <highgui.h>

#include "Selection.h"

/*
1. normalizing in the right spot?
2. where to subtract iMin?
3. do i want equal values to be able to be extrema?
4. release arrays?

*/

  
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

cv::Mat Selection::computeHistogram(cv::Mat src, cv::Mat iMin, cv::Mat iMax){
	cv::Mat src_gray;
	cv::cvtColor(src, src_gray, CV_RGB2GRAY);

	//int* histogram = new int[src.cols];
	auto histogram = std::unique_ptr<int[]>( new int[src.cols] );
	
	//int* iMinHist = new int[src.cols];
	auto iMinHist = std::unique_ptr<int[]>( new int[src.cols]);


	for (int i = 0; i < src.cols; i++){
	  //capture values from cross section for I
	  histogram[i] = (int) src_gray.at<uchar>(src.rows/2, i);
	  //capture values from cross section for Imin
	  iMinHist[i] = (int) iMin.at<uchar>(src.rows/2, i);


	  //SUBTRACT IMIN
	  if (iMinHist[i] <= histogram[i]){
		  histogram[i] -= iMinHist[i];
	  }
	  else{
		histogram[i] = 0;
	  }

	}

	 //for(int i = 0; i < src_gray.cols; i++)
       // std::cout<<histogram[i]<<" ";

	 int hist_width = src_gray.cols;
	 int hist_height = src_gray.rows;
 
	 
	 //WILL THIS MESS UP DATA?????? NOW I AM NORMALIZING IN DRAWING FUNCTIONS
	 //NORMALIZE histogram between 0 and hist_height
	/*for(int i = 0; i < hist_width; i++){
		histogram[i] = cvRound(((double)histogram[i]/255*hist_height));
	}*/


	//for(int i = 0; i < src_gray.cols; i++)
      //  std::cout<<histogram[i]<<" ";



	//Draw line for histogram
	for (int i = 1; i < hist_width; i++){
			cv::line(src, cv::Point((i-1), hist_height - cvRound(((double)histogram[i-1]/255.0*hist_height))),
			cv::Point(i, hist_height - cvRound(((double)histogram[i]/255.0*hist_height))),
			cv::Scalar( 255, 100, 100), 1, 8, 0);

		
	}
		
	//float* ideal_wave = new float[hist_width];
	auto ideal_wave = std::unique_ptr<float[]>(new float[hist_width]);


	Selection::findExtrema(src, histogram.get(), 45, ideal_wave.get());
	
	/*
	for(int i = 0; i < src_gray.cols; i++)
        std::cout<<ideal_wave[i]<<" ";
		*/

	//draw mins and maxes
	for (int i = 0;	i < hist_width; i++){	
		if (ideal_wave[i] != -1)
			cv::circle(src, cv::Point(i, hist_height - cvRound(((double)ideal_wave[i]/255.0*hist_height))), 3, cv::Scalar(0, 0, 255), -1, 8, 0);
	}

	Selection::interpolateWave(src, ideal_wave.get());

	/*
	for(int i = 0; i < src_gray.cols; i++)
        std::cout<<ideal_wave[i]<<" ";
		*/
	//draw ideal wave
	for (int i = 1; i < hist_width; i++){
			cv::line(src, cv::Point((i-1), hist_height - cvRound(((double)ideal_wave[i-1]/255.0*hist_height)) ),
			cv::Point(i, hist_height - cvRound(((double)ideal_wave[i]/255*hist_height))),
			cv::Scalar( 200, 255, 0), 1, 8, 0);

		
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

	/*
	for (int i = 0; i < hist_width; i++){
	std::cout<<histogram[i]<<" ";
	}*/


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

					//DO I WANT THIS PART??? (This lets two maxes/mins be in the same region if they are equal)
					/*else{
						possMin = false;
						possMax = false;
						continue;
					}*/
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

					//DO I WANT THIS PART???
					/*else{
						possMin = false;
						possMax = false;
						continue;
					}*/
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
	
	/*
	for (int i = 0; i < hist_width; i++){
		if (toFill[i] != -1.0){
			std::cout<<i<<" ";
		std::cout<<toFill[i]<<" ";
		}
	}*/

	for (int i = 0; i < hist_width; i++){
		if (toFill[i] != -1.0){
			if (iExtrema1 == -1)
				iExtrema1 = i;
			else if (iExtrema2 == -1)
				iExtrema2 = i;
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
  float yRange = y2 - y1;

  for (int i = x1 + 1; i < x2; i++){

	 // toFill[i] = (yRange/2.0) * direction * (cos(((2*M_PI*(i - x1))/(2 * xRange * 1.0)))) + (yRange/2.0) + y1;
	toFill[i] = -1 * (yRange/2.0) * (cos(((2*M_PI*(i - x1))/(2 * xRange * 1.0)))) + (yRange/2.0) + y1;
  }

}



int main(int argc, char *argv[])
{
	//cv::VideoCapture capture(1);
	
	auto camera = shared_ptr<lens::ICamera>(new lens::PointGreyCamera());
	camera->open();

	bool iMinCaptured = false;
	bool iMaxCaptured = false;

	cv::Mat frame;

	cv::Mat iMin;
	cv::Mat iMax;
	
	cv::namedWindow("View");
		
	cvSetMouseCallback("View", Selection::mouseHandler, NULL);
	
	int key = 0;
		
	while(key != 'q'){
		
		//ASK USER TO HIT ENTER TO CAPTURE MIN IMAGE
		while (!iMinCaptured){
			
			//wait for enter key
			while ( 13 != cvWaitKey( 15 ) )
			{	
		
			//capture.read(frame);
			

			frame = cv::Mat(camera->getFrame());
			cv::putText(frame, "Press <Enter> to capture minimum intensity image.", cv::Point( 50, 50 ), cv::FONT_HERSHEY_TRIPLEX, .5, cv::Scalar(255, 255, 255) );
			
			
			cv::imshow("View", frame);
			}
		
			//CAPTURE MIN IMAGE
			//capture.read(iMin);
			
			iMin = cv::Mat(camera->getFrame());
			iMinCaptured = true;
		
		}

		//ASK USER TO HIT ENTER TO CAPTURE MAX IMAGE

			//capture.read(frame);

			frame = cv::Mat(camera->getFrame());

			cv::Mat image_roi;

			if(roiP1 != notRoi && roiP2 != notRoi){
			  
			  cv::Rect roi = Selection::getRect(roiP1,roiP2);

			  image_roi = frame(roi);

			  if(computeHist){
						
				Selection::computeHistogram(image_roi, iMin, iMax);

			  }

			  cv::rectangle(frame, roi, CV_RGB(0, 255, 0), 2, 8, 0);
			  
			}
			
			cv::imshow("View", frame);

			key = cv::waitKey(1);

		}
		cv::destroyWindow("View");

		return 0;


}