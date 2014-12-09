#include "stdafx.h"
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

string intToString(int number){
	//this function has a number input and string output
	std::stringstream ss;
	ss << number;
	return ss.str();
}

void searchForMovement(Mat thresholdImage, Mat &cameraFeed){
	bool objectDetected=false;
	Mat temp;
	thresholdImage.copyTo(temp);
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	if(contours.size()>0)objectDetected=true;
	else objectDetected = false;

	if(objectDetected){
		vector< vector<Point> > largestContourVec;
		largestContourVec.push_back(contours.at(contours.size()-1));
		objectBoundingRectangle = boundingRect(largestContourVec.at(0));
		int xpos = objectBoundingRectangle.x+objectBoundingRectangle.width/2;
		int ypos = objectBoundingRectangle.y+objectBoundingRectangle.height/2;
		theObject[0] = xpos , theObject[1] = ypos;
	}
	//make some temp x and y variables so we dont have to type out so much
	int x = theObject[0];
	int y = theObject[1];
	circle(cameraFeed,Point(x,y),20,Scalar(0,255,0),2);
	line(cameraFeed,Point(x,y),Point(x,y-25),Scalar(0,255,0),2);
	line(cameraFeed,Point(x,y),Point(x,y+25),Scalar(0,255,0),2);
	line(cameraFeed,Point(x,y),Point(x-25,y),Scalar(0,255,0),2);
	line(cameraFeed,Point(x,y),Point(x+25,y),Scalar(0,255,0),2);
	putText(cameraFeed,"Tracking object at (" + intToString(x)+","+intToString(y)+")",Point(x,y),1,1,Scalar(255,0,0),2);
}

int main(){
	Mat  grayImage1,grayImage2;
	Mat  differenceImage;
	Mat  thresholdImage;
	CvCapture* capture;
	capture = cvCaptureFromCAM(1);
	IplImage* frame1 = cvQueryFrame( capture );
	while(1){
		IplImage* frame2 = cvQueryFrame( capture );
		cv::cvtColor(frame1,grayImage1,COLOR_BGR2GRAY);
		cv::cvtColor(frame2,grayImage2,COLOR_BGR2GRAY);
		cv::absdiff(grayImage1,grayImage2,differenceImage);
		cv::threshold(differenceImage,thresholdImage,SENSITIVITY_VALUE,255,THRESH_BINARY);
		cv::imshow("Difference Image",differenceImage);
		cv::imshow("Threshold Image", thresholdImage);	
		cv::blur(thresholdImage,thresholdImage,cv::Size(BLUR_SIZE,BLUR_SIZE));
		cv::threshold(thresholdImage,thresholdImage,SENSITIVITY_VALUE,255,THRESH_BINARY);
		searchForMovement(thresholdImage, frame1);	
		imshow("Final Threshold Image",thresholdImage);
		rectangle(frame1,Point(0,460),Point(200,480),Scalar(255,255,255),-1);
		imshow("Frame1",frame1);
		frame1=frame2;
	}
	return 0;
}