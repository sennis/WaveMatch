// Standard C++ includes
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// OpenCV includes
#include <cv.h>
#include <highgui.h>


	IplImage* frame;

	IplImage* selection;

	CvCapture* capture;
	
	CvPoint point;

	int drag = 0;
	int key = 0;

void mouseHandler(int event, int x, int y, int flags, void*param){
	
	//IplImage* frame = (IplImage*) param;
	
	//user presses left button
	if (event == CV_EVENT_LBUTTONDOWN && !drag)
	{
		point = cvPoint(x,y);
		drag = 1;
	}

	//user drags the mouse
	if (event == CV_EVENT_MOUSEMOVE && drag)
	{
		selection = cvCloneImage(frame);
		cvRectangle(selection, point, cvPoint(x,y), CV_RGB(0, 255, 0), 1, 8, 0);
		cvCopy(selection, frame, NULL);
		cvShowImage("View", selection);
	}

	//user releases the left button
	else if (event == CV_EVENT_LBUTTONUP && drag)
	{
		selection = cvCloneImage(frame);
		
		//top left to bottom right selection
		if ( x - point.x > 0 && y - point.y > 0 ){
			cvSetImageROI(selection, cvRect(point.x,point.y,x-point.x,y-point.y));
		}

		
		//bottom left to top right selection
		else if ( x - point.x > 0 && y - point.y < 0) {
			cvSetImageROI(selection, cvRect(point.x, y ,x-point.x, point.y - y));
		}


		//top right to bottom left selection
		else if ( x - point.x < 0 && y - point.y > 0){
			cvSetImageROI(selection, cvRect(x, point.y, point.x - x, y -point.y));

		}

		//bottom right to top left selection
		else if (  x - point.x < 0 && y - point.y < 0){
			cvSetImageROI(selection, cvRect(x, y, point.x - x, point.y - y));

		}



		//cvNot(selection, selection);
		cvShowImage("ROI", selection);
		cvResetImageROI(selection);
		cvCopy(selection, frame, NULL);
		cvShowImage("View", selection);
		drag = 0;
	}

	//user click right button, reset drawing
	else if (event == CV_EVENT_RBUTTONUP){

		cvShowImage("View", frame);
		
		drag = 0;

	}


}
	
	
	
int main(int argc, char *argv[])
{
	capture = cvCreateCameraCapture(1);

	assert(capture);
	
	cvNamedWindow("View");
	
	cvNamedWindow("ROI");

			//cvShowImage("View",frame);
	//IplImage* frame = cvQueryFrame(capture);
		
		cvSetMouseCallback("View", mouseHandler, NULL);//(void*) frame);

		while(key != 'q'){

			frame = cvQueryFrame(capture);
			cvShowImage("View", frame);
			key = cvWaitKey(1);

		}

		cvDestroyWindow("ROI");
		cvDestroyWindow("View");
		

		cvReleaseImage(&frame);
		cvReleaseImage(&selection);

		return 0;


}