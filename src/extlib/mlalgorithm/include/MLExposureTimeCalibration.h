#pragma once
#include <opencv2\opencv.hpp>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>
#include"CalibrationProcess.h"
class __declspec(dllexport)ExposureTimeProcessor:public CalibrationProcess
{
public:
	/// <summary>
	///  The ExposureTime  calibration process;
	/// </summary>
	/// <param name="img">The image taken by the camera including the setup parameters like bining,exposure time etc</param>
	/// <returns></returns>
	ImageData  process(ImageData& img);
private:
	/// <summary>
	/// Sanity check arguments for FFC operators.This is called internally by the FFC functions.
	/// Violations will throw exceptions 
	/// </summary>
	/// <param name="img">The input image</param>
	/// <param name="caller">Name of calling function for error reporting</param>
	void  ExposureTimeOperator_InputImageCheck(cv::Mat& img, const char* caller);


};

