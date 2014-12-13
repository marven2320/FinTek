#include "stdafx.h"
#include <sstream>
#include <string>
#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include <opencv\cv.h>
#include "opencv2/opencv.hpp"
#include <time.h>
using namespace std;
using namespace cv;
#include <windows.h>
const static int SENSITIVITY_VALUE = 20;
const static int BLUR_SIZE = 10;
int theObject[2] = {0,0};
Rect objectBoundingRectangle = Rect(0,0,0,0);

//initial min and max HSV filter values.
////these will be changed using trackbars
//int H_MIN = 0;
//int H_MAX = 256;
//int S_MIN = 0;
//int S_MAX = 256;
//int V_MIN = 0;
//int V_MAX = 256;
////default capture width and height
//const int FRAME_WIDTH = 640;
//const int FRAME_HEIGHT = 480;
////max number of objects to be detected in frame
//const int MAX_NUM_OBJECTS=50;
////minimum and maximum object area
//const int MIN_OBJECT_AREA = 20*20;
//const int MAX_OBJECT_AREA = FRAME_HEIGHT*FRAME_WIDTH/1.5;

string intToString(int number){
	//this function has a number input and string output
	std::stringstream ss;
	ss << number;
	return ss.str();
}
//void drawObject(int x, int y,Mat &frame){
//
//	//use some of the openCV drawing functions to draw crosshairs
//	//on your tracked image!
//
//    //UPDATE:JUNE 18TH, 2013
//    //added 'if' and 'else' statements to prevent
//    //memory errors from writing off the screen (ie. (-25,-25) is not within the window!)
//
//	circle(frame,Point(x,y),20,Scalar(0,255,0),2);
//    if(y-25>0)
//    line(frame,Point(x,y),Point(x,y-25),Scalar(0,255,0),2);
//    else line(frame,Point(x,y),Point(x,0),Scalar(0,255,0),2);
//    if(y+25<FRAME_HEIGHT)
//    line(frame,Point(x,y),Point(x,y+25),Scalar(0,255,0),2);
//    else line(frame,Point(x,y),Point(x,FRAME_HEIGHT),Scalar(0,255,0),2);
//    if(x-25>0)
//    line(frame,Point(x,y),Point(x-25,y),Scalar(0,255,0),2);
//    else line(frame,Point(x,y),Point(0,y),Scalar(0,255,0),2);
//    if(x+25<FRAME_WIDTH)
//    line(frame,Point(x,y),Point(x+25,y),Scalar(0,255,0),2);
//    else line(frame,Point(x,y),Point(FRAME_WIDTH,y),Scalar(0,255,0),2);
//
//	putText(frame,intToString(x)+","+intToString(y),Point(x,y+30),1,1,Scalar(0,255,0),2);
//
//}
//void trackFilteredObject(int &x, int &y, Mat threshold, Mat &cameraFeed){
//
//	Mat temp;
//	threshold.copyTo(temp);
//	//these two vectors needed for output of findContours
//	vector< vector<Point> > contours;
//	vector<Vec4i> hierarchy;
//	//find contours of filtered image using openCV findContours function
//	findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );
//	//use moments method to find our filtered object
//	double refArea = 0;
//	bool objectFound = false;
//	if (hierarchy.size() > 0) {
//		int numObjects = hierarchy.size();
//        //if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
//        if(numObjects<MAX_NUM_OBJECTS){
//			for (int index = 0; index >= 0; index = hierarchy[index][0]) {
//
//				Moments moment = moments((cv::Mat)contours[index]);
//				double area = moment.m00;
//
//				//if the area is less than 20 px by 20px then it is probably just noise
//				//if the area is the same as the 3/2 of the image size, probably just a bad filter
//				//we only want the object with the largest area so we safe a reference area each
//				//iteration and compare it to the area in the next iteration.
//                if(area>MIN_OBJECT_AREA && area<MAX_OBJECT_AREA && area>refArea){
//					x = moment.m10/area;
//					y = moment.m01/area;
//					objectFound = true;
//					refArea = area;
//				}else objectFound = false;
//
//
//			}
//			//let user know you found an object
//			if(objectFound ==true){
//				putText(cameraFeed,"Tracking Object",Point(0,50),2,1,Scalar(0,255,0),2);
//				//draw object location on screen
//				drawObject(x,y,cameraFeed);}
//
//		}else putText(cameraFeed,"TOO MUCH NOISE! ADJUST FILTER",Point(0,50),1,2,Scalar(0,0,255),2);
//	}
//}
void searchForMovement(Mat thresholdImage, Mat &cameraFeed){
	//notice how we use the '&' operator for the cameraFeed. This is because we wish
	//to take the values passed into the function and manipulate them, rather than just working with a copy.
	//eg. we draw to the cameraFeed in this function which is then displayed in the main() function.
	bool objectDetected=false;
	Mat temp;
	thresholdImage.copyTo(temp);
	//these two vectors needed for output of findContours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	//findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );// retrieves all contours
	findContours(temp,contours,hierarchy,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE );// retrieves external contours

	//if contours vector is not empty, we have found some objects
	if(contours.size()>0)objectDetected=true;
	else objectDetected = false;

	if(objectDetected){
		//the largest contour is found at the end of the contours vector
		//we will simply assume that the biggest contour is the object we are looking for.
		vector< vector<Point> > largestContourVec;
		largestContourVec.push_back(contours.at(contours.size()-1));
		//make a bounding rectangle around the largest contour then find its centroid
		//this will be the object's final estimated position.
		objectBoundingRectangle = boundingRect(largestContourVec.at(0));
		int xpos = objectBoundingRectangle.x+objectBoundingRectangle.width/2;
		int ypos = objectBoundingRectangle.y+objectBoundingRectangle.height/2;

		//update the objects positions by changing the 'theObject' array values
		theObject[0] = xpos , theObject[1] = ypos;
	}
	//make some temp x and y variables so we dont have to type out so much
	int x = theObject[0];
	int y = theObject[1];
	//draw some crosshairs on the object
	circle(cameraFeed,Point(x,y),20,Scalar(0,255,0),2);
	line(cameraFeed,Point(x,y),Point(x,y-25),Scalar(0,255,0),2);
	line(cameraFeed,Point(x,y),Point(x,y+25),Scalar(0,255,0),2);
	line(cameraFeed,Point(x,y),Point(x-25,y),Scalar(0,255,0),2);
	line(cameraFeed,Point(x,y),Point(x+25,y),Scalar(0,255,0),2);
	putText(cameraFeed,"Tracking object at (" + intToString(x)+","+intToString(y)+")",Point(x,y),1,1,Scalar(255,0,0),2);
	imshow("Final Threshold Image",thresholdImage);
	rectangle(cameraFeed,Point(0,460),Point(200,480),Scalar(255,255,255),-1);
}

