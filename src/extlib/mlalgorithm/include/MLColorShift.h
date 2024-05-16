#pragma once
#include <opencv2\opencv.hpp>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>
#include <stdlib.h>
#include <numeric> 
#include"CalibrationProcess.h"

class __declspec(dllexport)ColorShiftProcessor:public CalibrationProcess
{
public:
	/// <summary>
	/// Set the ColorShift offset parameter;
	/// </summary>
	/// <param name="point"> The color shift offset</param>
	void setOffset(cv::Point2d point);
	/// <summary>
	/// The ColorShift calibration process
	/// </summary>
	/// <param name="rawImage">The image taken by the camera including the setup parameters like bining,exposure time etc </param>
	/// <returns></returns>
	ImageData process(ImageData& rawImage);
private:
	/// <summary>
	/// The preColorShift process,according to the binning num to change the offset parameter;
	/// </summary>
	/// <param name="rawImage">The image taken by the camera including the setup parameters like bining,exposure time etc </param>
	void preColorShiftProcess(ImageData& rawImage);
	/// <summary>
	/// Sanity check arguments for ColorShift operators.This is called internally by the ColorShift functions.
	/// Violations will throw exceptions. 
	/// </summary>
	/// <param name="img">The input image</param>
	/// <param name="caller">Name of calling function for error reporting</param>
	void  ShiftProcessOperator_InputImageCheck(cv::Mat &img, const char* caller);
	cv::Point2d m_Offset;

};

