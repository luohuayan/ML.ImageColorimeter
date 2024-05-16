#pragma once
#include <opencv2\opencv.hpp>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>
#include"Utl.h"
#include <ctime>
#include <string>
using namespace std;
struct  __declspec(dllexport) ImageData
{
	int bin = 1;
	int bitdepth = 16;
	cv::Rect cropArea = cv::Rect(-1, -1, 0, 0);
	cv::Mat img;
	cv::Size rawSize;
	double expt = 0;	
};
//class TaskEventCallback
//{
//	//virtual ImageData ffc(ImageData&);
//};

class  __declspec(dllexport) CalibrationProcess
{
public:    
	//virtual LinkTask();
	virtual ImageData process(ImageData& img)=0;
//	void setEventCallback(TaskEventCallback* aEventCbk);
//	TaskEventCallback* getEventCallback();
};