int main(int argc, char* argv[]){
	Mat  grayImage1,grayImage2, frame1;
	Mat  differenceImage;
	Mat  thresholdImage;
	//int x=0, y=0;
	CvCapture* capture;
	capture = cvCaptureFromCAM(1);
	IplImage* f1 = cvQueryFrame(capture);
	cv::Mat matf1(f1,false);
	matf1.copyTo(frame1);

	while(1){
		IplImage* f2 = cvQueryFrame( capture );
		cv::Mat frame2(f2,false);
		cv::imshow("frame1",frame1);
		cv::imshow("frame2",frame2);
		cv::cvtColor(frame1,grayImage1,COLOR_BGR2GRAY);
		cv::cvtColor(frame2,grayImage2,COLOR_BGR2GRAY);
		cv::absdiff(grayImage1,grayImage2,differenceImage);
		cv::threshold(differenceImage,thresholdImage,SENSITIVITY_VALUE,255,THRESH_BINARY);
		cv::imshow("Difference Image",differenceImage);
		cv::imshow("Threshold Image", thresholdImage);	
		cv::blur(thresholdImage,thresholdImage,cv::Size(BLUR_SIZE,BLUR_SIZE));
		cv::threshold(thresholdImage,thresholdImage,SENSITIVITY_VALUE,255,THRESH_BINARY);
		//trackFilteredObject(x, y, thresholdImage, frame1);
		/*searchForMovement(thresholdImage, frame1);
		imshow("Final Threshold Image",thresholdImage);
		rectangle(frame1,Point(0,460),Point(200,480),Scalar(255,255,255),-1);
		imshow("Frame1",frame1);*/
		//frame1=frame2;*/
		frame2.copyTo(frame1);
		cvWaitKey(10);
	}
	return 0;
}